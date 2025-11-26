from PIL import Image, ImageDraw
import os
import json
import base64
import zlib
import gzip
import struct
from collections import Counter

# Global list to track level sizes for compression statistics
level_sizes = []  # List of (level_name, uncompressed_bytes, compressed_bytes)
level_data_combined = []  # List of (level_name, const_data, const_data_no_collision, uncompressed_bytes, uncompressed_no_collision_bytes)

# Shared GID mapping (built incrementally as levels are processed)
shared_gid_mapping_global = {
    'tile_to_gid': {},  # Maps (tl, tr, bl, br, pal_idx, flip_flags) -> GID
    'gid_map_data': [],  # List of tile entries indexed by GID
    'next_gid': 1  # Next available GID (0 is reserved for empty tiles)
}

"""
NES Sprite Sheet Converter

This script converts sprite sheets to NES-compatible CHR format (2bpp, 4 colors per tile).
Each original 8x8 tile is converted to a 16x16 sprite (4 8x8 tiles) in the CHR output.
The CHR data is split across two CHR banks.

NES Format:
- 2bpp (2 bits per pixel) = 4 colors per tile
- Each 8x8 tile = 16 bytes
- Each original 8x8 tile becomes 4 8x8 tiles (16x16 sprite = 64 bytes)
- CHR Bank 0: 2048 bytes (128 8x8 tiles)
- CHR Bank 1: 4096 bytes (256 8x8 tiles)
"""

# Collision and object definitions (from visualize_map.py)
far_background_gids = [40, 41, 42, 56, 57, 58, 16, 103, 104, 88]
deco_objects_gids = [44, 60,61,62,63]
solid_gids = [32, 33, 34, 35, 36, 37, 38, 39, 48, 49, 50, 51, 52, 53, 54, 55, 66, 67, 68, 69, 82, 83, 84, 85, 98, 99, 100, 101, 114, 115, 116, 117, 72]
pointy_gids = [17, 27, 43, 59]
icy_gids = [66,67,68,69,82,83,84,85,98,99,100,101,114,115,116,117]
all_background_gids = far_background_gids + solid_gids + pointy_gids + icy_gids + deco_objects_gids

arrMustBeObject = [8, 9, 10, 11, 12, 13, 14, 15, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 30, 31, 45, 46, 47, 64, 70, 71, 86, 87, 96, 97, 102, 118, 119, 120]

def convert_tile_to_2bpp(tile_image, palette):
    """Converts an 8x8 tile image to NES 2bpp format (16 bytes)."""
    tile_2bpp = bytearray(16)

    # Use RGB mode for palette processing
    tile_rgb = tile_image.convert('RGB')
    pixels = list(tile_rgb.getdata())

    # Create a mapping from palette color to palette index (0-3)
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
            # Fallback for colors not exactly in palette
            # Find the closest color by Euclidean distance
            min_dist = float('inf')
            best_idx = 0
            for i, pal_color in enumerate(palette):
                pr, pg, pb = pal_color
                dist = (r-pr)**2 + (g-pg)**2 + (b-pb)**2
                if dist < min_dist:
                    min_dist = dist
                    best_idx = i
            pixel_indices.append(best_idx)

    # Encode as 2bpp (NES format)
    # NES format: First 8 bytes are plane 0 (LSB), next 8 bytes are plane 1 (MSB)
    # Each byte represents one row, with bits packed left-to-right (bit 7 = leftmost pixel)
    for y in range(8):
        row_pixels = pixel_indices[y*8 : y*8+8]

        bp0 = 0  # Bit plane 0 (LSB)
        bp1 = 0  # Bit plane 1 (MSB)

        for i, pixel_index in enumerate(row_pixels):
            bp0 |= (pixel_index & 1) << (7 - i)
            bp1 |= ((pixel_index >> 1) & 1) << (7 - i)

        # NES format: plane 0 bytes at 0-7, plane 1 bytes at 8-15
        tile_2bpp[y] = bp0      # Plane 0 (LSB)
        tile_2bpp[y + 8] = bp1  # Plane 1 (MSB)

    return tile_2bpp


def extract_palette(tile_image, max_colors=4):
    """
    Extract a palette from a tile image, limiting to max_colors.
    Always ensures black (0,0,0) is color 0.
    """
    tile_rgb = tile_image.convert('RGB')
    colors = tile_rgb.getcolors(maxcolors=256)

    if not colors:
        return [(0, 0, 0)] * max_colors

    # Sort by frequency (descending) then by color value for determinism
    colors.sort(key=lambda item: (-item[0], item[1]))

    # Extract unique colors
    palette_colors = []
    black = (0, 0, 0)

    # Always start with black
    palette_colors.append(black)

    # Add other colors (excluding black)
    for count, color in colors:
        if color != black and color not in palette_colors:
            palette_colors.append(color)
            if len(palette_colors) >= max_colors:
                break

    # Pad with black if needed
    while len(palette_colors) < max_colors:
        palette_colors.append(black)

    return palette_colors[:max_colors]


def create_preview_image(sprite_data_list, tiles_per_row=8, scale=2, show_grid=True):
    """
    Create a preview image showing all 16x16 sprites with grid overlay.
    Each sprite is shown at 16x16 (scaled 2x from 8x8 original).
    Grid overlay shows the underlying 8x8 tile boundaries.
    
    Args:
        sprite_data_list: List of (sprite_4tiles, palette) tuples where sprite_4tiles
                         is a list of 4 tile data bytearrays [TL, TR, BL, BR]
    """
    if not sprite_data_list:
        # Return a small empty image if no tiles
        return Image.new('RGB', (16, 16), (128, 128, 128))

    num_sprites = len(sprite_data_list)
    num_rows = (num_sprites + tiles_per_row - 1) // tiles_per_row

    # Each sprite is 16x16, scaled by scale factor
    sprite_size_scaled = 16 * scale
    preview_width = tiles_per_row * sprite_size_scaled
    preview_height = num_rows * sprite_size_scaled

    preview = Image.new('RGB', (preview_width, preview_height), (128, 128, 128))

    # Draw sprites
    for sprite_idx, (sprite_4tiles, palette) in enumerate(sprite_data_list):
        row = sprite_idx // tiles_per_row
        col = sprite_idx % tiles_per_row

        # Reconstruct 16x16 sprite from 4 8x8 tiles
        # Decode each tile
        tl_pixels = decode_tile_from_2bpp(sprite_4tiles[0], palette)
        tr_pixels = decode_tile_from_2bpp(sprite_4tiles[1], palette)
        bl_pixels = decode_tile_from_2bpp(sprite_4tiles[2], palette)
        br_pixels = decode_tile_from_2bpp(sprite_4tiles[3], palette)

        # Create 16x16 image
        sprite_16x16 = Image.new('RGB', (16, 16))

        # Top row: TL and TR
        for y in range(8):
            for x in range(8):
                sprite_16x16.putpixel((x, y), tl_pixels[y*8 + x])
                sprite_16x16.putpixel((x+8, y), tr_pixels[y*8 + x])

        # Bottom row: BL and BR
        for y in range(8):
            for x in range(8):
                sprite_16x16.putpixel((x, y+8), bl_pixels[y*8 + x])
                sprite_16x16.putpixel((x+8, y+8), br_pixels[y*8 + x])

        # Scale up the sprite
        sprite_scaled = sprite_16x16.resize((sprite_size_scaled, sprite_size_scaled), Image.NEAREST)

        # Paste into preview
        x = col * sprite_size_scaled
        y = row * sprite_size_scaled
        preview.paste(sprite_scaled, (x, y))

    # Draw grid overlay showing 8x8 tile boundaries
    if show_grid:
        draw = ImageDraw.Draw(preview)

        # Grid color: semi-transparent white/light gray
        grid_color = (200, 200, 200)

        # Draw vertical lines (every 8*scale pixels for 8x8 tile boundaries)
        tile_8x8_size_scaled = 8 * scale
        for x in range(0, preview_width + 1, tile_8x8_size_scaled):
            draw.line([(x, 0), (x, preview_height)], fill=grid_color, width=1)

        # Draw horizontal lines (every 8*scale pixels for 8x8 tile boundaries)
        for y in range(0, preview_height + 1, tile_8x8_size_scaled):
            draw.line([(0, y), (preview_width, y)], fill=grid_color, width=1)

    return preview


def convert_8x8_to_16x16_sprite(tile_8x8, palette):
    """
    Convert an 8x8 tile to a 16x16 sprite (4 8x8 tiles).
    Returns a list of 4 tile data bytearrays: [top_left, top_right, bottom_left, bottom_right]
    """
    # Scale the 8x8 tile to 16x16 using nearest neighbor
    tile_16x16 = tile_8x8.resize((16, 16), Image.NEAREST)

    # Split into 4 8x8 tiles
    top_left = tile_16x16.crop((0, 0, 8, 8))
    top_right = tile_16x16.crop((8, 0, 16, 8))
    bottom_left = tile_16x16.crop((0, 8, 8, 16))
    bottom_right = tile_16x16.crop((8, 8, 16, 16))

    # Convert each 8x8 tile to 2bpp
    tiles_data = [
        convert_tile_to_2bpp(top_left, palette),
        convert_tile_to_2bpp(top_right, palette),
        convert_tile_to_2bpp(bottom_left, palette),
        convert_tile_to_2bpp(bottom_right, palette)
    ]

    return tiles_data


def decode_tile_from_2bpp(tile_data, palette):
    """Decode 2bpp tile data back to pixel colors."""
    # NES format: First 8 bytes are plane 0 (LSB), next 8 bytes are plane 1 (MSB)
    pixels = []

    for y in range(8):
        bp0 = tile_data[y]      # Plane 0 (LSB) - bytes 0-7
        bp1 = tile_data[y + 8]  # Plane 1 (MSB) - bytes 8-15

        for x in range(8):
            bit0 = (bp0 >> (7 - x)) & 1
            bit1 = (bp1 >> (7 - x)) & 1
            pixel_index = (bit1 << 1) | bit0

            # Clamp to palette size
            if pixel_index >= len(palette):
                pixel_index = 0

            pixels.append(palette[pixel_index])

    return pixels


def decode_tiled_layer_data(layer):
    """Decodes layer data if it is base64 encoded and compressed."""
    if layer.get("encoding") == "base64" and isinstance(layer.get("data"), str):
        decoded_data = base64.b64decode(layer["data"])
        compression = layer.get("compression")

        if compression == "zlib":
            decompressed_data = zlib.decompress(decoded_data)
        elif compression == "gzip":
            decompressed_data = gzip.decompress(decoded_data)
        else:
            decompressed_data = decoded_data

        num_tiles = len(decompressed_data) // 4
        unpacked_data = struct.unpack(f"<{num_tiles}I", decompressed_data)
        layer["data"] = list(unpacked_data)


# NES 64-color palette (RGB values)
# This is the standard NES palette with 64 predefined colors
NES_PALETTE = [
    (0x75, 0x75, 0x75), (0x27, 0x1B, 0x8F), (0x00, 0x00, 0xAB), (0x47, 0x00, 0x9F),
    (0x8F, 0x00, 0x77), (0xAB, 0x00, 0x13), (0xA7, 0x00, 0x00), (0x7F, 0x0B, 0x00),
    (0x43, 0x2F, 0x00), (0x00, 0x47, 0x00), (0x00, 0x51, 0x00), (0x00, 0x3F, 0x17),
    (0x1B, 0x3F, 0x5F), (0x00, 0x00, 0x00), (0x00, 0x00, 0x00), (0x00, 0x00, 0x00),
    (0xBC, 0xBC, 0xBC), (0x00, 0x73, 0xEF), (0x23, 0x3B, 0xEF), (0x83, 0x00, 0xF3),
    (0xBF, 0x00, 0xBF), (0xE7, 0x00, 0x5B), (0xDB, 0x2B, 0x00), (0xCB, 0x4F, 0x0F),
    (0x8B, 0x73, 0x00), (0x00, 0x97, 0x00), (0x00, 0xAB, 0x00), (0x00, 0x93, 0x3B),
    (0x00, 0x83, 0x8B), (0x00, 0x00, 0x00), (0x00, 0x00, 0x00), (0x00, 0x00, 0x00),
    (0xFF, 0xFF, 0xFF), (0x3F, 0xBF, 0xFF), (0x5F, 0x97, 0xFF), (0xA7, 0x8B, 0xFD),
    (0xF7, 0x7B, 0xFF), (0xFF, 0x77, 0xB7), (0xFF, 0x77, 0x63), (0xFF, 0x9F, 0x3B),
    (0xF3, 0xBF, 0x3B), (0x83, 0xD3, 0x13), (0x4F, 0xDF, 0x4B), (0x58, 0xF8, 0x98),
    (0x00, 0xEB, 0xDB), (0x00, 0x00, 0x00), (0x00, 0x00, 0x00), (0x00, 0x00, 0x00),
    (0xFF, 0xFF, 0xFF), (0xAB, 0xE7, 0xFF), (0xC7, 0xD7, 0xFF), (0xD7, 0xCB, 0xFF),
    (0xFF, 0xC7, 0xFF), (0xFF, 0xC7, 0xDB), (0xFF, 0xBF, 0xB3), (0xFF, 0xDB, 0xAB),
    (0xFF, 0xE7, 0xA3), (0xE3, 0xFF, 0xA3), (0xAB, 0xF3, 0xBF), (0xB3, 0xFF, 0xCF),
    (0x9F, 0xFF, 0xF3), (0x00, 0x00, 0x00), (0x00, 0x00, 0x00), (0x00, 0x00, 0x00),
]


def rgb_to_nes_6bit(r, g, b):
    """
    Convert RGB (0-255) to NES 6-bit color index (0-63).
    Finds the closest matching color from the NES 64-color palette.
    """
    # Find the closest NES color by Euclidean distance
    min_dist = float('inf')
    closest_idx = 0

    for i, (nr, ng, nb) in enumerate(NES_PALETTE):
        # Calculate Euclidean distance in RGB space
        dist = ((r - nr) ** 2 + (g - ng) ** 2 + (b - nb) ** 2) ** 0.5
        if dist < min_dist:
            min_dist = dist
            closest_idx = i

    return closest_idx


def compress_lzss(data):
    """
    LZSS (Lempel-Ziv-Storer-Szymanski) compression for estimation purposes.
    LZSS is commonly used in SNES/NES games and is more efficient than basic LZ77.
    
    Format:
    - Flag byte: 8 bits, each bit indicates if next token is literal (0) or match (1)
    - Literal: 1 byte (the literal value)
    - Match: 2 bytes (distance low, distance high) + 1 byte (length)
    
    Args:
        data: bytes or bytearray to compress
        
    Returns:
        compressed_data: bytearray of compressed data
    """
    if len(data) == 0:
        return bytearray()
    
    data = bytearray(data)
    output = bytearray()
    i = 0
    window_size = 4096  # 4KB sliding window (common for SNES/NES)
    min_match = 3
    max_match = 18
    
    while i < len(data):
        flag_byte = 0
        flag_byte_pos = len(output)
        output.append(0)  # Placeholder for flag byte
        
        # Process up to 8 tokens (one flag byte worth)
        for token in range(8):
            if i >= len(data):
                break
            
            best_match_len = 0
            best_match_dist = 0
            
            # Search in the sliding window
            search_start = max(0, i - window_size)
            
            for j in range(search_start, i):
                match_len = 0
                # Find how many bytes match
                while (match_len < max_match and 
                       i + match_len < len(data) and
                       j + match_len < i and
                       data[j + match_len] == data[i + match_len]):
                    match_len += 1
                
                if match_len > best_match_len:
                    best_match_len = match_len
                    best_match_dist = i - j
            
            # Decide: literal or match?
            if best_match_len >= min_match:
                # Match is better - encode as match
                flag_byte |= (1 << token)  # Set bit for match
                
                # Encode match: distance (2 bytes) + length (1 byte)
                dist_low = best_match_dist & 0xFF
                dist_high = (best_match_dist >> 8) & 0xFF
                length_byte = min(best_match_len - min_match, max_match - min_match)  # 0-15 for lengths 3-18
                
                output.append(dist_low)
                output.append(dist_high)
                output.append(length_byte)
                i += best_match_len
            else:
                # Literal is better - encode as literal
                output.append(data[i])
                i += 1
        
        # Update flag byte
        output[flag_byte_pos] = flag_byte
    
    return output


def estimate_lzss_size(data):
    """
    Estimate LZSS compressed size using the LZSS compressor.
    
    Args:
        data: bytes or bytearray to estimate
        
    Returns:
        estimated_size: estimated compressed size in bytes
    """
    if len(data) == 0:
        return 0
    
    compressed = compress_lzss(data)
    return len(compressed)


def compress_lzsa2(data):
    """
    LZSA2 (Lempel-Ziv-Storer-Szymanski variant 2) compression for estimation purposes.
    LZSA2 uses a different encoding scheme than LZSS, often with better compression ratios.
    
    Format:
    - Flag byte: 8 bits, each bit indicates if next token is literal (0) or match (1)
    - Literal: 1 byte (the literal value)
    - Match: Variable encoding - typically 2 bytes for distance + 1 byte for length
    - Uses different distance/length encoding than LZSS
    
    Args:
        data: bytes or bytearray to compress
        
    Returns:
        compressed_data: bytearray of compressed data
    """
    if len(data) == 0:
        return bytearray()
    
    data = bytearray(data)
    output = bytearray()
    i = 0
    window_size = 4096  # 4KB sliding window (common for SNES/NES)
    min_match = 2  # LZSA2 often uses shorter minimum matches
    max_match = 257  # LZSA2 supports longer matches
    
    while i < len(data):
        flag_byte = 0
        flag_byte_pos = len(output)
        output.append(0)  # Placeholder for flag byte
        
        # Process up to 8 tokens (one flag byte worth)
        for token in range(8):
            if i >= len(data):
                break
            
            best_match_len = 0
            best_match_dist = 0
            
            # Search in the sliding window
            search_start = max(0, i - window_size)
            
            for j in range(search_start, i):
                match_len = 0
                # Find how many bytes match
                while (match_len < max_match and 
                       i + match_len < len(data) and
                       j + match_len < i and
                       data[j + match_len] == data[i + match_len]):
                    match_len += 1
                
                if match_len > best_match_len:
                    best_match_len = match_len
                    best_match_dist = i - j
            
            # Decide: literal or match?
            # LZSA2: match is beneficial if length >= min_match
            if best_match_len >= min_match:
                # Match is better - encode as match
                flag_byte |= (1 << token)  # Set bit for match
                
                # LZSA2 encoding: distance (2 bytes) + length (variable, but we'll use 1 byte for estimation)
                # Distance encoding: low byte, high byte
                dist_low = best_match_dist & 0xFF
                dist_high = (best_match_dist >> 8) & 0xFF
                
                # Length encoding: for LZSA2, we encode length-2 (since min_match=2)
                # Cap at 255 for single byte encoding
                length_byte = min(best_match_len - min_match, 255)
                
                output.append(dist_low)
                output.append(dist_high)
                output.append(length_byte)
                i += best_match_len
            else:
                # Literal is better - encode as literal
                output.append(data[i])
                i += 1
        
        # Update flag byte
        output[flag_byte_pos] = flag_byte
    
    return output


def estimate_lzsa2_size(data):
    """
    Estimate LZSA2 compressed size using the LZSA2 compressor.
    
    Args:
        data: bytes or bytearray to estimate
        
    Returns:
        estimated_size: estimated compressed size in bytes
    """
    if len(data) == 0:
        return 0
    
    compressed = compress_lzsa2(data)
    return len(compressed)


def generate_nes_tilemap_header(tile_data, layer_name, map_width, map_height, all_sprite_data, tile_mapping, unique_chr_tiles, all_palettes, object_palettes=None, object_palette_mapping=None, shared_gid_mapping=None):
    """
    Generate a .h file with tilemap, palette, collision, object, and spawn data for NES.
    
    Args:
        tile_data: List of tile GIDs from the JSON map
        layer_name: Name of the layer
        map_width: Width of map in tiles
        map_height: Height of map in tiles
        all_sprite_data: List of (sprite_4tiles, palette) tuples
        tile_mapping: Dictionary mapping original_tile_index -> optimized_tile_index
        unique_chr_tiles: List of unique CHR tile data
        all_palettes: List of all palettes
        object_palettes: List of optimized object palettes (3 palettes, 1 reserved for player)
        object_palette_mapping: Dictionary mapping tile_index -> object_palette_index (0-2)
    """
    # Sanitize layer name for C identifiers
    safe_layer_name = ''.join(c if c.isalnum() else '_' for c in layer_name)
    if not safe_layer_name or safe_layer_name[0].isdigit():
        safe_layer_name = 'level_' + safe_layer_name

    # Get the directory where this script is located (for output files)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_filename = os.path.join(script_dir, f'tilemap_{safe_layer_name}_nes.h')

    # Process tile data to extract tilemap, palette, collision, objects, and spawn
    tilemap_data = []  # List of (tl_tile, tr_tile, bl_tile, br_tile, palette_idx, flip_flags)
    collision_data = []
    object_data = []
    spawn_x = 0
    spawn_y = 0
    spawn_found = False

    # Collect unique palettes separately for background and sprite tiles
    # Background palettes: max 4, first color must be black
    background_palettes = []
    background_palette_to_index = {}  # Maps palette tuple to index in background_palettes

    # Sprite palettes: max 4
    # Initialize with object_palettes (3 palettes for indices 0-2, index 3 reserved for player)
    sprite_palettes = []
    sprite_palette_to_index = {}  # Maps palette tuple to index in sprite_palettes
    
    # Initialize sprite palettes with object_palettes if provided (for indices 0-2)
    if object_palettes is not None and len(object_palettes) > 0:
        for i, obj_pal in enumerate(object_palettes[:3]):  # Use first 3 palettes (indices 0-2)
            # Normalize palette: pad to 4 colors if needed
            normalized_obj_pal = list(obj_pal)
            while len(normalized_obj_pal) < 4:
                normalized_obj_pal.append((0, 0, 0))
            normalized_obj_pal = normalized_obj_pal[:4]
            sprite_palettes.append(normalized_obj_pal)
            sprite_palette_to_index[tuple(normalized_obj_pal)] = i
            print(f"Initialized sprite palette {i} with object palette: {normalized_obj_pal}")
    
    # Ensure sprite_palettes has at least 4 entries (pad with black if needed)
    while len(sprite_palettes) < 4:
        sprite_palettes.append([(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)])
    
    # Pre-populate sprite palettes with player palette at index 3
    # Player sprite frames map to sprite sheet tile indices: 1, 2, 3, 4, 5, 6, 7
    # (corresponding to PLAYER_SPRITE_IDLE=0, WALK_1=2, WALK_2=4, WALK_3=6, WALL=8, DOWN=10, UP=12)
    player_sprite_tile_indices = [1, 2, 3, 4, 5, 6, 7]
    player_palette = None
    for player_tile_idx in player_sprite_tile_indices:
        if player_tile_idx < len(all_sprite_data):
            _, player_palette = all_sprite_data[player_tile_idx]
            if player_palette and len(player_palette) > 0:
                # Check if palette is not all black
                is_all_black = all(color == (0, 0, 0) for color in player_palette)
                if not is_all_black:
                    # Found a valid non-black player palette
                    print(f"Found player palette from tile index {player_tile_idx}: {player_palette}")
                    break
                else:
                    print(f"Warning: Player tile index {player_tile_idx} has all-black palette, trying next...")
    
    if player_palette is not None:
        # Normalize player palette
        normalized_player_palette = list(player_palette)
        while len(normalized_player_palette) < 4:
            normalized_player_palette.append((0, 0, 0))
        normalized_player_palette = normalized_player_palette[:4]
        
        # Ensure sprite_palettes has at least 4 entries (pad with black if needed)
        while len(sprite_palettes) < 4:
            sprite_palettes.append([(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)])
        
        # Set player palette at index 3 (reserved for player)
        sprite_palettes[3] = normalized_player_palette
        sprite_palette_to_index[tuple(normalized_player_palette)] = 3
        print(f"Pre-populated sprite palette 3 with player palette: {normalized_player_palette}")
    else:
        print("WARNING: Could not find valid player palette! All player frames may be black.")

    for i, tile_gid in enumerate(tile_data):
        # Extract flip flags before clearing
        FLIPPED_HORIZONTALLY_FLAG = 0x80000000
        FLIPPED_VERTICALLY_FLAG = 0x40000000
        FLIPPED_DIAGONALLY_FLAG = 0x20000000

        flipped_horizontally = bool(tile_gid & FLIPPED_HORIZONTALLY_FLAG)
        flipped_vertically = bool(tile_gid & FLIPPED_VERTICALLY_FLAG)
        flipped_diagonally = bool(tile_gid & FLIPPED_DIAGONALLY_FLAG)

        # Clear flags to get actual tile index
        tile_gid_clean = tile_gid & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG)
        tile_index = (tile_gid_clean - 1) & 0x3FFF  # 14-bit tile index

        # Calculate tile position
        map_x_tile = i % map_width
        map_y_tile = i // map_width

        if tile_gid_clean == 0 or tile_index < 0 or tile_index >= len(all_sprite_data):
            # Empty or invalid tile
            tilemap_data.append((0, 0, 0, 0, 0, 0))  # Empty tile entry
            collision_data.append(0)
            continue

        # Special handling for player (tile index 1) - track spawn but skip in tilemap
        if tile_index == 1:
            # Track spawn location for .h file
            if not spawn_found:
                spawn_x = map_x_tile
                spawn_y = map_y_tile
                spawn_found = True
            # Skip in tilemap (empty black tile)
            tilemap_data.append((0, 0, 0, 0, 0, 0))  # Empty tile entry
            collision_data.append(0)
            continue

        # Skip tiles in arrMustBeObject - treat as empty black tiles in tilemap
        if tile_index in arrMustBeObject:
            # Add to object data but skip in tilemap (empty black tile)
            object_data.append((tile_index, map_x_tile, map_y_tile))
            tilemap_data.append((0, 0, 0, 0, 0, 0))  # Empty tile entry
            collision_data.append(0)
            continue

        # Get sprite data
        sprite_4tiles, palette = all_sprite_data[tile_index]

        # Map original tile indices to optimized indices
        tl_opt_idx = tile_mapping.get(tile_index * 4 + 0, 0)
        tr_opt_idx = tile_mapping.get(tile_index * 4 + 1, 0)
        bl_opt_idx = tile_mapping.get(tile_index * 4 + 2, 0)
        br_opt_idx = tile_mapping.get(tile_index * 4 + 3, 0)

        # Clamp to valid range
        tl_opt_idx = min(tl_opt_idx, 255)
        tr_opt_idx = min(tr_opt_idx, 255)
        bl_opt_idx = min(bl_opt_idx, 255)
        br_opt_idx = min(br_opt_idx, 255)

        # Determine if this is a background tile (includes far_background, solid, pointy, and icy)
        is_background = tile_index in all_background_gids

        # Normalize palette: pad to 4 colors
        # Note: extract_palette already ensures black is color 0 for all palettes
        if is_background:
            # Background tiles: palette should already have black as color 0 (from extract_palette)
            normalized_palette = list(palette)
            # Pad to 4 colors if needed
            while len(normalized_palette) < 4:
                normalized_palette.append((0, 0, 0))
            normalized_palette = normalized_palette[:4]
            # Verify black is color 0 (should always be true from extract_palette)
            if len(normalized_palette) > 0 and normalized_palette[0] != (0, 0, 0):
                # Force black as color 0 (this shouldn't happen, but safety check)
                normalized_palette = [(0, 0, 0)] + [c for c in normalized_palette if c != (0, 0, 0)]
                normalized_palette = normalized_palette[:4]

            # Get or add palette to background palettes (max 4)
            palette_key = tuple(normalized_palette)
            if palette_key not in background_palette_to_index:
                if len(background_palettes) >= 4:
                    # Use the first palette if we've exceeded the limit
                    print(f"WARNING: Exceeded 4 background palettes limit. Reusing palette 0 for tile {tile_index}")
                    palette_idx = 0
                else:
                    background_palette_to_index[palette_key] = len(background_palettes)
                    background_palettes.append(normalized_palette)
                    palette_idx = len(background_palettes) - 1
            else:
                palette_idx = background_palette_to_index[palette_key]

            # Encode: bit 2 = 0 (background), bits 0-1 = palette index (0-3)
            palette_idx_encoded = palette_idx & 0x03  # 0-3

            # Safety check: ensure we never exceed 3
            if palette_idx_encoded > 3:
                print(f"ERROR: Background palette index {palette_idx_encoded} exceeds limit of 3!")
                palette_idx_encoded = 0
        else:
            # Sprite tile - use sprite palettes
            normalized_palette = list(palette)
            # Pad to 4 colors if needed
            while len(normalized_palette) < 4:
                normalized_palette.append((0, 0, 0))
            normalized_palette = normalized_palette[:4]

            # Get or add palette to sprite palettes (max 4)
            # Note: Palette index 3 is reserved for player, so we skip it when adding new palettes
            palette_key = tuple(normalized_palette)
            if palette_key not in sprite_palette_to_index:
                # Check if this is the player palette (should already be at index 3)
                if palette_key in sprite_palette_to_index:
                    palette_idx = sprite_palette_to_index[palette_key]
                else:
                    # Find an available slot (0, 1, or 2 - skip 3 which is reserved for player)
                    available_slots = [0, 1, 2]
                    palette_idx = None
                    for slot in available_slots:
                        if slot < len(sprite_palettes):
                            # Check if this slot is empty (all black) or can be reused
                            if sprite_palettes[slot] == [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)]:
                                sprite_palettes[slot] = normalized_palette
                                sprite_palette_to_index[palette_key] = slot
                                palette_idx = slot
                                break
                    
                    if palette_idx is None:
                        # All slots 0-2 are taken, use slot 0
                        palette_idx = 0
                        sprite_palettes[0] = normalized_palette
                        sprite_palette_to_index[palette_key] = 0
            else:
                palette_idx = sprite_palette_to_index[palette_key]

            # Encode: bit 2 = 1 (sprite), bits 0-1 = palette index (0-3)
            palette_idx_encoded = (palette_idx & 0x03) | 0x04  # 4-7

        # Encode flip flags (3 bits: H, V, D)
        flip_flags = 0
        if flipped_horizontally:
            flip_flags |= 0x01
        if flipped_vertically:
            flip_flags |= 0x02
        if flipped_diagonally:
            flip_flags |= 0x04

        # Store tilemap entry: (TL, TR, BL, BR, palette_idx, flip_flags)
        tilemap_data.append((tl_opt_idx, tr_opt_idx, bl_opt_idx, br_opt_idx, palette_idx_encoded, flip_flags))

        # Generate collision data
        if tile_index in solid_gids or tile_index in icy_gids:
            collision_data.append(1)
        elif tile_index in pointy_gids:
            if tile_index == 17:
                collision_data.append(4)
            elif tile_index == 27:
                collision_data.append(8)
            elif tile_index == 43:
                collision_data.append(16)  # 0x10
            elif tile_index == 59:
                collision_data.append(32)  # 0x20
            else:
                collision_data.append(0)
        else:
            collision_data.append(0)

    # Ensure player palette is still at sprite palette index 3 (in case it got overwritten)
    # This is a safety check - player palette should already be set above
    # Player sprite frames map to sprite sheet tile indices: 1, 2, 3, 4, 5, 6, 7
    player_sprite_tile_indices = [1, 2, 3, 4, 5, 6, 7]
    player_palette = None
    for player_tile_idx in player_sprite_tile_indices:
        if player_tile_idx < len(all_sprite_data):
            _, player_palette = all_sprite_data[player_tile_idx]
            if player_palette and len(player_palette) > 0:
                break
    
    if player_palette is not None:
        # Normalize player palette
        normalized_player_palette = list(player_palette)
        while len(normalized_player_palette) < 4:
            normalized_player_palette.append((0, 0, 0))
        normalized_player_palette = normalized_player_palette[:4]
        
        # Ensure sprite_palettes has at least 4 entries
        while len(sprite_palettes) < 4:
            sprite_palettes.append([(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)])
        
        # Force player palette at index 3 (reserved for player)
        sprite_palettes[3] = normalized_player_palette
        sprite_palette_to_index[tuple(normalized_player_palette)] = 3

    # Initialize object sprite data count for byte calculation
    object_sprite_data_count = 0
    
    # Initialize shared GID mapping on first use (reserve GID 0 for empty tiles)
    if len(shared_gid_mapping_global['gid_map_data']) == 0:
        empty_tile = (0, 0, 0, 0, 0, 0)
        shared_gid_mapping_global['tile_to_gid'][empty_tile] = 0
        shared_gid_mapping_global['gid_map_data'].append(empty_tile)
        print("Initialized shared GID mapping (GID 0 reserved for empty tiles)")
    
    # Use shared GID mapping (build incrementally)
    tile_to_gid = shared_gid_mapping_global['tile_to_gid']
    gid_map_data = shared_gid_mapping_global['gid_map_data']
    next_gid = shared_gid_mapping_global['next_gid']
    
    # Generate GID array from tilemap_data using shared mapping
    gid_array = []
    for tile_entry in tilemap_data:
        if tile_entry in tile_to_gid:
            # Use existing GID from shared mapping
            gid = tile_to_gid[tile_entry]
        else:
            # Add new entry to shared mapping
            if next_gid >= 256:
                # Out of GIDs, reuse GID 0 (empty tile) as fallback
                print(f"WARNING: Exceeded 256 GIDs limit. Reusing empty tile GID for layer '{layer_name}'.")
                gid = 0
            else:
                gid = next_gid
                tile_to_gid[tile_entry] = gid
                gid_map_data.append(tile_entry)
                next_gid += 1
                shared_gid_mapping_global['next_gid'] = next_gid
        gid_array.append(gid)
    
    print(f"Using shared GID mapping ({len(gid_map_data)} unique GIDs total) for layer '{layer_name}'")
    
    # Write .h file
    with open(output_filename, 'w') as f:
        # Header guard
        header_guard = f"TILEMAP_{safe_layer_name.upper()}_NES_H"
        f.write(f"// NES tilemap data for layer '{layer_name}'\n")
        f.write(f"// Generated from baseCelesteTileMap.json\n\n")
        f.write(f"#ifndef {header_guard}\n")
        f.write(f"#define {header_guard}\n\n")

        # Map dimensions
        f.write(f"// Tilemap dimensions: {map_width}x{map_height} tiles (NES 8x8)\n")
        f.write(f"#define TILEMAP_{safe_layer_name.upper()}_WIDTH {map_width}\n")
        f.write(f"#define TILEMAP_{safe_layer_name.upper()}_HEIGHT {map_height}\n\n")

        # Using shared GID mapping - include reference to shared header
        f.write(f"// Using shared GID mapping (see gid_to_tile_shared.h)\n")
        f.write(f"#include \"gid_to_tile_shared.h\"\n\n")

        # Tilemap data - array of GIDs (one byte per entry)
        f.write(f"// Tilemap data for layer '{layer_name}' (GIDs, one byte per entry)\n")
        f.write(f"const unsigned char tilemap_{safe_layer_name}[] = {{\n")
        for y in range(map_height):
            f.write(f"    // Row {y}\n")
            for x in range(map_width):
                idx = y * map_width + x
                if idx < len(gid_array):
                    gid = gid_array[idx]
                    f.write(f"    {gid}")
                    if idx < len(gid_array) - 1:
                        f.write(",")
                    f.write("\n")
        f.write("};\n\n")
        f.write(f"#define TILEMAP_{safe_layer_name.upper()}_COUNT {len(gid_array)}\n\n")

        # Background palette data - NES 6-bit format (4 palettes, 4 colors per palette)
        # Background palettes must have black as color 0
        f.write(f"// Background palette data for layer '{layer_name}' (NES 6-bit format)\n")
        f.write(f"// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]\n")
        f.write(f"// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)\n")
        f.write(f"const unsigned char palette_background_{safe_layer_name}[4][4] = {{\n")
        for pal_idx in range(4):
            if pal_idx < len(background_palettes):
                palette = background_palettes[pal_idx]
            else:
                # Default: all black
                palette = [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)]
            f.write(f"    // Background Palette {pal_idx}\n")
            f.write("    { ")
            for color_idx, (r, g, b) in enumerate(palette):
                nes_color = rgb_to_nes_6bit(r, g, b)
                f.write(f"0x{nes_color:02x}")
                if color_idx < len(palette) - 1:
                    f.write(", ")
            f.write(" }")
            if pal_idx < 3:
                f.write(",")
            f.write("\n")
        f.write("};\n\n")
        f.write(f"#define PALETTE_BACKGROUND_{safe_layer_name.upper()}_COUNT {min(len(background_palettes), 4)}\n\n")

        # Final safety check: ensure player palette is at index 3
        # Player sprite frames map to sprite sheet tile indices: 1, 2, 3, 4, 5, 6, 7
        player_sprite_tile_indices = [1, 2, 3, 4, 5, 6, 7]
        player_palette_final = None
        for player_tile_idx in player_sprite_tile_indices:
            if player_tile_idx < len(all_sprite_data):
                _, player_palette_final = all_sprite_data[player_tile_idx]
                if player_palette_final and len(player_palette_final) > 0:
                    is_all_black = all(color == (0, 0, 0) for color in player_palette_final)
                    if not is_all_black:
                        break
        
        if player_palette_final is not None:
            normalized_player_palette_final = list(player_palette_final)
            while len(normalized_player_palette_final) < 4:
                normalized_player_palette_final.append((0, 0, 0))
            normalized_player_palette_final = normalized_player_palette_final[:4]
            
            # Ensure sprite_palettes has at least 4 entries
            while len(sprite_palettes) < 4:
                sprite_palettes.append([(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)])
            
            # Force player palette at index 3
            sprite_palettes[3] = normalized_player_palette_final
            print(f"Final check: Set sprite palette 3 to: {normalized_player_palette_final}")

        # Sprite palette data - NES 6-bit format (4 palettes, 4 colors per palette)
        f.write(f"// Sprite palette data for layer '{layer_name}' (NES 6-bit format)\n")
        f.write(f"// 4 sprite palettes, each with 4 colors: [color0, color1, color2, color3]\n")
        f.write(f"// Used by non-background tiles\n")
        f.write(f"// Palette 3 is reserved for player sprites\n")
        f.write(f"const unsigned char palette_sprite_{safe_layer_name}[4][4] = {{\n")
        for pal_idx in range(4):
            if pal_idx < len(sprite_palettes):
                palette = sprite_palettes[pal_idx]
            else:
                # Default: all black
                palette = [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)]
            
            # Debug: print palette values before writing
            if pal_idx == 3:
                print(f"Writing sprite palette {pal_idx} (player): {palette}")
            
            f.write(f"    // Sprite Palette {pal_idx}")
            if pal_idx == 3:
                f.write(" (Player)")
            f.write("\n")
            f.write("    { ")
            for color_idx, (r, g, b) in enumerate(palette):
                nes_color = rgb_to_nes_6bit(r, g, b)
                f.write(f"0x{nes_color:02x}")
                if color_idx < len(palette) - 1:
                    f.write(", ")
            f.write(" }")
            if pal_idx < 3:
                f.write(",")
            f.write("\n")
        f.write("};\n\n")
        f.write(f"#define PALETTE_SPRITE_{safe_layer_name.upper()}_COUNT {min(len(sprite_palettes), 4)}\n\n")

        # Collision data
        f.write(f"// Collision data for layer '{layer_name}'\n")
        f.write(f"const unsigned char collision_{safe_layer_name}[] = {{\n")
        # Write in rows for readability
        for y in range(map_height):
            row_start = y * map_width
            row_end = row_start + map_width
            row_data = collision_data[row_start:row_end]
            f.write(f"    // Row {y}\n")
            f.write("    ")
            f.write(", ".join(str(val) for val in row_data))
            if y < map_height - 1:
                f.write(",")
            f.write("\n")
        f.write("};\n\n")
        f.write(f"#define COLLISION_{safe_layer_name.upper()}_COUNT {len(collision_data)}\n\n")

        # Object data
        f.write(f"// Object data for layer '{layer_name}'\n")
        if object_data:
            f.write(f"const unsigned char object_{safe_layer_name}[] = {{\n")
            for obj_tile, obj_x, obj_y in object_data:
                f.write(f"    {obj_tile}, {obj_x}, {obj_y},\n")
            f.write("};\n\n")
            f.write(f"#define OBJECT_{safe_layer_name.upper()}_COUNT {len(object_data)}\n\n")
        else:
            f.write(f"const unsigned char object_{safe_layer_name}[] = {{}};\n\n")
            f.write(f"#define OBJECT_{safe_layer_name.upper()}_COUNT 0\n\n")

        # Object sprite data and palettes
        if object_palettes and object_palette_mapping:
            # Collect unique object tiles from object_data
            unique_object_tiles = set()
            for obj_tile, obj_x, obj_y in object_data:
                if obj_tile < len(all_sprite_data):
                    unique_object_tiles.add(obj_tile)
            
            # Add all player sprite frames to object sprite data
            # Player sprite states: IDLE=0, WALK_1=2, WALK_2=4, WALK_3=6, WALL=8, DOWN=10, UP=12
            # Maps to sprite sheet tile indices: 1, 2, 3, 4, 5, 6, 7
            # Player uses palette 3 (reserved for player, which is sprite palette index 3)
            player_sprite_tile_indices = [1, 2, 3, 4, 5, 6, 7]  # Sprite sheet tile indices for player frames
            for player_tile_idx in player_sprite_tile_indices:
                if player_tile_idx < len(all_sprite_data):
                    unique_object_tiles.add(player_tile_idx)
            
            # Generate object sprite data (CHR data for each unique object)
            # Each object is a 16x16 sprite = 4 8x8 tiles = 64 bytes
            f.write(f"// Object sprite data for layer '{layer_name}'\n")
            f.write(f"// Each object sprite is 16x16 (4 8x8 tiles = 64 bytes)\n")
            f.write(f"// Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile, ...]\n")
            
            # Create mapping from object tile index to sprite data index
            object_sprite_mapping = {}
            object_sprite_data = []
            
            for obj_tile_idx in sorted(unique_object_tiles):
                if obj_tile_idx < len(all_sprite_data):
                    sprite_4tiles, _ = all_sprite_data[obj_tile_idx]
                    # Get optimized tile indices
                    tl_opt_idx = tile_mapping.get(obj_tile_idx * 4 + 0, 0)
                    tr_opt_idx = tile_mapping.get(obj_tile_idx * 4 + 1, 0)
                    bl_opt_idx = tile_mapping.get(obj_tile_idx * 4 + 2, 0)
                    br_opt_idx = tile_mapping.get(obj_tile_idx * 4 + 3, 0)
                    
                    # Get palette index for this object
                    # Player frames (tile indices 1, 2, 3, 4, 5, 6, 7) use sprite palette 3 (reserved for player)
                    # Other objects use object palettes (0-2)
                    player_sprite_tile_indices = [1, 2, 3, 4, 5, 6, 7]
                    if obj_tile_idx in player_sprite_tile_indices:
                        # Player frames use sprite palette 3 (mark with 3)
                        palette_idx = 3
                    else:
                        # Get palette index for this object (0-2, since we have 3 object palettes)
                        palette_idx = object_palette_mapping.get(obj_tile_idx, 0)
                        if palette_idx >= len(object_palettes):
                            palette_idx = 0
                    
                    object_sprite_mapping[obj_tile_idx] = len(object_sprite_data)
                    object_sprite_data.append((obj_tile_idx, palette_idx, tl_opt_idx, tr_opt_idx, bl_opt_idx, br_opt_idx))
            
            object_sprite_data_count = len(object_sprite_data)
            
            # Write object sprite data array
            f.write(f"const unsigned char object_sprite_{safe_layer_name}[] = {{\n")
            f.write(f"    // Format: [tile_index, palette_index, tl_tile, tr_tile, bl_tile, br_tile]\n")
            for obj_tile_idx, palette_idx, tl, tr, bl, br in object_sprite_data:
                f.write(f"    {obj_tile_idx}, {palette_idx}, {tl}, {tr}, {bl}, {br},\n")
            f.write("};\n\n")
            f.write(f"#define OBJECT_SPRITE_{safe_layer_name.upper()}_COUNT {len(object_sprite_data)}\n\n")
            
            # Write object palettes (3 palettes for objects, 1 reserved for player)
            f.write(f"// Object palette data for layer '{layer_name}' (NES 6-bit format)\n")
            f.write(f"// 3 object palettes, each with 4 colors: [color0, color1, color2, color3]\n")
            f.write(f"// Note: Palette 3 is reserved for the player\n")
            f.write(f"const unsigned char palette_object_{safe_layer_name}[3][4] = {{\n")
            for pal_idx in range(3):
                if pal_idx < len(object_palettes):
                    palette = object_palettes[pal_idx]
                else:
                    # Default: all black
                    palette = [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)]
                f.write(f"    // Object Palette {pal_idx}\n")
                f.write("    { ")
                for color_idx, (r, g, b) in enumerate(palette):
                    nes_color = rgb_to_nes_6bit(r, g, b)
                    f.write(f"0x{nes_color:02x}")
                    if color_idx < len(palette) - 1:
                        f.write(", ")
                f.write(" }")
                if pal_idx < 2:
                    f.write(",")
                f.write("\n")
            f.write("};\n\n")
            f.write(f"#define PALETTE_OBJECT_{safe_layer_name.upper()}_COUNT {min(len(object_palettes), 3)}\n\n")
        else:
            # No object palettes provided, write empty arrays
            f.write(f"// Object sprite data for layer '{layer_name}'\n")
            f.write(f"const unsigned char object_sprite_{safe_layer_name}[] = {{}};\n\n")
            f.write(f"#define OBJECT_SPRITE_{safe_layer_name.upper()}_COUNT 0\n\n")
            f.write(f"// Object palette data for layer '{layer_name}'\n")
            f.write(f"const unsigned char palette_object_{safe_layer_name}[3][4] = {{\n")
            f.write(f"    {{ 0x00, 0x00, 0x00, 0x00 }},\n")
            f.write(f"    {{ 0x00, 0x00, 0x00, 0x00 }},\n")
            f.write(f"    {{ 0x00, 0x00, 0x00, 0x00 }}\n")
            f.write("};\n\n")
            f.write(f"#define PALETTE_OBJECT_{safe_layer_name.upper()}_COUNT 0\n\n")

        # Spawn data
        f.write(f"// Player start location for layer '{layer_name}'\n")
        if spawn_found:
            f.write(f"#define SPAWN_X_{safe_layer_name.upper()} {spawn_x}\n")
            f.write(f"#define SPAWN_Y_{safe_layer_name.upper()} {spawn_y}\n\n")
        else:
            f.write(f"#define SPAWN_X_{safe_layer_name.upper()} 0\n")
            f.write(f"#define SPAWN_Y_{safe_layer_name.upper()} 0\n\n")

        f.write(f"#endif // {header_guard}\n")

    # Calculate total bytes for all const data
    # Tilemap is now GIDs (1 byte per entry)
    tilemap_gid_bytes = len(gid_array) * 1  # Each entry: 1 byte (GID)
    # GID mapping: 6 bytes per unique GID
    gid_mapping_bytes = len(gid_map_data) * 6  # Each entry: TL, TR, BL, BR, palette_idx, flip_flags
    background_palette_bytes = 4 * 4  # 4 palettes * 4 colors
    sprite_palette_bytes = 4 * 4  # 4 palettes * 4 colors
    collision_bytes = len(collision_data) * 1  # Each entry: 1 byte
    object_bytes = len(object_data) * 3  # Each entry: tile_index, x, y (3 bytes)
    object_sprite_bytes = object_sprite_data_count * 6  # Each entry: tile_index, palette_index, tl, tr, bl, br (6 bytes)
    object_palette_bytes = 3 * 4  # 3 palettes * 4 colors
    
    total_bytes = (tilemap_gid_bytes + gid_mapping_bytes + background_palette_bytes + sprite_palette_bytes + 
                   collision_bytes + object_bytes + object_sprite_bytes + object_palette_bytes)
    
    # Calculate total without collision data (can be derived from tilemap)
    total_bytes_no_collision = (tilemap_gid_bytes + gid_mapping_bytes + background_palette_bytes + sprite_palette_bytes + 
                                object_bytes + object_sprite_bytes + object_palette_bytes)

    # Collect all const data into a bytearray for compression estimation
    const_data = bytearray()
    
    # Add tilemap GIDs (1 byte per entry)
    const_data.extend(gid_array)
    
    # Add GID mapping data (6 bytes per unique GID)
    for gid_entry in gid_map_data:
        tl, tr, bl, br, pal_idx, flip = gid_entry
        const_data.extend([tl, tr, bl, br, pal_idx, flip])
    
    # Add background palettes
    for pal_idx in range(4):
        if pal_idx < len(background_palettes):
            palette = background_palettes[pal_idx]
        else:
            palette = [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)]
        for r, g, b in palette:
            const_data.append(rgb_to_nes_6bit(r, g, b))
    
    # Add sprite palettes
    for pal_idx in range(4):
        if pal_idx < len(sprite_palettes):
            palette = sprite_palettes[pal_idx]
        else:
            palette = [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)]
        for r, g, b in palette:
            const_data.append(rgb_to_nes_6bit(r, g, b))
    
    # Add collision data
    const_data.extend(collision_data)
    
    # Create version without collision data for comparison
    const_data_no_collision = bytearray()
    # Add tilemap GIDs (1 byte per entry)
    const_data_no_collision.extend(gid_array)
    
    # Add GID mapping data (6 bytes per unique GID)
    for gid_entry in gid_map_data:
        tl, tr, bl, br, pal_idx, flip = gid_entry
        const_data_no_collision.extend([tl, tr, bl, br, pal_idx, flip])
    # Add background palettes
    for pal_idx in range(4):
        if pal_idx < len(background_palettes):
            palette = background_palettes[pal_idx]
        else:
            palette = [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)]
        for r, g, b in palette:
            const_data_no_collision.append(rgb_to_nes_6bit(r, g, b))
    # Add sprite palettes
    for pal_idx in range(4):
        if pal_idx < len(sprite_palettes):
            palette = sprite_palettes[pal_idx]
        else:
            palette = [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)]
        for r, g, b in palette:
            const_data_no_collision.append(rgb_to_nes_6bit(r, g, b))
    # Skip collision data
    # Add object data
    for obj_tile, obj_x, obj_y in object_data:
        const_data.extend([obj_tile, obj_x, obj_y])
        const_data_no_collision.extend([obj_tile, obj_x, obj_y])
    
    # Add object sprite data (if available)
    if object_palettes and object_palette_mapping:
        unique_object_tiles = set()
        for obj_tile, obj_x, obj_y in object_data:
            if obj_tile < len(all_sprite_data):
                unique_object_tiles.add(obj_tile)
        player_sprite_tile_indices = [1, 2, 3, 4, 5, 6, 7]
        for player_tile_idx in player_sprite_tile_indices:
            if player_tile_idx < len(all_sprite_data):
                unique_object_tiles.add(player_tile_idx)
        
        for obj_tile_idx in sorted(unique_object_tiles):
            if obj_tile_idx < len(all_sprite_data):
                tl_opt_idx = tile_mapping.get(obj_tile_idx * 4 + 0, 0)
                tr_opt_idx = tile_mapping.get(obj_tile_idx * 4 + 1, 0)
                bl_opt_idx = tile_mapping.get(obj_tile_idx * 4 + 2, 0)
                br_opt_idx = tile_mapping.get(obj_tile_idx * 4 + 3, 0)
                
                player_sprite_tile_indices = [1, 2, 3, 4, 5, 6, 7]
                if obj_tile_idx in player_sprite_tile_indices:
                    palette_idx = 3
                else:
                    palette_idx = object_palette_mapping.get(obj_tile_idx, 0)
                    if palette_idx >= len(object_palettes):
                        palette_idx = 0
                
                const_data.extend([obj_tile_idx, palette_idx, tl_opt_idx, tr_opt_idx, bl_opt_idx, br_opt_idx])
                const_data_no_collision.extend([obj_tile_idx, palette_idx, tl_opt_idx, tr_opt_idx, bl_opt_idx, br_opt_idx])
        
        # Add object palettes
        for pal_idx in range(3):
            if pal_idx < len(object_palettes):
                palette = object_palettes[pal_idx]
            else:
                palette = [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)]
            for r, g, b in palette:
                const_data.append(rgb_to_nes_6bit(r, g, b))
                const_data_no_collision.append(rgb_to_nes_6bit(r, g, b))
    else:
        # Add empty object palettes
        for pal_idx in range(3):
            for _ in range(4):
                const_data.append(0)
                const_data_no_collision.append(0)
    
    # Estimate LZSS and LZSA2 compressed sizes (with and without collision)
    compressed_size_lzss = estimate_lzss_size(const_data)
    compressed_size_lzss_no_collision = estimate_lzss_size(const_data_no_collision)
    compressed_size_lzsa2 = estimate_lzsa2_size(const_data)
    compressed_size_lzsa2_no_collision = estimate_lzsa2_size(const_data_no_collision)
    
    # Store in global list for final summary: (name, uncompressed_with_collision, lzss_with_collision, lzsa2_with_collision, uncompressed_no_collision, lzss_no_collision, lzsa2_no_collision)
    level_sizes.append((layer_name, total_bytes, compressed_size_lzss, compressed_size_lzsa2, total_bytes_no_collision, compressed_size_lzss_no_collision, compressed_size_lzsa2_no_collision))
    
    # Store data for combined compression (exclude level32)
    if layer_name != "level32":
        level_data_combined.append((layer_name, bytes(const_data), bytes(const_data_no_collision), total_bytes, total_bytes_no_collision))

    print(f"Generated tilemap header: {output_filename}")
    print(f"  Tilemap GIDs: {len(gid_array)} entries (1 byte each) = {tilemap_gid_bytes} bytes")
    print(f"  GID mapping: {len(gid_map_data)} unique GIDs (6 bytes each) = {gid_mapping_bytes} bytes")
    print(f"  Background palettes: {min(len(background_palettes), 4)}/4 = {background_palette_bytes} bytes")
    print(f"  Sprite palettes: {min(len(sprite_palettes), 4)}/4 = {sprite_palette_bytes} bytes")
    print(f"  Collision data: {len(collision_data)} entries = {collision_bytes} bytes")
    print(f"  Object data: {len(object_data)} objects = {object_bytes} bytes")
    if object_palettes and object_palette_mapping:
        print(f"  Object sprite data: {object_sprite_data_count} unique object sprites = {object_sprite_bytes} bytes")
        print(f"  Object palettes: {min(len(object_palettes), 3)}/3 (1 reserved for player) = {object_palette_bytes} bytes")
    else:
        print(f"  Object sprite data: 0 unique object sprites = 0 bytes")
        print(f"  Object palettes: 0/3 = {object_palette_bytes} bytes")
    print(f"  TOTAL CONST DATA SIZE: {total_bytes} bytes (uncompressed)")
    print(f"  ESTIMATED LZSS COMPRESSED SIZE: {compressed_size_lzss} bytes ({100.0 * compressed_size_lzss / total_bytes if total_bytes > 0 else 0:.1f}% of original)")
    print(f"  ESTIMATED LZSA2 COMPRESSED SIZE: {compressed_size_lzsa2} bytes ({100.0 * compressed_size_lzsa2 / total_bytes if total_bytes > 0 else 0:.1f}% of original)")
    print(f"  WITHOUT COLLISION DATA: {total_bytes_no_collision} bytes uncompressed")
    print(f"    LZSS: {compressed_size_lzss_no_collision} bytes, LZSA2: {compressed_size_lzsa2_no_collision} bytes")
    print(f"  SAVINGS FROM DROPPING COLLISION (LZSS): {collision_bytes} bytes uncompressed, {compressed_size_lzss - compressed_size_lzss_no_collision} bytes compressed")
    print(f"  SAVINGS FROM DROPPING COLLISION (LZSA2): {collision_bytes} bytes uncompressed, {compressed_size_lzsa2 - compressed_size_lzsa2_no_collision} bytes compressed")
    if spawn_found:
        print(f"  Spawn location: ({spawn_x}, {spawn_y})")
    else:
        print(f"  Spawn location: not found (defaulting to 0, 0)")


def collect_background_colors(img, tiles_per_row, num_rows, all_background_gids):
    """
    Collect all unique colors from background tiles (all_background_gids).
    Returns a Counter of (r, g, b) colors with their frequencies.
    """
    color_counter = Counter()

    for y in range(num_rows):
        for x in range(tiles_per_row):
            tile_index = y * tiles_per_row + x
            if tile_index in all_background_gids:
                box = (x * 8, y * 8, (x + 1) * 8, (y + 1) * 8)
                tile_8x8 = img.crop(box)
                tile_rgb = tile_8x8.convert('RGB')
                pixels = list(tile_rgb.getdata())
                for color in pixels:
                    color_counter[color] += 1

    return color_counter


def quantize_colors(color_counter, target_colors=9):
    """
    Quantize colors down to target_colors + black (10 total).
    Uses the most frequent colors, always including black.
    Maps colors to NES palette for accuracy.
    Returns a list of 10 colors: [black, color1, color2, ..., color9]
    """
    # Always include black
    black = (0, 0, 0)
    quantized = [black]

    # Remove black from counter if present
    if black in color_counter:
        del color_counter[black]

    # Get the most frequent colors
    most_common = color_counter.most_common(target_colors * 2)  # Get more candidates

    # Map colors to NES palette and deduplicate
    nes_colors_seen = set()
    for color, count in most_common:
        if color == black:
            continue

        # Map to NES color
        nes_idx = rgb_to_nes_6bit(color[0], color[1], color[2])
        nes_color = NES_PALETTE[nes_idx]

        if nes_color != black and nes_color not in nes_colors_seen:
            quantized.append(nes_color)
            nes_colors_seen.add(nes_color)
            if len(quantized) >= 10:  # black + 9 colors
                break

    # Pad with black if needed
    while len(quantized) < 10:
        quantized.append(black)

    return quantized[:10]


def create_background_palettes(quantized_colors):
    """
    Create 4 background palettes from 10 quantized colors.
    Each palette has 4 colors, with black always as color 0.
    Returns a list of 4 palettes, each with 4 colors.
    """
    black = (0, 0, 0)
    other_colors = [c for c in quantized_colors if c != black]

    # Distribute the 9 non-black colors across 4 palettes
    # Each palette: [black, color1, color2, color3]
    palettes = []

    # Strategy: distribute colors evenly
    colors_per_palette = 3  # 3 non-black colors per palette (black is always 0)

    for pal_idx in range(4):
        palette = [black]  # Always start with black

        # Add 3 colors from the quantized set
        for i in range(colors_per_palette):
            color_idx = (pal_idx * colors_per_palette + i) % len(other_colors)
            if color_idx < len(other_colors):
                palette.append(other_colors[color_idx])

        # Pad with black if needed
        while len(palette) < 4:
            palette.append(black)

        palettes.append(palette[:4])

    return palettes


def find_best_palette_for_tile(tile_image, background_palettes):
    """
    Find the best background palette for a tile by minimizing color distance.
    Returns (palette_index, remapped_palette) where remapped_palette is the best palette.
    """
    tile_rgb = tile_image.convert('RGB')
    pixels = list(tile_rgb.getdata())

    best_palette_idx = 0
    min_total_distance = float('inf')

    for pal_idx, palette in enumerate(background_palettes):
        total_distance = 0
        for r, g, b in pixels:
            # Find closest color in palette
            min_dist = float('inf')
            for pr, pg, pb in palette:
                dist = (r - pr)**2 + (g - pg)**2 + (b - pb)**2
                if dist < min_dist:
                    min_dist = dist
            total_distance += min_dist

        if total_distance < min_total_distance:
            min_total_distance = total_distance
            best_palette_idx = pal_idx

    return best_palette_idx, background_palettes[best_palette_idx]


def collect_object_colors(img, tiles_per_row, num_rows, arrMustBeObject):
    """
    Collect all unique colors from object tiles (arrMustBeObject).
    Returns a Counter of (r, g, b) colors with their frequencies.
    """
    color_counter = Counter()

    for y in range(num_rows):
        for x in range(tiles_per_row):
            tile_index = y * tiles_per_row + x
            if tile_index in arrMustBeObject:
                box = (x * 8, y * 8, (x + 1) * 8, (y + 1) * 8)
                tile_8x8 = img.crop(box)
                tile_rgb = tile_8x8.convert('RGB')
                pixels = list(tile_rgb.getdata())
                for color in pixels:
                    color_counter[color] += 1

    return color_counter


def create_object_palettes(quantized_colors, num_palettes=3):
    """
    Create object palettes from quantized colors.
    Each palette has 4 colors.
    Note: We use 3 palettes for objects, reserving 1 palette for the player.
    Returns a list of palettes, each with 4 colors.
    """
    black = (0, 0, 0)
    other_colors = [c for c in quantized_colors if c != black]

    palettes = []

    # Strategy: distribute colors evenly across palettes
    colors_per_palette = 3  # 3 non-black colors per palette (black is always 0)

    for pal_idx in range(num_palettes):
        palette = [black]  # Always start with black

        # Add 3 colors from the quantized set
        for i in range(colors_per_palette):
            color_idx = (pal_idx * colors_per_palette + i) % len(other_colors)
            if color_idx < len(other_colors):
                palette.append(other_colors[color_idx])

        # Pad with black if needed
        while len(palette) < 4:
            palette.append(black)

        palettes.append(palette[:4])

    return palettes


def find_best_object_palette_for_tile(tile_image, object_palettes):
    """
    Find the best object palette for a tile by minimizing color distance.
    Returns (palette_index, remapped_palette) where remapped_palette is the best palette.
    """
    tile_rgb = tile_image.convert('RGB')
    pixels = list(tile_rgb.getdata())

    best_palette_idx = 0
    min_total_distance = float('inf')

    for pal_idx, palette in enumerate(object_palettes):
        total_distance = 0
        for r, g, b in pixels:
            # Find closest color in palette
            min_dist = float('inf')
            for pr, pg, pb in palette:
                dist = (r - pr)**2 + (g - pg)**2 + (b - pb)**2
                if dist < min_dist:
                    min_dist = dist
            total_distance += min_dist

        if total_distance < min_total_distance:
            min_total_distance = total_distance
            best_palette_idx = pal_idx

    return best_palette_idx, object_palettes[best_palette_idx]


def deduplicate_tiles(all_chr_tiles):
    """
    Remove duplicate 8x8 tiles and create a mapping from original to optimized indices.
    
    Returns:
        (unique_tiles, tile_mapping)
        - unique_tiles: List of unique 8x8 tile data (bytearrays)
        - tile_mapping: Dictionary mapping original_tile_index -> optimized_tile_index
    """
    unique_tiles = []
    tile_mapping = {}
    tile_to_index = {}  # Maps tile bytes (as tuple) to optimized index

    for original_idx, tile_data in enumerate(all_chr_tiles):
        # Convert bytearray to tuple for hashing
        tile_key = tuple(tile_data)

        if tile_key in tile_to_index:
            # Duplicate tile found, reuse existing index
            tile_mapping[original_idx] = tile_to_index[tile_key]
        else:
            # New unique tile
            optimized_idx = len(unique_tiles)
            unique_tiles.append(tile_data)
            tile_to_index[tile_key] = optimized_idx
            tile_mapping[original_idx] = optimized_idx

    return unique_tiles, tile_mapping


def create_map_preview(tilemap_data, optimized_chr_tiles, tile_mapping, all_palettes, all_sprite_data, output_path, map_width_tiles, map_height_tiles, scale=2, show_grid=True):
    """
    Create a preview image of the map using optimized NES CHR data.
    
    Args:
        tilemap_data: List of tile GIDs from the JSON map
        optimized_chr_tiles: List of unique CHR tile data (8x8 tiles)
        tile_mapping: Dictionary mapping original_tile_index -> optimized_tile_index
        all_palettes: List of all palettes
        all_sprite_data: List of (sprite_4tiles, palette) tuples (uses original indices)
        output_path: Path to save the preview
        map_width_tiles: Width of map in tiles
        map_height_tiles: Height of map in tiles
        scale: Scale factor for preview (2 = 16x16 pixels per 8x8 tile)
        show_grid: Whether to show 8x8 tile grid overlay
    """

    # Calculate preview size (each 8x8 tile becomes 16x16 in preview)
    tile_size_scaled = 8 * scale
    preview_width = map_width_tiles * tile_size_scaled
    preview_height = map_height_tiles * tile_size_scaled

    preview = Image.new('RGB', (preview_width, preview_height), (0, 0, 0))

    # Track palette indices for each 8x8 tile position
    # Each 16x16 sprite contains 4 8x8 tiles, so we need 2x the dimensions
    palette_indices_8x8 = [[-1] * (map_width_tiles * 2) for _ in range(map_height_tiles * 2)]

    # Collect unique background palettes only (map preview only shows background tiles)
    background_palettes = []
    background_palette_to_index = {}

    # Process each tile in the map
    for i, tile_gid in enumerate(tilemap_data):
        if tile_gid == 0:
            continue  # Empty tile

        # Calculate tile position in map
        map_x_tile = i % map_width_tiles
        map_y_tile = i // map_width_tiles

        # Extract flip flags
        FLIPPED_HORIZONTALLY_FLAG = 0x80000000
        FLIPPED_VERTICALLY_FLAG = 0x40000000
        FLIPPED_DIAGONALLY_FLAG = 0x20000000

        flipped_horizontally = bool(tile_gid & FLIPPED_HORIZONTALLY_FLAG)
        flipped_vertically = bool(tile_gid & FLIPPED_VERTICALLY_FLAG)
        flipped_diagonally = bool(tile_gid & FLIPPED_DIAGONALLY_FLAG)

        # Clear flags to get actual tile index
        tile_gid &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG)

        # Convert GID to original tile index (GID is 1-based, tiles are 0-based)
        original_tile_index = tile_gid - 1

        if original_tile_index < 0 or original_tile_index >= len(all_sprite_data):
            continue  # Invalid tile index

        # Skip player (tile index 1) - don't draw in preview
        if original_tile_index == 1:
            continue  # Skip drawing player tile

        # Skip tiles in arrMustBeObject - don't draw them in preview
        if original_tile_index in arrMustBeObject:
            continue  # Skip drawing this tile

        # Get the sprite data (4 tiles for 16x16 sprite)
        sprite_4tiles, palette = all_sprite_data[original_tile_index]

        # Determine palette index (same logic as generate_nes_tilemap_header)
        is_background = original_tile_index in all_background_gids

        # Skip sprite tiles - only show background tiles in map preview
        if not is_background:
            continue  # Skip drawing sprite tiles

        # Normalize palette: pad to 4 colors
        normalized_palette = list(palette)
        while len(normalized_palette) < 4:
            normalized_palette.append((0, 0, 0))
        normalized_palette = normalized_palette[:4]

        # Background tiles only (sprite tiles are skipped above)
        palette_key = tuple(normalized_palette)
        if palette_key not in background_palette_to_index:
            if len(background_palettes) >= 4:
                palette_idx_encoded = 0
            else:
                background_palette_to_index[palette_key] = len(background_palettes)
                background_palettes.append(normalized_palette)
                palette_idx_encoded = len(background_palettes) - 1
        else:
            palette_idx_encoded = background_palette_to_index[palette_key]

        # Validate: background palette index must be 0-3
        if palette_idx_encoded > 3:
            print(f"WARNING: Background palette index {palette_idx_encoded} exceeds limit of 3, clamping to 0")
            palette_idx_encoded = 0

        # Store palette index for all 4 8x8 tiles in this sprite
        # Each 16x16 sprite is at position (map_x_tile, map_y_tile)
        # It contains 4 8x8 tiles at positions:
        # TL: (map_x_tile * 2, map_y_tile * 2)
        # TR: (map_x_tile * 2 + 1, map_y_tile * 2)
        # BL: (map_x_tile * 2, map_y_tile * 2 + 1)
        # BR: (map_x_tile * 2 + 1, map_y_tile * 2 + 1)
        tl_8x8_x = map_x_tile * 2
        tl_8x8_y = map_y_tile * 2
        if tl_8x8_x < map_width_tiles * 2 and tl_8x8_y < map_height_tiles * 2:
            palette_indices_8x8[tl_8x8_y][tl_8x8_x] = palette_idx_encoded
            palette_indices_8x8[tl_8x8_y][tl_8x8_x + 1] = palette_idx_encoded
            palette_indices_8x8[tl_8x8_y + 1][tl_8x8_x] = palette_idx_encoded
            palette_indices_8x8[tl_8x8_y + 1][tl_8x8_x + 1] = palette_idx_encoded

        # Map original tile indices to optimized indices
        # Each sprite has 4 tiles, so we need to map each one
        tl_opt_idx = tile_mapping.get(original_tile_index * 4 + 0, 0)
        tr_opt_idx = tile_mapping.get(original_tile_index * 4 + 1, 0)
        bl_opt_idx = tile_mapping.get(original_tile_index * 4 + 2, 0)
        br_opt_idx = tile_mapping.get(original_tile_index * 4 + 3, 0)

        # Get optimized tile data
        tl_tile = optimized_chr_tiles[tl_opt_idx] if tl_opt_idx < len(optimized_chr_tiles) else bytearray(16)
        tr_tile = optimized_chr_tiles[tr_opt_idx] if tr_opt_idx < len(optimized_chr_tiles) else bytearray(16)
        bl_tile = optimized_chr_tiles[bl_opt_idx] if bl_opt_idx < len(optimized_chr_tiles) else bytearray(16)
        br_tile = optimized_chr_tiles[br_opt_idx] if br_opt_idx < len(optimized_chr_tiles) else bytearray(16)

        # Reconstruct 16x16 sprite from 4 8x8 tiles using optimized CHR data
        sprite_16x16 = Image.new('RGB', (16, 16))

        # Decode each tile from optimized CHR data
        tl_pixels = decode_tile_from_2bpp(tl_tile, palette)
        tr_pixels = decode_tile_from_2bpp(tr_tile, palette)
        bl_pixels = decode_tile_from_2bpp(bl_tile, palette)
        br_pixels = decode_tile_from_2bpp(br_tile, palette)

        # Top row: TL and TR
        for y in range(8):
            for x in range(8):
                sprite_16x16.putpixel((x, y), tl_pixels[y*8 + x])
                sprite_16x16.putpixel((x+8, y), tr_pixels[y*8 + x])

        # Bottom row: BL and BR
        for y in range(8):
            for x in range(8):
                sprite_16x16.putpixel((x, y+8), bl_pixels[y*8 + x])
                sprite_16x16.putpixel((x+8, y+8), br_pixels[y*8 + x])

        # Apply transformations
        if flipped_diagonally:
            if flipped_horizontally and flipped_vertically:
                sprite_16x16 = sprite_16x16.transpose(Image.ROTATE_270)
                sprite_16x16 = sprite_16x16.transpose(Image.FLIP_LEFT_RIGHT)
            elif flipped_horizontally:
                sprite_16x16 = sprite_16x16.transpose(Image.ROTATE_270)
            elif flipped_vertically:
                sprite_16x16 = sprite_16x16.transpose(Image.ROTATE_90)
            else:
                sprite_16x16 = sprite_16x16.transpose(Image.ROTATE_270)
                sprite_16x16 = sprite_16x16.transpose(Image.FLIP_TOP_BOTTOM)
        else:
            if flipped_horizontally:
                sprite_16x16 = sprite_16x16.transpose(Image.FLIP_LEFT_RIGHT)
            if flipped_vertically:
                sprite_16x16 = sprite_16x16.transpose(Image.FLIP_TOP_BOTTOM)

        # Scale up the sprite
        sprite_scaled = sprite_16x16.resize((tile_size_scaled, tile_size_scaled), Image.NEAREST)

        # Paste into preview
        x = map_x_tile * tile_size_scaled
        y = map_y_tile * tile_size_scaled
        preview.paste(sprite_scaled, (x, y))

    # Draw grid overlay showing 8x8 tile boundaries
    if show_grid:
        draw = ImageDraw.Draw(preview)
        grid_color = (200, 200, 200)

        # Draw vertical lines (every tile_size_scaled pixels)
        for x in range(0, preview_width + 1, tile_size_scaled):
            draw.line([(x, 0), (x, preview_height)], fill=grid_color, width=1)

        # Draw horizontal lines (every tile_size_scaled pixels)
        for y in range(0, preview_height + 1, tile_size_scaled):
            draw.line([(0, y), (preview_width, y)], fill=grid_color, width=1)

    # Draw transparent overlay showing palette indices for each 8x8 tile
    overlay = Image.new('RGBA', (preview_width, preview_height), (0, 0, 0, 0))
    overlay_draw = ImageDraw.Draw(overlay)

    # Color mapping for palette indices (background palettes only: 0-3)
    # Background palettes: 0-3 (blue tones)
    # Alpha set to ~20% (51 out of 255)
    alpha = 51
    palette_colors = [
        (0, 0, 255, alpha),    # 0: Blue (background)
        (0, 128, 255, alpha),  # 1: Light blue (background)
        (128, 128, 255, alpha), # 2: Purple-blue (background)
        (0, 255, 255, alpha),  # 3: Cyan (background)
        (255, 0, 0, alpha),    # 4: Red (sprite)
        (255, 128, 0, alpha),  # 5: Orange (sprite)
        (255, 0, 255, alpha),  # 6: Magenta (sprite)
        (255, 128, 128, alpha), # 7: Pink (sprite)
    ]

    # Draw overlay rectangles for each 8x8 tile
    tile_8x8_size = tile_size_scaled // 2  # Each 8x8 tile is half the size of a 16x16 sprite
    for y_8x8 in range(map_height_tiles * 2):
        for x_8x8 in range(map_width_tiles * 2):
            palette_idx = palette_indices_8x8[y_8x8][x_8x8]
            if palette_idx >= 0:
                # Draw semi-transparent rectangle
                x_pixel = x_8x8 * tile_8x8_size
                y_pixel = y_8x8 * tile_8x8_size
                color = palette_colors[palette_idx % len(palette_colors)]
                overlay_draw.rectangle(
                    [(x_pixel, y_pixel), (x_pixel + tile_8x8_size - 1, y_pixel + tile_8x8_size - 1)],
                    fill=color
                )

    # Draw palette index number only once per 16x16 sprite (centered)
    # Iterate over 16x16 sprite positions
    font_size = max(10, tile_size_scaled // 2)
    try:
        from PIL import ImageFont
        font = ImageFont.truetype("arial.ttf", font_size)
    except:
        try:
            font = ImageFont.load_default()
        except:
            font = None

    for y_sprite in range(map_height_tiles):
        for x_sprite in range(map_width_tiles):
            # Get palette index from top-left 8x8 tile of this sprite
            y_8x8 = y_sprite * 2
            x_8x8 = x_sprite * 2
            if y_8x8 < map_height_tiles * 2 and x_8x8 < map_width_tiles * 2:
                palette_idx = palette_indices_8x8[y_8x8][x_8x8]
                if palette_idx >= 0:
                    # Calculate center of 16x16 sprite
                    x_pixel = x_sprite * tile_size_scaled
                    y_pixel = y_sprite * tile_size_scaled
                    # Draw palette index label in the center of the sprite
                    # Map preview only shows background tiles, so palette indices are always 0-3
                    # Format: "B0", "B1", "B2", or "B3"
                    text = f"B{palette_idx}"
                    # Calculate text position (centered in 16x16 sprite)
                    bbox = overlay_draw.textbbox((0, 0), text, font=font)
                    text_width = bbox[2] - bbox[0]
                    text_height = bbox[3] - bbox[1]
                    text_x = x_pixel + (tile_size_scaled - text_width) // 2
                    text_y = y_pixel + (tile_size_scaled - text_height) // 2
                    # Use white text
                    text_color = (255, 255, 255, 255)
                    overlay_draw.text((text_x, text_y), text, fill=text_color, font=font)

    # Composite the overlay onto the preview
    preview_rgba = preview.convert('RGBA')
    preview_rgba = Image.alpha_composite(preview_rgba, overlay)
    preview = preview_rgba.convert('RGB')

    preview.save(output_path)
    return preview


def main():
    # Clear level sizes list for this run
    global level_sizes, level_data_combined
    level_sizes = []
    level_data_combined = []
    
    # Get the directory where this script is located
    script_dir = os.path.dirname(os.path.abspath(__file__))

    image_filename = os.path.join(script_dir, 'baseCelesteSpriteSheet.png')
    output_chr_bank0 = os.path.join(script_dir, 'sprite_chr_bank0.bin')
    output_chr_bank1 = os.path.join(script_dir, 'sprite_chr_bank1.bin')
    output_preview_bank0 = os.path.join(script_dir, 'sprite_chr_bank0_preview.png')
    output_preview_bank1 = os.path.join(script_dir, 'sprite_chr_bank1_preview.png')

    # Check for Pillow installation
    try:
        from PIL import Image
    except ImportError:
        print("Pillow library not found. Please install it using: pip install Pillow")
        return

    if not os.path.exists(image_filename):
        print(f"Error: '{image_filename}' not found.")
        print(f"Script directory: {script_dir}")
        print(f"Current working directory: {os.getcwd()}")
        return

    img = Image.open(image_filename)
    width, height = img.size

    if width % 8 != 0 or height % 8 != 0:
        print("Warning: Image dimensions are not a multiple of 8. Some parts may be cropped.")

    # Calculate number of tiles
    tiles_per_row = width // 8
    num_rows = height // 8
    total_tiles = tiles_per_row * num_rows

    print(f"Processing {total_tiles} tiles from {width}x{height} image...")

    # First, collect colors from background tiles for quantization
    print("\nCollecting colors from background tiles...")
    background_color_counter = collect_background_colors(img, tiles_per_row, num_rows, all_background_gids)
    print(f"  Found {len(background_color_counter)} unique colors in background tiles")

    # Quantize to 9 colors + black and create 4 palettes
    background_palettes = None
    if len(background_color_counter) > 0:
        quantized_colors = quantize_colors(background_color_counter, target_colors=9)
        print(f"  Quantized to {len(quantized_colors)} colors (black + 9 colors)")

        background_palettes = create_background_palettes(quantized_colors)
        print(f"  Created {len(background_palettes)} background palettes")
        for i, pal in enumerate(background_palettes):
            print(f"    Palette {i}: {pal}")
    else:
        print("  No background tiles found, using default palettes")
        background_palettes = [[(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)] for _ in range(4)]

    # Collect colors from object tiles for quantization
    print("\nCollecting colors from object tiles...")
    object_color_counter = collect_object_colors(img, tiles_per_row, num_rows, arrMustBeObject)
    print(f"  Found {len(object_color_counter)} unique colors in object tiles")

    # Quantize object colors to 9 colors + black and create 3 palettes (1 reserved for player)
    object_palettes = None
    if len(object_color_counter) > 0:
        object_quantized_colors = quantize_colors(object_color_counter, target_colors=9)
        print(f"  Quantized to {len(object_quantized_colors)} colors (black + 9 colors)")

        object_palettes = create_object_palettes(object_quantized_colors, num_palettes=3)
        print(f"  Created {len(object_palettes)} object palettes (1 palette reserved for player)")
        for i, pal in enumerate(object_palettes):
            print(f"    Object Palette {i}: {pal}")
    else:
        print("  No object tiles found, using default palettes")
        object_palettes = [[(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)] for _ in range(3)]

    # Process all tiles - each 8x8 tile becomes a 16x16 sprite (4 8x8 tiles)
    all_sprite_data = []  # List of (sprite_4tiles, palette) tuples
    all_palettes = []
    all_chr_tiles = []  # Flat list of all 8x8 tile data for CHR banks
    object_palette_mapping = {}  # Maps tile_index -> object_palette_index (0-2)

    # Player sprite frames map to sprite sheet tile indices: 1, 2, 3, 4, 5, 6, 7
    # (corresponding to PLAYER_SPRITE_IDLE=0, WALK_1=2, WALK_2=4, WALK_3=6, WALL=8, DOWN=10, UP=12)
    player_sprite_tile_indices = [1, 2, 3, 4, 5, 6, 7]

    for y in range(num_rows):
        for x in range(tiles_per_row):
            box = (x * 8, y * 8, (x + 1) * 8, (y + 1) * 8)
            tile_8x8 = img.crop(box)
            tile_index = y * tiles_per_row + x

            # For background tiles, use quantized palette; for objects use object palettes; otherwise extract palette
            # Player sprite frames (tile indices 1-7) should extract their own palette
            if tile_index in all_background_gids:
                # Find best palette from background_palettes
                best_palette_idx, palette = find_best_palette_for_tile(tile_8x8, background_palettes)
            elif tile_index in arrMustBeObject:
                # Use optimized object palettes
                best_palette_idx, palette = find_best_object_palette_for_tile(tile_8x8, object_palettes)
                object_palette_mapping[tile_index] = best_palette_idx
            elif tile_index in player_sprite_tile_indices:
                # Player sprites (tile indices 1-7): extract palette directly (they'll use sprite palette 3)
                palette = extract_palette(tile_8x8, max_colors=4)
                # Debug: print player palette if it's not all black
                if palette and len(palette) > 0:
                    is_all_black = all(color == (0, 0, 0) for color in palette)
                    if not is_all_black:
                        print(f"Player sprite tile index {tile_index} palette: {palette}")
                    else:
                        print(f"WARNING: Player sprite tile index {tile_index} has all-black palette!")
            else:
                # Extract palette (max 4 colors for NES)
                palette = extract_palette(tile_8x8, max_colors=4)

            # Convert 8x8 tile to 16x16 sprite (4 8x8 tiles)
            sprite_4tiles = convert_8x8_to_16x16_sprite(tile_8x8, palette)

            all_sprite_data.append((sprite_4tiles, palette))
            all_palettes.append(palette)

            # Add all 4 tiles to the flat CHR list
            all_chr_tiles.extend(sprite_4tiles)

    print(f"\nProcessed {len(all_sprite_data)} original 8x8 tiles")
    print(f"Generated {len(all_chr_tiles)} 8x8 tiles for CHR output (4 tiles per sprite)")

    # Deduplicate tiles to optimize CHR usage
    print("\nDeduplicating tiles...")
    unique_chr_tiles, tile_mapping = deduplicate_tiles(all_chr_tiles)
    print(f"  Original tiles: {len(all_chr_tiles)}")
    print(f"  Unique tiles: {len(unique_chr_tiles)}")
    print(f"  Duplicates removed: {len(all_chr_tiles) - len(unique_chr_tiles)}")
    print(f"  Space saved: {((len(all_chr_tiles) - len(unique_chr_tiles)) * 16)} bytes")

    # Split into CHR banks using optimized unique tiles
    # Bank 0: 4096 bytes = 256 tiles
    # Bank 1: 4096 bytes = 256 tiles
    bank0_size = 4096
    bank1_size = 4096
    tiles_per_bank0 = bank0_size // 16  # 256 tiles
    tiles_per_bank1 = bank1_size // 16  # 256 tiles

    # Write CHR Bank 0 (using optimized unique tiles)
    bank0_data = bytearray()
    bank0_tiles = unique_chr_tiles[:tiles_per_bank0]

    for tile_data in bank0_tiles:
        bank0_data.extend(tile_data)

    # Pad bank 0 if needed
    while len(bank0_data) < bank0_size:
        bank0_data.append(0)

    # Write CHR Bank 1 (using optimized unique tiles)
    bank1_data = bytearray()
    bank1_tiles = unique_chr_tiles[tiles_per_bank0:tiles_per_bank0 + tiles_per_bank1]

    for tile_data in bank1_tiles:
        bank1_data.extend(tile_data)

    # Pad bank 1 if needed
    while len(bank1_data) < bank1_size:
        bank1_data.append(0)

    # Create optimized sprite data for previews (using optimized tile indices)
    # We need to map the sprite data to use optimized tile indices
    optimized_sprite_data = []
    for sprite_idx, (sprite_4tiles, palette) in enumerate(all_sprite_data):
        # Map each of the 4 tiles in the sprite to optimized indices
        optimized_4tiles = []
        for tile_idx_in_sprite in range(4):
            original_tile_idx = sprite_idx * 4 + tile_idx_in_sprite
            optimized_tile_idx = tile_mapping.get(original_tile_idx, 0)
            # Get the optimized tile data
            if optimized_tile_idx < len(unique_chr_tiles):
                optimized_4tiles.append(unique_chr_tiles[optimized_tile_idx])
        else:
            optimized_4tiles.append(bytearray(16))
        optimized_sprite_data.append((optimized_4tiles, palette))

    # For preview, we need to filter sprites that fit in each bank
    # Since tiles are now optimized, we need to check which sprites can be rendered
    # based on whether all their tiles are in the bank
    bank0_sprites = []
    bank1_sprites = []

    for sprite_idx, (optimized_4tiles, palette) in enumerate(optimized_sprite_data):
        # Check if all 4 tiles are in bank 0
        all_in_bank0 = all(
            tile_mapping.get(sprite_idx * 4 + i, tiles_per_bank0) < tiles_per_bank0
            for i in range(4)
        )
        if all_in_bank0 and sprite_idx < len(all_sprite_data):
            bank0_sprites.append((optimized_4tiles, palette))
        elif sprite_idx < len(all_sprite_data):
            # Check if all tiles are in bank 1
            all_in_bank1 = all(
                tiles_per_bank0 <= tile_mapping.get(sprite_idx * 4 + i, tiles_per_bank0 + tiles_per_bank1) < tiles_per_bank0 + tiles_per_bank1
                for i in range(4)
            )
            if all_in_bank1:
                bank1_sprites.append((optimized_4tiles, palette))

    # Write CHR files
    with open(output_chr_bank0, 'wb') as f:
        f.write(bank0_data)

    with open(output_chr_bank1, 'wb') as f:
        f.write(bank1_data)

    # Create combined CHR file
    output_chr_combined = os.path.join(script_dir, 'sprite_chr_combined.bin')
    combined_data = bank0_data + bank1_data
    with open(output_chr_combined, 'wb') as f:
        f.write(combined_data)

    print(f"Wrote {len(bank0_data)} bytes to {output_chr_bank0} ({len(bank0_tiles)} unique 8x8 tiles)")
    print(f"Wrote {len(bank1_data)} bytes to {output_chr_bank1} ({len(bank1_tiles)} unique 8x8 tiles)")
    print(f"Wrote {len(combined_data)} bytes to {output_chr_combined} (combined CHR data)")

    # Create separate preview images for each bank (16x16 sprites with grid overlay)
    # Use optimized sprite data for previews
    print("Creating preview images...")
    if bank0_sprites:
        preview_bank0 = create_preview_image(bank0_sprites, tiles_per_row=tiles_per_row, scale=2, show_grid=True)
        preview_bank0.save(output_preview_bank0)
        print(f"Saved Bank 0 preview to {output_preview_bank0} ({len(bank0_sprites)} 16x16 sprites)")

    if bank1_sprites:
        preview_bank1 = create_preview_image(bank1_sprites, tiles_per_row=tiles_per_row, scale=2, show_grid=True)
        preview_bank1.save(output_preview_bank1)
        print(f"Saved Bank 1 preview to {output_preview_bank1} ({len(bank1_sprites)} 16x16 sprites)")

    # Statistics
    unique_palettes = {}
    for palette in all_palettes:
        palette_key = tuple(palette)
        unique_palettes[palette_key] = unique_palettes.get(palette_key, 0) + 1

    print(f"\nStatistics:")
    print(f"  Original 8x8 tiles: {len(all_sprite_data)}")
    print(f"  Total 8x8 CHR tiles (before dedup): {len(all_chr_tiles)} (4 per sprite)")
    print(f"  Unique 8x8 CHR tiles (after dedup): {len(unique_chr_tiles)}")
    print(f"  Bank 0: {len(bank0_tiles)} unique 8x8 tiles ({len(bank0_sprites)} 16x16 sprites)")
    print(f"  Bank 1: {len(bank1_tiles)} unique 8x8 tiles ({len(bank1_sprites)} 16x16 sprites)")
    print(f"  Unique palettes: {len(unique_palettes)}")

    # Check if we have more tiles than can fit in both banks
    max_tiles = tiles_per_bank0 + tiles_per_bank1
    if len(unique_chr_tiles) > max_tiles:
        print(f"  WARNING: {len(unique_chr_tiles)} unique 8x8 tiles exceed capacity of {max_tiles} tiles!")
        print(f"  Only first {max_tiles} tiles were written to CHR banks.")
    else:
        remaining_tiles = max_tiles - len(unique_chr_tiles)
        print(f"  Remaining capacity: {remaining_tiles} unique 8x8 tiles")

    # Process map JSON and create level previews and .h files
    tilemap_filename = os.path.join(script_dir, 'baseCelesteTileMap.json')
    if os.path.exists(tilemap_filename):
        print("\nProcessing map JSON file...")
        try:
            with open(tilemap_filename, 'r') as f:
                tilemap_json = json.load(f)

            map_width_global = tilemap_json.get("width", 128)
            map_height_global = tilemap_json.get("height", 64)

            # Process each layer
            for layer in tilemap_json.get("layers", []):
                if layer.get("type") == "tilelayer":
                    decode_tiled_layer_data(layer)
                    tile_data_all = layer.get("data", [])

                    if not tile_data_all:
                        continue

                    # Split map into 16x16 tile levels (8x4 grid = 32 levels)
                    xMaps = 8
                    yMaps = 4
                    map_width_tiles_local = 16
                    map_height_tiles_local = 16

                    # Build shared GID mapping that will be used by all levels
                    # Initialize shared mapping
                    shared_tile_to_gid = {}  # Maps (tl, tr, bl, br, pal_idx, flip_flags) -> GID
                    shared_gid_map_data = []  # List of tile entries indexed by GID
                    
                    # Reserve GID 0 for empty tiles
                    empty_tile = (0, 0, 0, 0, 0, 0)
                    shared_tile_to_gid[empty_tile] = 0
                    shared_gid_map_data.append(empty_tile)
                    next_shared_gid = 1
                    
                    # Process all levels to build shared GID mapping
                    print("\nBuilding shared GID mapping from all levels...")
                    all_levels_data = []  # Store level data for second pass
                    
                    for mapNum in range(xMaps * yMaps):
                        level_name = f"level{mapNum+1}"

                        # Calculate top-left corner of this submap
                        topLeftX = (mapNum % xMaps) * map_width_tiles_local
                        topLeftY = (mapNum // xMaps) * map_height_tiles_local

                        # Extract 16x16 tile data for this level
                        submap_data = []
                        for y in range(map_height_tiles_local):
                            row = []
                            for x in range(map_width_tiles_local):
                                global_x = topLeftX + x
                                global_y = topLeftY + y

                                # Check bounds
                                if global_x < map_width_global and global_y < map_height_global:
                                    index = global_y * map_width_global + global_x
                                    if index < len(tile_data_all):
                                        row.append(tile_data_all[index])
                                    else:
                                        row.append(0)
                                else:
                                    row.append(0)
                            submap_data.append(row)

                        # Flatten submap_data
                        flat_submap_data = []
                        for row in submap_data:
                            flat_submap_data.extend(row)
                        
                        all_levels_data.append((level_name, flat_submap_data))
                    
                    # First pass: process all levels to collect unique tile entries
                    # We'll call generate_nes_tilemap_header with collect_only=True to build shared mapping
                    # Actually, let's just process levels and collect as we go, building shared mapping incrementally
                    # For now, process first level to initialize, then others add to it
                    shared_gid_mapping = None  # Will be built incrementally
                    
                    # Generate headers for each level (shared mapping built incrementally)
                    for mapNum in range(xMaps * yMaps):
                        level_name = f"level{mapNum+1}"
                        flat_submap_data = all_levels_data[mapNum][1]
                        level_name = f"level{mapNum+1}"

                        # Calculate top-left corner of this submap
                        topLeftX = (mapNum % xMaps) * map_width_tiles_local
                        topLeftY = (mapNum // xMaps) * map_height_tiles_local

                        # Extract 16x16 tile data for this level
                        submap_data = []
                        for y in range(map_height_tiles_local):
                            row = []
                            for x in range(map_width_tiles_local):
                                global_x = topLeftX + x
                                global_y = topLeftY + y

                                # Check bounds
                                if global_x < map_width_global and global_y < map_height_global:
                                    index = global_y * map_width_global + global_x
                                    if index < len(tile_data_all):
                                        row.append(tile_data_all[index])
                                    else:
                                        row.append(0)
                                else:
                                    row.append(0)
                            submap_data.append(row)

                        # Flatten submap_data for preview and header generation
                        flat_submap_data = []
                        for row in submap_data:
                            flat_submap_data.extend(row)

                        # Create preview for this level
                        output_map_preview = os.path.join(script_dir, f'level_map_preview_{level_name}.png')

                        print(f"Creating map preview for {level_name} (16x16 tiles)...")
                        create_map_preview(
                            flat_submap_data,
                            unique_chr_tiles,
                            tile_mapping,
                            all_palettes,
                            all_sprite_data,
                            output_map_preview,
                            map_width_tiles_local,
                            map_height_tiles_local,
                            scale=2,
                            show_grid=True
                        )
                        print(f"Saved map preview to {output_map_preview}")

                        # Generate .h file with tilemap, palette, collision, object, and spawn data for this level
                        generate_nes_tilemap_header(
                            flat_submap_data,
                            level_name,
                            map_width_tiles_local,
                            map_height_tiles_local,
                            all_sprite_data,
                            tile_mapping,
                            unique_chr_tiles,
                            all_palettes,
                            object_palettes,
                            object_palette_mapping
                        )
                    
                    # Generate shared GID mapping header after all levels are processed
                    # Ensure it's always generated, even if empty (for compilation)
                    shared_gid_header_filename = os.path.join(script_dir, 'gid_to_tile_shared.h')
                    print(f"\nGenerating shared GID mapping header: {shared_gid_header_filename}")
                    with open(shared_gid_header_filename, 'w') as f:
                        f.write("// Shared GID to tile mapping for all levels\n")
                        f.write("// Generated from baseCelesteTileMap.json\n")
                        f.write("// Each entry: TL_tile, TR_tile, BL_tile, BR_tile, palette_idx, flip_flags\n")
                        f.write("// flip_flags: bit 0=H, bit 1=V, bit 2=D\n")
                        f.write("// GID 0 is reserved for empty tiles\n\n")
                        f.write("#ifndef GID_TO_TILE_SHARED_H\n")
                        f.write("#define GID_TO_TILE_SHARED_H\n\n")
                        if len(shared_gid_mapping_global['gid_map_data']) > 0:
                            f.write(f"const unsigned char gid_to_tile_shared[{len(shared_gid_mapping_global['gid_map_data'])}][6] = {{\n")
                            for gid in range(len(shared_gid_mapping_global['gid_map_data'])):
                                tl, tr, bl, br, pal_idx, flip = shared_gid_mapping_global['gid_map_data'][gid]
                                f.write(f"    // GID {gid}\n")
                                f.write(f"    {{ {tl}, {tr}, {bl}, {br}, {pal_idx}, {flip} }}")
                                if gid < len(shared_gid_mapping_global['gid_map_data']) - 1:
                                    f.write(",")
                                f.write("\n")
                            f.write("};\n\n")
                            f.write(f"#define GID_TO_TILE_SHARED_COUNT {len(shared_gid_mapping_global['gid_map_data'])}\n\n")
                        else:
                            # Empty mapping (just empty tile)
                            f.write("const unsigned char gid_to_tile_shared[1][6] = {\n")
                            f.write("    // GID 0 (empty tile)\n")
                            f.write("    { 0, 0, 0, 0, 0, 0 }\n")
                            f.write("};\n\n")
                            f.write("#define GID_TO_TILE_SHARED_COUNT 1\n\n")
                        f.write("#endif // GID_TO_TILE_SHARED_H\n")
                    print(f"Generated shared GID mapping with {len(shared_gid_mapping_global['gid_map_data']) if len(shared_gid_mapping_global['gid_map_data']) > 0 else 1} unique GIDs")
        except Exception as e:
            print(f"Error processing map JSON: {e}")
            import traceback
            traceback.print_exc()
    else:
        print(f"\nMap JSON file '{tilemap_filename}' not found, skipping map preview generation.")
    
    # Ensure shared GID mapping header is always generated (even if no levels were processed)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    shared_gid_header_filename = os.path.join(script_dir, 'gid_to_tile_shared.h')
    if not os.path.exists(shared_gid_header_filename) or len(shared_gid_mapping_global['gid_map_data']) == 0:
        print(f"\nGenerating shared GID mapping header: {shared_gid_header_filename}")
        with open(shared_gid_header_filename, 'w') as f:
            f.write("// Shared GID to tile mapping for all levels\n")
            f.write("// Generated from baseCelesteTileMap.json\n")
            f.write("// Each entry: TL_tile, TR_tile, BL_tile, BR_tile, palette_idx, flip_flags\n")
            f.write("// flip_flags: bit 0=H, bit 1=V, bit 2=D\n")
            f.write("// GID 0 is reserved for empty tiles\n\n")
            f.write("#ifndef GID_TO_TILE_SHARED_H\n")
            f.write("#define GID_TO_TILE_SHARED_H\n\n")
            if len(shared_gid_mapping_global['gid_map_data']) > 0:
                f.write(f"const unsigned char gid_to_tile_shared[{len(shared_gid_mapping_global['gid_map_data'])}][6] = {{\n")
                for gid in range(len(shared_gid_mapping_global['gid_map_data'])):
                    tl, tr, bl, br, pal_idx, flip = shared_gid_mapping_global['gid_map_data'][gid]
                    f.write(f"    // GID {gid}\n")
                    f.write(f"    {{ {tl}, {tr}, {bl}, {br}, {pal_idx}, {flip} }}")
                    if gid < len(shared_gid_mapping_global['gid_map_data']) - 1:
                        f.write(",")
                    f.write("\n")
                f.write("};\n\n")
                f.write(f"#define GID_TO_TILE_SHARED_COUNT {len(shared_gid_mapping_global['gid_map_data'])}\n\n")
            else:
                # Empty mapping (just empty tile)
                f.write("const unsigned char gid_to_tile_shared[1][6] = {\n")
                f.write("    // GID 0 (empty tile)\n")
                f.write("    { 0, 0, 0, 0, 0, 0 }\n")
                f.write("};\n\n")
                f.write("#define GID_TO_TILE_SHARED_COUNT 1\n\n")
            f.write("#endif // GID_TO_TILE_SHARED_H\n")
        print(f"Generated shared GID mapping with {len(shared_gid_mapping_global['gid_map_data']) if len(shared_gid_mapping_global['gid_map_data']) > 0 else 1} unique GIDs")

    # Output compression statistics for all levels
    if level_sizes:
        print(f"\n{'='*120}")
        print(f"LEVEL COMPRESSION STATISTICS SUMMARY (LZSS vs LZSA2)")
        print(f"{'='*120}")
        total_uncompressed = 0
        total_lzss_compressed = 0
        total_lzsa2_compressed = 0
        total_uncompressed_no_collision = 0
        total_lzss_compressed_no_collision = 0
        total_lzsa2_compressed_no_collision = 0
        
        print(f"\n{'Level':<15} {'Uncomp':>12} {'LZSS':>12} {'LZSA2':>12} {'Uncomp':>12} {'LZSS':>12} {'LZSA2':>12}")
        print(f"{'':<15} {'(w/coll)':>12} {'(w/coll)':>12} {'(w/coll)':>12} {'(no coll)':>12} {'(no coll)':>12} {'(no coll)':>12}")
        print(f"{'-'*120}")
        
        for level_data in sorted(level_sizes):
            if len(level_data) == 7:
                level_name, uncompressed, lzss_compressed, lzsa2_compressed, uncompressed_no_collision, lzss_compressed_no_collision, lzsa2_compressed_no_collision = level_data
            elif len(level_data) == 5:
                # Old format with single compression method
                level_name, uncompressed, compressed, uncompressed_no_collision, compressed_no_collision = level_data
                lzss_compressed = compressed
                lzsa2_compressed = compressed
                lzss_compressed_no_collision = compressed_no_collision
                lzsa2_compressed_no_collision = compressed_no_collision
            else:
                # Backward compatibility with very old format
                level_name, uncompressed, compressed = level_data
                uncompressed_no_collision = uncompressed
                lzss_compressed = compressed
                lzsa2_compressed = compressed
                lzss_compressed_no_collision = compressed
                lzsa2_compressed_no_collision = compressed
            
            # Exclude level32 from totals
            if level_name != "level32":
                total_uncompressed += uncompressed
                total_lzss_compressed += lzss_compressed
                total_lzsa2_compressed += lzsa2_compressed
                total_uncompressed_no_collision += uncompressed_no_collision
                total_lzss_compressed_no_collision += lzss_compressed_no_collision
                total_lzsa2_compressed_no_collision += lzsa2_compressed_no_collision
            
            # Still show level32 in the table, but mark it as excluded from totals
            marker = " (excl.)" if level_name == "level32" else ""
            print(f"{level_name:<15}{marker:<8} {uncompressed:>12,} {lzss_compressed:>12,} {lzsa2_compressed:>12,} {uncompressed_no_collision:>12,} {lzss_compressed_no_collision:>12,} {lzsa2_compressed_no_collision:>12,}")
        
        print(f"{'-'*120}")
        print(f"{'TOTAL (excl. level32)':<23} {total_uncompressed:>12,} {total_lzss_compressed:>12,} {total_lzsa2_compressed:>12,} {total_uncompressed_no_collision:>12,} {total_lzss_compressed_no_collision:>12,} {total_lzsa2_compressed_no_collision:>12,}")
        print(f"{'='*120}")
        
        # Calculate savings and ratios
        total_lzss_ratio = 100.0 * total_lzss_compressed / total_uncompressed if total_uncompressed > 0 else 0
        total_lzss_savings = total_uncompressed - total_lzss_compressed
        total_lzsa2_ratio = 100.0 * total_lzsa2_compressed / total_uncompressed if total_uncompressed > 0 else 0
        total_lzsa2_savings = total_uncompressed - total_lzsa2_compressed
        
        total_lzss_ratio_no_collision = 100.0 * total_lzss_compressed_no_collision / total_uncompressed_no_collision if total_uncompressed_no_collision > 0 else 0
        total_lzss_savings_no_collision = total_uncompressed_no_collision - total_lzss_compressed_no_collision
        total_lzsa2_ratio_no_collision = 100.0 * total_lzsa2_compressed_no_collision / total_uncompressed_no_collision if total_uncompressed_no_collision > 0 else 0
        total_lzsa2_savings_no_collision = total_uncompressed_no_collision - total_lzsa2_compressed_no_collision
        
        print(f"\nWITH COLLISION DATA (level32 excluded from totals):")
        print(f"  LZSS: {total_lzss_savings:,} bytes saved ({100.0 * total_lzss_savings / total_uncompressed if total_uncompressed > 0 else 0:.1f}% reduction), ratio: {total_lzss_ratio:.1f}%")
        print(f"  LZSA2: {total_lzsa2_savings:,} bytes saved ({100.0 * total_lzsa2_savings / total_uncompressed if total_uncompressed > 0 else 0:.1f}% reduction), ratio: {total_lzsa2_ratio:.1f}%")
        lzsa2_vs_lzss = total_lzss_compressed - total_lzsa2_compressed
        if lzsa2_vs_lzss > 0:
            print(f"  LZSA2 is {lzsa2_vs_lzss:,} bytes smaller than LZSS ({100.0 * lzsa2_vs_lzss / total_lzss_compressed if total_lzss_compressed > 0 else 0:.1f}% better)")
        elif lzsa2_vs_lzss < 0:
            print(f"  LZSS is {abs(lzsa2_vs_lzss):,} bytes smaller than LZSA2 ({100.0 * abs(lzsa2_vs_lzss) / total_lzsa2_compressed if total_lzsa2_compressed > 0 else 0:.1f}% better)")
        else:
            print(f"  LZSS and LZSA2 are the same size")
        
        print(f"\nWITHOUT COLLISION DATA (derived from tilemap, level32 excluded from totals):")
        print(f"  LZSS: {total_lzss_savings_no_collision:,} bytes saved ({100.0 * total_lzss_savings_no_collision / total_uncompressed_no_collision if total_uncompressed_no_collision > 0 else 0:.1f}% reduction), ratio: {total_lzss_ratio_no_collision:.1f}%")
        print(f"  LZSA2: {total_lzsa2_savings_no_collision:,} bytes saved ({100.0 * total_lzsa2_savings_no_collision / total_uncompressed_no_collision if total_uncompressed_no_collision > 0 else 0:.1f}% reduction), ratio: {total_lzsa2_ratio_no_collision:.1f}%")
        lzsa2_vs_lzss_no_collision = total_lzss_compressed_no_collision - total_lzsa2_compressed_no_collision
        if lzsa2_vs_lzss_no_collision > 0:
            print(f"  LZSA2 is {lzsa2_vs_lzss_no_collision:,} bytes smaller than LZSS ({100.0 * lzsa2_vs_lzss_no_collision / total_lzss_compressed_no_collision if total_lzss_compressed_no_collision > 0 else 0:.1f}% better)")
        elif lzsa2_vs_lzss_no_collision < 0:
            print(f"  LZSS is {abs(lzsa2_vs_lzss_no_collision):,} bytes smaller than LZSA2 ({100.0 * abs(lzsa2_vs_lzss_no_collision) / total_lzsa2_compressed_no_collision if total_lzsa2_compressed_no_collision > 0 else 0:.1f}% better)")
        else:
            print(f"  LZSS and LZSA2 are the same size")
        
        # Calculate savings from dropping collision
        collision_savings_uncompressed = total_uncompressed - total_uncompressed_no_collision
        collision_savings_lzss = total_lzss_compressed - total_lzss_compressed_no_collision
        collision_savings_lzsa2 = total_lzsa2_compressed - total_lzsa2_compressed_no_collision
        
        print(f"\nSAVINGS FROM DROPPING COLLISION ARRAY:")
        print(f"  Uncompressed: {collision_savings_uncompressed:,} bytes ({100.0 * collision_savings_uncompressed / total_uncompressed if total_uncompressed > 0 else 0:.1f}% reduction)")
        print(f"  LZSS compressed: {collision_savings_lzss:,} bytes ({100.0 * collision_savings_lzss / total_lzss_compressed if total_lzss_compressed > 0 else 0:.1f}% reduction)")
        print(f"  LZSA2 compressed: {collision_savings_lzsa2:,} bytes ({100.0 * collision_savings_lzsa2 / total_lzsa2_compressed if total_lzsa2_compressed > 0 else 0:.1f}% reduction)")
        print(f"  Final LZSS compressed size without collision: {total_lzss_compressed_no_collision:,} bytes")
        print(f"  Final LZSA2 compressed size without collision: {total_lzsa2_compressed_no_collision:,} bytes")
        
        # Calculate combined compression (all 31 levels compressed together)
        if level_data_combined:
            print(f"\n{'='*120}")
            print(f"COMBINED COMPRESSION (All 31 levels compressed together as one block)")
            print(f"{'='*120}")
            
            # Combine all level data
            combined_data_with_collision = bytearray()
            combined_data_no_collision = bytearray()
            combined_uncompressed_with_collision = 0
            combined_uncompressed_no_collision = 0
            
            for level_name, const_data, const_data_no_collision, uncompressed, uncompressed_no_collision in level_data_combined:
                combined_data_with_collision.extend(const_data)
                combined_data_no_collision.extend(const_data_no_collision)
                combined_uncompressed_with_collision += uncompressed
                combined_uncompressed_no_collision += uncompressed_no_collision
            
            # Compress combined data with LZSA2
            combined_lzsa2_with_collision = estimate_lzsa2_size(combined_data_with_collision)
            combined_lzsa2_no_collision = estimate_lzsa2_size(combined_data_no_collision)
            
            print(f"\nWITH COLLISION DATA (all 31 levels combined):")
            print(f"  Uncompressed total: {combined_uncompressed_with_collision:,} bytes")
            print(f"  LZSA2 compressed: {combined_lzsa2_with_collision:,} bytes")
            combined_ratio_with = 100.0 * combined_lzsa2_with_collision / combined_uncompressed_with_collision if combined_uncompressed_with_collision > 0 else 0
            combined_savings_with = combined_uncompressed_with_collision - combined_lzsa2_with_collision
            print(f"  Compression ratio: {combined_ratio_with:.1f}%")
            print(f"  Space saved: {combined_savings_with:,} bytes ({100.0 * combined_savings_with / combined_uncompressed_with_collision if combined_uncompressed_with_collision > 0 else 0:.1f}% reduction)")
            
            # Compare with individual compression
            individual_vs_combined_with = total_lzsa2_compressed - combined_lzsa2_with_collision
            if individual_vs_combined_with > 0:
                print(f"  Combined compression saves {individual_vs_combined_with:,} bytes vs individual compression ({100.0 * individual_vs_combined_with / total_lzsa2_compressed if total_lzsa2_compressed > 0 else 0:.1f}% better)")
            elif individual_vs_combined_with < 0:
                print(f"  Individual compression is {abs(individual_vs_combined_with):,} bytes smaller ({100.0 * abs(individual_vs_combined_with) / combined_lzsa2_with_collision if combined_lzsa2_with_collision > 0 else 0:.1f}% better)")
            else:
                print(f"  Combined and individual compression are the same size")
            
            print(f"\nWITHOUT COLLISION DATA (all 31 levels combined, derived from tilemap):")
            print(f"  Uncompressed total: {combined_uncompressed_no_collision:,} bytes")
            print(f"  LZSA2 compressed: {combined_lzsa2_no_collision:,} bytes")
            combined_ratio_no_collision = 100.0 * combined_lzsa2_no_collision / combined_uncompressed_no_collision if combined_uncompressed_no_collision > 0 else 0
            combined_savings_no_collision = combined_uncompressed_no_collision - combined_lzsa2_no_collision
            print(f"  Compression ratio: {combined_ratio_no_collision:.1f}%")
            print(f"  Space saved: {combined_savings_no_collision:,} bytes ({100.0 * combined_savings_no_collision / combined_uncompressed_no_collision if combined_uncompressed_no_collision > 0 else 0:.1f}% reduction)")
            
            # Compare with individual compression
            individual_vs_combined_no_collision = total_lzsa2_compressed_no_collision - combined_lzsa2_no_collision
            if individual_vs_combined_no_collision > 0:
                print(f"  Combined compression saves {individual_vs_combined_no_collision:,} bytes vs individual compression ({100.0 * individual_vs_combined_no_collision / total_lzsa2_compressed_no_collision if total_lzsa2_compressed_no_collision > 0 else 0:.1f}% better)")
            elif individual_vs_combined_no_collision < 0:
                print(f"  Individual compression is {abs(individual_vs_combined_no_collision):,} bytes smaller ({100.0 * abs(individual_vs_combined_no_collision) / combined_lzsa2_no_collision if combined_lzsa2_no_collision > 0 else 0:.1f}% better)")
            else:
                print(f"  Combined and individual compression are the same size")
            
            print(f"{'='*120}")

    print(f"\nConversion complete!")
    print(f"  CHR Bank 0: {output_chr_bank0}")
    print(f"  CHR Bank 1: {output_chr_bank1}")
    print(f"  CHR Combined: {output_chr_combined}")
    if bank0_sprites:
        print(f"  Bank 0 Preview: {output_preview_bank0}")
    if bank1_sprites:
        print(f"  Bank 1 Preview: {output_preview_bank1}")


if __name__ == '__main__':
    main()
