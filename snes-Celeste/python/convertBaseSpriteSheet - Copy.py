from PIL import Image
import os

arrMustBeObject = [8, 9, 10, 11, 12, 13, 14, 15, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 30, 31, 45,46,47, 60,62,64, 70,71,86,87, 96,97, 102, 118,119,120]

def convert_tile_to_2bpp(tile_image, palette):
    """Converts an 8x8 tile image to SNES 2bpp format."""
    tile_2bpp = bytearray(16)

    # Use RGB mode for palette processing
    tile_rgb = tile_image.convert('RGB')
    pixels = list(tile_rgb.getdata())

    # Create a mapping from palette color to palette index (0-3)
    # Using a dictionary for quick lookup of exact matches
    color_to_palette_index = {color: i for i, color in enumerate(palette)}

    pixel_indices = []
    for pixel_color in pixels:
        # Special handling for near-black colors to ensure they map to index 0
        r, g, b = pixel_color
        if r < 8 and g < 8 and b < 8:
            pixel_indices.append(0)
            continue

        if pixel_color in color_to_palette_index:
            pixel_indices.append(color_to_palette_index[pixel_color])
        else:
            # Fallback for colors not exactly in palette.
            # This is a simple-minded search for the closest color by Euclidean distance.
            min_dist = float('inf')
            best_idx = 0
            for i, pal_color in enumerate(palette):
                pr, pg, pb = pal_color
                dist = (r-pr)**2 + (g-pg)**2 + (b-pb)**2
                if dist < min_dist:
                    min_dist = dist
                    best_idx = i
            pixel_indices.append(best_idx)

    for y in range(8):
        row_pixels = pixel_indices[y*8 : y*8+8]

        bp0 = 0
        bp1 = 0

        for i, pixel_index in enumerate(row_pixels):
            bp0 |= (pixel_index & 1) << (7 - i)
            bp1 |= ((pixel_index >> 1) & 1) << (7 - i)

        tile_2bpp[y*2] = bp0
        tile_2bpp[y*2+1] = bp1

    return tile_2bpp

def convert_tile_to_4bpp(tile_image, palette):
    """Converts an 8x8 tile image to SNES 4bpp format (32 bytes)."""
    tile_4bpp = bytearray(32)
    tile_rgb = tile_image.convert('RGB')
    pixels = list(tile_rgb.getdata())

    # Create a mapping from palette color to palette index (0-15)
    color_to_palette_index = {color: i for i, color in enumerate(palette)}

    pixel_indices = []
    for pixel_color in pixels:
        # Special handling for near-black colors to ensure they map to index 0
        r, g, b = pixel_color
        if r < 8 and g < 8 and b < 8:
            pixel_indices.append(0)
            continue

        if pixel_color in color_to_palette_index:
            pixel_indices.append(color_to_palette_index[pixel_color])
        else:
            # Fallback for closest color
            min_dist = float('inf')
            best_idx = 0
            for i, pal_color in enumerate(palette):
                pr, pg, pb = pal_color
                dist = (r-pr)**2 + (g-pg)**2 + (b-pb)**2
                if dist < min_dist:
                    min_dist = dist
                    best_idx = i
            pixel_indices.append(best_idx)

    for y in range(8):
        row_pixels = pixel_indices[y*8 : y*8+8]

        bp0, bp1, bp2, bp3 = 0, 0, 0, 0

        for i, pixel_index in enumerate(row_pixels):
            bp0 |= (pixel_index & 1) << (7 - i)
            bp1 |= ((pixel_index >> 1) & 1) << (7 - i)
            bp2 |= ((pixel_index >> 2) & 1) << (7 - i)
            bp3 |= ((pixel_index >> 3) & 1) << (7 - i)

        offset = y * 2
        tile_4bpp[offset] = bp0
        tile_4bpp[offset + 1] = bp1
        tile_4bpp[offset + 16] = bp2
        tile_4bpp[offset + 17] = bp3

    return tile_4bpp

def convert_tile_to_2bpp_16x16(tile_image, palette):
    """
    Converts a 16x16 tile into four 8x8 2bpp SNES tiles,
    and returns top and bottom row data separately.
    """
    if tile_image.size != (16, 16):
        raise ValueError("Input tile image must be 16x16")

    tl = tile_image.crop((0, 0, 8, 8))
    tr = tile_image.crop((8, 0, 16, 8))
    bl = tile_image.crop((0, 8, 8, 16))
    br = tile_image.crop((8, 8, 16, 16))

    top_data = bytearray()
    top_data.extend(convert_tile_to_2bpp(tl, palette))
    top_data.extend(convert_tile_to_2bpp(tr, palette))

    bottom_data = bytearray()
    bottom_data.extend(convert_tile_to_2bpp(bl, palette))
    bottom_data.extend(convert_tile_to_2bpp(br, palette))

    return top_data, bottom_data

def convert_tile_to_4bpp_16x16(tile_image, palette):
    """
    Converts a 16x16 tile into four 8x8 4bpp SNES tiles,
    and returns top and bottom row data separately.
    """
    if tile_image.size != (16, 16):
        raise ValueError("Input tile image must be 16x16")

    tl = tile_image.crop((0, 0, 8, 8))
    tr = tile_image.crop((8, 0, 16, 8))
    bl = tile_image.crop((0, 8, 8, 16))
    br = tile_image.crop((8, 8, 16, 16))

    top_data = bytearray()
    top_data.extend(convert_tile_to_4bpp(tl, palette))
    top_data.extend(convert_tile_to_4bpp(tr, palette))

    bottom_data = bytearray()
    bottom_data.extend(convert_tile_to_4bpp(bl, palette))
    bottom_data.extend(convert_tile_to_4bpp(br, palette))

    return top_data, bottom_data

def convert_color_format(r, g, b):
    """
    Converts an RGB888 color to a 16-bit value based on the specified format:
    - bits 0-4: red intensity (5 bits)
    - bits 5-9: green intensity (5 bits)
    - bits 10-14: blue intensity (5 bits)
    """
    r_5bit = r >> 3
    g_5bit = g >> 3
    b_5bit = b >> 3

    return (b_5bit << 10) | (g_5bit << 5) | r_5bit


def main():
    image_filename = 'baseCelesteSpriteSheet.png'
    output_filename_h = 'sprite_data.h'

    # Check for Pillow installation
    try:
        from PIL import Image
    except ImportError:
        print("Pillow library not found. Please install it using: pip install Pillow")
        return

    if not os.path.exists(image_filename):
        print(f"Error: '{image_filename}' not found. Make sure it's in the same directory as the script.")
        print(f"Current directory is: {os.getcwd()}")
        return

    img = Image.open(image_filename)

    width, height = img.size
    if width % 8 != 0 or height % 8 != 0:
        print("Warning: Image dimensions are not a multiple of 8. Some parts may be cropped.")

    all_2bpp_data = bytearray()
    all_2bpp_palettes = []
    all_4bpp_data = bytearray()
    all_4bpp_palettes = []
    sprite_info = []

    tile_count = 0
    tile_count_2bpp = 0
    tile_count_4bpp = 0
    tiles_per_row = width // 8
    num_rows = height // 8

    row_top_2bpp_data = bytearray()
    row_bottom_2bpp_data = bytearray()
    row_top_4bpp_data = bytearray()
    row_bottom_4bpp_data = bytearray()

    row_2bpp_palettes = []
    row_4bpp_palettes = []
    row_sprite_info = []

    outputIndexTranslationTable = []

    for y in range(0, num_rows):
        for x in range(0, tiles_per_row):
            box = (x * 8, y * 8, (x + 1) * 8, (y + 1) * 8)
            tile_8x8 = img.crop(box)
            tile = tile_8x8.resize((16, 16), Image.NEAREST)

            # --- Per-tile palette generation and processing ---
            tile_rgb = tile.convert('RGB')
            colors = tile_rgb.getcolors(maxcolors=256)
            num_colors = len(colors) if colors else 0

            #Force objects to be 4bpp
            if tile_count in arrMustBeObject:
                num_colors = 5

            black = (0, 0, 0)

            if num_colors > 4:
                outputIndexTranslationTable.append(0)
            else:
                outputIndexTranslationTable.append(tile_count_2bpp)

            if num_colors > 4:
                tile_count_4bpp += 1
                # --- Process as 4bpp ---
                if colors:
                    # Sort by frequency (desc) then by color value (asc) for determinism
                    colors.sort(key=lambda item: (-item[0], item[1]))
                    palette_colors = [c[1] for c in colors]
                    if black in palette_colors:
                        palette_colors.remove(black)
                    # Ensure black is color 0
                    final_palette = [black] + palette_colors[:15]
                    while len(final_palette) < 16:
                        final_palette.append(black)
                    palette = final_palette
                else:
                    palette = [black] * 16

                top_data, bottom_data = convert_tile_to_4bpp_16x16(tile, palette)

                info = {"bpp": 4, "gfx_idx": (len(all_4bpp_data) + len(row_top_4bpp_data)) // 32, "pal_idx": len(all_4bpp_palettes) + len(row_4bpp_palettes)}
                row_top_4bpp_data.extend(top_data)
                row_bottom_4bpp_data.extend(bottom_data)
                row_4bpp_palettes.append(palette)
                row_sprite_info.append(info)

            else:
                # --- Process as 2bpp ---
                tile_count_2bpp += 1
                if colors:
                    # Sort by frequency (desc) then by color value (asc) for determinism
                    colors.sort(key=lambda item: (-item[0], item[1]))
                    palette_colors = [c[1] for c in colors]
                    if black in palette_colors:
                        palette_colors.remove(black)
                    # Ensure black is color 0
                    final_palette = [black] + palette_colors[:3]
                    while len(final_palette) < 4:
                        final_palette.append(black)
                    palette = final_palette
                else:
                    palette = [black] * 4

                top_data, bottom_data = convert_tile_to_2bpp_16x16(tile, palette)

                info = {"bpp": 2, "gfx_idx": (len(all_2bpp_data) + len(row_top_2bpp_data)) // 16, "pal_idx": len(all_2bpp_palettes) + len(row_2bpp_palettes)}
                row_top_2bpp_data.extend(top_data)
                row_bottom_2bpp_data.extend(bottom_data)
                row_2bpp_palettes.append(palette)
                row_sprite_info.append(info)

            tile_count += 1

    # Append row data to main arrays
    # Process 2bpp data in 16-tile segments
    for i in range(0, len(row_top_2bpp_data), 16 * 16):  # 16 tiles * 16 bytes per tile
        segment_top = row_top_2bpp_data[i:i + 16 * 16]
        segment_bottom = row_bottom_2bpp_data[i:i + 16 * 16]

        # Pad to 16 tiles if needed
        if len(segment_top) < 16 * 16:
            segment_top.extend([0] * (16 * 16 - len(segment_top)))
        if len(segment_bottom) < 16 * 16:
            segment_bottom.extend([0] * (16 * 16 - len(segment_bottom)))

        all_2bpp_data.extend(segment_top)
        all_2bpp_data.extend(segment_bottom)

    # Process 4bpp data in 16-tile segments
    for i in range(0, len(row_top_4bpp_data), 16 * 32):  # 16 tiles * 32 bytes per tile
        segment_top = row_top_4bpp_data[i:i + 16 * 32]
        segment_bottom = row_bottom_4bpp_data[i:i + 16 * 32]

        # Pad to 16 tiles if needed
        if len(segment_top) < 16 * 32:
            segment_top.extend([0] * (16 * 32 - len(segment_top)))
        if len(segment_bottom) < 16 * 32:
            segment_bottom.extend([0] * (16 * 32 - len(segment_bottom)))

        all_4bpp_data.extend(segment_top)
        all_4bpp_data.extend(segment_bottom)

    all_2bpp_palettes.extend(row_2bpp_palettes)
    all_4bpp_palettes.extend(row_4bpp_palettes)
    sprite_info.extend(row_sprite_info)

    # --- Write C Header File ---
    image_basename = os.path.basename(image_filename)
    # Sanitize basename for C header guard
    c_safe_basename = ''.join(c if c.isalnum() else '_' for c in image_basename)

    with open(output_filename_h, 'w') as f:
        f.write(f"// Header file for graphics and palettes from {image_basename}\n\n")

        header_guard = f"SPRITE_DATA_{c_safe_basename.upper()}_H"
        f.write(f"#ifndef {header_guard}\n")
        f.write(f"#define {header_guard}\n\n")

        # --- Struct Definition and Info Array ---
        f.write("typedef struct {\n")
        f.write("    unsigned char bpp;       // 2 or 4\n")
        f.write("    unsigned int gfx_index;  // Index into the corresponding graphics array\n")
        f.write("    unsigned int pal_index;  // Index into the corresponding palette array\n")
        f.write("} SpriteInfo;\n\n")

        f.write(f"// Info for each of the {len(sprite_info)} 16x16 tiles\n")
        f.write("const SpriteInfo sprite_info[] = {\n")
        for i, info in enumerate(sprite_info):
            f.write(f"    {{ {info['bpp']}, {info['gfx_idx']}, {info['pal_idx']} }}, // Tile {i}\n")
        f.write("};\n\n")

        # --- 2bpp Data ---
        if all_2bpp_data:
            f.write(f"// 2bpp sprite sheet graphics ({len(all_2bpp_data)} bytes, {len(all_2bpp_data)//16} 8x8 tiles)\n")
            f.write("const unsigned char sprite_gfx_2bpp[] = {\n    ")
            for i, byte in enumerate(all_2bpp_data):
                f.write(f"0x{byte:02x}, ")
                if (i + 1) % 16 == 0 and (i + 1) < len(all_2bpp_data):
                    f.write("\n    ")
            f.write("\n};\n\n")

            f.write(f"// 2bpp sprite sheet palettes ({len(all_2bpp_palettes)} palettes, 4 colors each)\n")
            f.write("const unsigned short sprite_palettes_2bpp[][4] = {\n")
            for i, palette in enumerate(all_2bpp_palettes):
                snes_palette = [convert_color_format(r, g, b) for r, g, b in palette]
                f.write(f"    {{ 0x{snes_palette[0]:04x}, 0x{snes_palette[1]:04x}, 0x{snes_palette[2]:04x}, 0x{snes_palette[3]:04x} }}, // Palette {i}\n")
            f.write("};\n\n")

        # --- 4bpp Data ---
        if all_4bpp_data:
            f.write(f"// 4bpp sprite sheet graphics ({len(all_4bpp_data)} bytes, {len(all_4bpp_data)//32} 8x8 tiles)\n")
            f.write("const unsigned char sprite_gfx_4bpp[] = {\n    ")
            for i, byte in enumerate(all_4bpp_data):
                f.write(f"0x{byte:02x}, ")
                if (i + 1) % 16 == 0 and (i + 1) < len(all_4bpp_data):  # Keep formatting at 16 bytes/line
                    f.write("\n    ")
            f.write("\n};\n\n")

            f.write(f"// 4bpp sprite sheet palettes ({len(all_4bpp_palettes)} palettes, 16 colors each)\n")
            f.write("const unsigned short sprite_palettes_4bpp[][16] = {\n")
            for i, palette in enumerate(all_4bpp_palettes):
                snes_palette = [convert_color_format(r, g, b) for r, g, b in palette]
                f.write(f"    {{ // Palette {i}\n        ")
                for j, color in enumerate(snes_palette):
                    f.write(f"0x{color:04x}, ")
                    if (j + 1) % 8 == 0 and (j + 1) < 16:
                        f.write("\n        ")
                f.write("\n    },\n")
            f.write("};\n\n")

        f.write(f"#endif // {header_guard}\n")

    print(f"Successfully converted '{image_filename}' to '{output_filename_h}'.")
    print(f"Total tiles processed: {tile_count} ({len(all_2bpp_palettes)} 2bpp, {len(all_4bpp_palettes)} 4bpp)")

if __name__ == '__main__':
    main()
