import os
from PIL import Image
import numpy as np

# Configuration
INPUT_IMAGE_FILE = 'clouds.png'
OUTPUT_HEADER_FILE = 'clouds.h'
TILE_WIDTH = 16
TILE_HEIGHT = 16
BPP = 2
VRAM_TILES_WIDTH = 16  # Width of VRAM tilemap in 8x8 tiles (for a 256px wide screen)

def snes_color_to_rgb(snes_color):
    """Converts a 15-bit BGR SNES color to 24-bit RGB."""
    b = (snes_color & 0x7C00) >> 10
    g = (snes_color & 0x03E0) >> 5
    r = (snes_color & 0x001F)
    r = (r << 3) | (r >> 2)
    g = (g << 3) | (g >> 2)
    b = (b << 3) | (b >> 2)
    return (r, g, b)

def rgb_to_snes_color(r, g, b):
    """Converts a 24-bit RGB color to 15-bit BGR SNES color format."""
    r_snes = r >> 3
    g_snes = g >> 3
    b_snes = b >> 3
    return (b_snes << 10) | (g_snes << 5) | r_snes

def to_snes_2bpp(tile_8x8, palette_map):
    """Converts an 8x8 tile to SNES 2bpp format."""
    pixels = np.array(tile_8x8, dtype=int).flatten()
    
    # Map pixels to palette indices
    mapped_pixels = np.array([palette_map.get(p, 0) for p in pixels], dtype=np.uint8)

    snes_data = bytearray(16)
    for y in range(8):
        row_pixels = mapped_pixels[y*8 : (y+1)*8]
        byte1 = 0
        byte2 = 0
        for i, pixel_index in enumerate(row_pixels):
            byte1 |= (pixel_index & 1) << (7 - i)
            byte2 |= ((pixel_index >> 1) & 1) << (7 - i)
        snes_data[y * 2] = byte1
        snes_data[y * 2 + 1] = byte2
    return snes_data

def main():
    """Main conversion function."""
    if not os.path.exists(INPUT_IMAGE_FILE):
        print(f"Error: Input image '{INPUT_IMAGE_FILE}' not found.")
        return

    # Load image and convert to paletted mode
    img = Image.open(INPUT_IMAGE_FILE)
    if img.mode != 'P':
        img = img.convert('P', palette=Image.ADAPTIVE, colors=4)

    # Palette processing
    palette = img.getpalette()
    colors = [(palette[i], palette[i+1], palette[i+2]) for i in range(0, len(palette), 3)]
    
    # Ensure black is the first color
    try:
        black_index = colors.index((0, 0, 0))
    except ValueError:
        print("Warning: Black (0,0,0) not found in palette. It will be added as color 0.")
        # Add black and truncate if necessary
        if (0,0,0) in colors:
             colors.remove((0,0,0))
        colors.insert(0, (0, 0, 0))
        if len(colors) > 4:
            colors = colors[:4]

    if black_index != 0 and black_index < len(colors):
        colors.pop(black_index)
        colors.insert(0, (0, 0, 0))
    
    # Create a mapping from original palette index to the new one
    palette_map = {i: colors.index(tuple(img.getpalette()[i*3:i*3+3])) for i in range(len(img.getpalette())//3)}


    # SNES palette
    snes_palette = [rgb_to_snes_color(r, g, b) for r, g, b in colors]
    
    # Tile processing
    img_width, img_height = img.size
    tiles_x = img_width // TILE_WIDTH
    tiles_y = img_height // TILE_HEIGHT

    unique_tiles = []
    tile_map_indices = []
    
    # Extract 16x16 tiles and find unique ones
    for y in range(tiles_y):
        for x in range(tiles_x):
            box = (x * TILE_WIDTH, y * TILE_HEIGHT, (x + 1) * TILE_WIDTH, (y + 1) * TILE_HEIGHT)
            tile_16x16 = img.crop(box)
            tile_data = tile_16x16.tobytes()

            if tile_data not in [t.tobytes() for t in unique_tiles]:
                unique_tiles.append(tile_16x16)
            
            tile_map_indices.append([t.tobytes() for t in unique_tiles].index(tile_data))
    
    # Prepare tile data for SNES
    snes_tile_data = bytearray()
    
    # This list will hold the four 8x8 sub-tiles for each unique 16x16 tile
    unique_8x8_subtiles = []
    for tile_16x16 in unique_tiles:
        # TL, TR, BL, BR
        sub_tiles = [
            tile_16x16.crop((0, 0, 8, 8)),
            tile_16x16.crop((8, 0, 16, 8)),
            tile_16x16.crop((0, 8, 8, 16)),
            tile_16x16.crop((8, 8, 16, 16))
        ]
        unique_8x8_subtiles.append(sub_tiles)
    
    num_unique_16x16_tiles = len(unique_tiles)
    
    # Arrange 8x8 tiles in VRAM layout
    arranged_8x8_tiles = []
    for row_of_16x16 in range((num_unique_16x16_tiles * 2 + VRAM_TILES_WIDTH - 1) // VRAM_TILES_WIDTH):
        # Top row of 8x8s
        for i in range(VRAM_TILES_WIDTH // 2):
            tile_index = row_of_16x16 * (VRAM_TILES_WIDTH // 2) + i
            if tile_index < num_unique_16x16_tiles:
                arranged_8x8_tiles.append(unique_8x8_subtiles[tile_index][0]) # TL
                arranged_8x8_tiles.append(unique_8x8_subtiles[tile_index][1]) # TR
        # Bottom row of 8x8s
        for i in range(VRAM_TILES_WIDTH // 2):
            tile_index = row_of_16x16 * (VRAM_TILES_WIDTH // 2) + i
            if tile_index < num_unique_16x16_tiles:
                arranged_8x8_tiles.append(unique_8x8_subtiles[tile_index][2]) # BL
                arranged_8x8_tiles.append(unique_8x8_subtiles[tile_index][3]) # BR

    for tile_8x8 in arranged_8x8_tiles:
        snes_tile_data.extend(to_snes_2bpp(tile_8x8, palette_map))
    
    # Generate tilemap with correct indices
    # For a 1024x1024 image, we create a tilemap composed of 4 512x512 quadrants
    # stored sequentially: Top-Left, Top-Right, Bottom-Left, Bottom-Right.
    
    ordered_tile_map_indices = []
    if tiles_x == 64 and tiles_y == 64: # Specific logic for 1024x1024 image
        half_tiles_x = tiles_x // 2
        half_tiles_y = tiles_y // 2

        # Top-Left quadrant
        for y in range(half_tiles_y):
            start_index = y * tiles_x
            ordered_tile_map_indices.extend(tile_map_indices[start_index : start_index + half_tiles_x])
        
        # Top-Right quadrant
        for y in range(half_tiles_y):
            start_index = y * tiles_x + half_tiles_x
            ordered_tile_map_indices.extend(tile_map_indices[start_index : start_index + half_tiles_x])

        # Bottom-Left quadrant
        for y in range(half_tiles_y, tiles_y):
            start_index = y * tiles_x
            ordered_tile_map_indices.extend(tile_map_indices[start_index : start_index + half_tiles_x])
            
        # Bottom-Right quadrant
        for y in range(half_tiles_y, tiles_y):
            start_index = y * tiles_x + half_tiles_x
            ordered_tile_map_indices.extend(tile_map_indices[start_index : start_index + half_tiles_x])
    else:
        # Keep original order for other image sizes
        ordered_tile_map_indices = tile_map_indices
        
    tilemap_data = []
    for unique_idx in ordered_tile_map_indices:
        # For each unique 16x16 tile, we calculate the index of its top-left 8x8 sub-tile in VRAM.
        # The unique 16x16 tiles are arranged in VRAM in rows of 8 (8*16=128px wide).
        # A row of 8 16x16 tiles is stored as a 16x2 block of 8x8 tiles.
        # Top 8x8s (TL, TR) form one row of 16 8x8 tiles.
        # Bottom 8x8s (BL, BR) form the next row of 16 8x8 tiles.
        row_of_16x16_in_vram = unique_idx // (VRAM_TILES_WIDTH // 2)
        col_of_16x16_in_vram = unique_idx % (VRAM_TILES_WIDTH // 2)

        # Each row of 16x16 tiles takes up 2 rows of 8x8 tiles in VRAM
        snes_tile_index = (row_of_16x16_in_vram * 2) * VRAM_TILES_WIDTH + (col_of_16x16_in_vram * 2)
        tilemap_data.append(snes_tile_index)
        
    # Write C header file
    base_name = os.path.splitext(os.path.basename(OUTPUT_HEADER_FILE))[0]
    with open(OUTPUT_HEADER_FILE, 'w') as f:
        f.write(f'#ifndef {base_name.upper()}_H\n')
        f.write(f'#define {base_name.upper()}_H\n\n')

        # Palette
        f.write(f'#define {base_name}_palette_size {len(snes_palette)}\n')
        f.write(f'const unsigned short {base_name}_palette[{len(snes_palette)}] = {{\n    ')
        f.write(', '.join(f'0x{c:04X}' for c in snes_palette))
        f.write('\n};\n\n')

        # Tile data
        f.write(f'#define {base_name}_tiled_width {tiles_x}\n')
        f.write(f'#define {base_name}_tiled_height {tiles_y}\n')
        f.write(f'#define {base_name}_tile_count {len(arranged_8x8_tiles)}\n')
        f.write(f'#define {base_name}_tile_size {len(snes_tile_data)}\n')
        f.write(f'const unsigned char {base_name}_tiles[{len(snes_tile_data)}] = {{\n')
        for i in range(0, len(snes_tile_data), 16):
            f.write('    ' + ', '.join(f'0x{b:02X}' for b in snes_tile_data[i:i+16]) + ',\n')
        f.write('};\n\n')

        # Tilemap
        f.write(f'#define {base_name}_map_size {len(tilemap_data)}\n')
        f.write(f'const unsigned short {base_name}_map[{len(tilemap_data)}] = {{\n    ')
        f.write(', '.join(map(str, tilemap_data)))
        f.write('\n};\n\n')
        
        f.write(f'#endif // {base_name.upper()}_H\n')

    print(f"Conversion complete. Output written to '{OUTPUT_HEADER_FILE}'.")

if __name__ == '__main__':
    main()
