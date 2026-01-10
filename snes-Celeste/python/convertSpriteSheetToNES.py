from PIL import Image, ImageDraw
import os
import json
import base64
import zlib
import gzip
import struct
import re
from collections import Counter


# Shared GID mapping (built incrementally as levels are processed)
shared_gid_mapping_global = {
    'tile_to_gid': {},  # Maps (tl, tr, bl, br, pal_idx, flip_flags) -> GID
    'gid_map_data': [],  # List of tile entries indexed by GID
    'gid_to_original_tile_index': {},  # Maps NES GID -> original tile_index (for collision)
    'next_gid': 1  # Next available GID (0 is reserved for empty tiles)
}

# Global list to collect GID arrays for compression dictionary building
temp_gid_arrays_collector = []

# Shared object sprite dictionary (built from all levels)
# Maps tile_index -> (tile_index, palette_idx, tl, tr, bl, br)
shared_object_sprite_dict = {}

# Track background palette indices for background tile objects (collapse tiles, breakable walls, etc.)
# Maps tile_index -> background_palette_index
background_tile_object_palette = {}

# Store background tile object GID mappings separately
collapse_tile_gid_data = []  # List of 3 tile entries for GIDs 24, 25, 26 (collapse tiles)
breakable_wall_gid_data = []  # List of 1 tile entry for GID 27 (breakable walls)
monument_gid_data = []  # List of 1 tile entry for GID 70 (monuments, composed of sprites 70, 71, 86, 87)
big_chest_gid_data = []  # List of 1 tile entry for GID 96 (big chest, composed of sprites 96, 97)

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
deco_objects_gids = [44, 60,61,62,63, 112, 113]
solid_gids = [32, 33, 34, 35, 36, 37, 38, 39, 48, 49, 50, 51, 52, 53, 54, 55, 66, 67, 68, 69, 82, 83, 84, 85, 98, 99, 100, 101, 114, 115, 116, 117, 72]
pointy_gids = [17, 27, 43, 59]
icy_gids = [66,67,68,69,82,83,84,85,98,99,100,101,114,115,116,117]
all_background_gids = far_background_gids + solid_gids + pointy_gids + icy_gids + deco_objects_gids

arrMustBeObject = [8, 9, 10, 11, 12, 13, 14, 15, 18, 19, 20, 21, 22, 26, 28, 29, 30, 31, 45, 46, 47, 70, 71, 86, 87, 96, 97, 102, 118, 119, 120]
# Background tile objects: rendered as objects but use background palettes and appear in tilemap
# Collapse tiles (23, 24, 25) are removed from arrMustBeObject so they use background palettes
collapse_tile_indices = [23, 24, 25]  # COLLAPSE_TILE_SPRITE_1, COLLAPSE_TILE_SPRITE_2, COLLAPSE_TILE_SPRITE_3
# Breakable walls (64) - can be in tilemap as background tiles
breakable_wall_indices = [64]  # BREAKABLE_WALL_SPRITE_1
# Monuments (70, 71, 86, 87) - can be in tilemap as background tiles
# For monuments, the GID value equals the sprite index
monument_indices = [70, 71, 86, 87]  # MONUMENT_SPRITE_1, MONUMENT_SPRITE_2, MONUMENT_SPRITE_3, MONUMENT_SPRITE_4
# Big chest (96, 97) - can be in tilemap as background tiles
# For big chest, the GID value equals the sprite index (96)
big_chest_indices = [96, 97]  # BIG_CHEST_SPRITE_1, BIG_CHEST_SPRITE_2
# All background tile objects (collapse tiles, breakable walls, monuments, big chest, etc.)
background_tile_object_indices = collapse_tile_indices + breakable_wall_indices + monument_indices + big_chest_indices

# Tile GIDs to manually exclude from CHR banks
# Add tile indices here to prevent them from being included in CHR bank data
exclude_tile_gids_from_chr = [ 73, 74, 75, 76, 77, 78, 79,
                               89, 90, 91, 92, 93, 94, 95,
                               105, 106, 107, 108, 109, 110, 111,
                               121, 122, 123, 124, 125, 126, 127
                               ]

# Sprite groups that should share the same palette
# Each sublist contains sprite indices that must use the same palette
sprite_palette_groups = [
    [18, 19],  # Sprite 18 and 19 share palette
    # Add more groups here as needed, e.g.:
    # [20, 21, 22],  # Multiple sprites sharing one palette
]

def get_collision_for_tile_index(tile_index):
    """Get collision flag for a given original tile_index."""
    if tile_index < 0:
        return 0
    if tile_index in solid_gids or tile_index in icy_gids:
        return 1
    elif tile_index in pointy_gids:
        if tile_index == 17:
            return 4
        elif tile_index == 27:
            return 8
        elif tile_index == 43:
            return 16  # 0x10
        elif tile_index == 59:
            return 32  # 0x20
        else:
            return 0
    else:
        return 0

# Shared compression dictionary (built from all levels)
shared_compression_dict = []  # List of (gid1, gid2) tuples, max 64 entries
best_compression_config = {'rle_threshold': 3, 'dict_size': 64}  # Best configuration found during testing

def build_shared_compression_dictionary(all_level_gid_arrays, dict_size=64):
    """
    Build a shared dictionary of common 2-GID sequences across all levels.
    Returns a list of (gid1, gid2) tuples, max dict_size entries.
    """
    # Count frequency of 2-GID sequences
    sequence_counts = Counter()
    
    for gid_array in all_level_gid_arrays:
        for i in range(len(gid_array) - 1):
            seq = (gid_array[i], gid_array[i + 1])
            sequence_counts[seq] += 1
    
    # Get most common sequences (max dict_size)
    most_common = sequence_counts.most_common(dict_size)
    
    # Build dictionary
    dictionary = []
    for seq, count in most_common:
        dictionary.append(seq)
    
    # Pad to dict_size entries if needed (with (0, 0))
    while len(dictionary) < dict_size:
        dictionary.append((0, 0))
    
    return dictionary[:dict_size]

def compress_level(gid_array, dictionary, rle_threshold=3):
    """
    Compress a level GID array using RLE and dictionary references.
    
    Format:
    - 0x00-0x7F: Literal GID (0-127)
    - 0x80-0xBF: RLE code (bits 0-5 = count-1, next byte = GID to repeat)
    - 0xC0-0xFF: Dictionary sequence (bits 0-5 = dict index 0-63, represents 2 GIDs)
    
    Returns compressed bytearray.
    """
    compressed = bytearray()
    i = 0
    
    while i < len(gid_array):
        # Try to find a dictionary match first (2 GIDs)
        if i < len(gid_array) - 1:
            seq = (gid_array[i], gid_array[i + 1])
            if seq in dictionary:
                dict_idx = dictionary.index(seq)
                # Current format only supports 64 entries (0xC0-0xFF)
                # For larger dictionaries, would need format change
                if dict_idx < 64:
                    compressed.append(0xC0 + dict_idx)  # Dictionary reference
                    i += 2
                    continue
                # If dict_idx >= 64, skip dictionary match and use literal/RLE instead
        
        # Try RLE (repeated single GID)
        gid = gid_array[i]
        count = 1
        while i + count < len(gid_array) and gid_array[i + count] == gid and count < 64:
            count += 1
        
        if count >= rle_threshold:  # RLE is beneficial for rle_threshold+ repeats
            compressed.append(0x80 + (count - 1))  # RLE code
            compressed.append(gid)  # GID to repeat
            i += count
        elif gid < 0x80:  # Literal GID (0-127)
            compressed.append(gid)
            i += 1
        else:  # GID >= 128, must use RLE even for single
            compressed.append(0x80)  # RLE count 1
            compressed.append(gid)
            i += 1
    
    return compressed

def decompress_level(compressed_data, dictionary, expected_size):
    """
    Decompress a level from compressed data.
    Returns a list of GIDs.
    """
    decompressed = []
    i = 0
    
    while i < len(compressed_data) and len(decompressed) < expected_size:
        byte = compressed_data[i]
        
        if byte < 0x80:  # Literal GID
            decompressed.append(byte)
            i += 1
        elif byte < 0xC0:  # RLE code
            count = (byte & 0x3F) + 1  # Extract count (1-64)
            if i + 1 < len(compressed_data):
                gid = compressed_data[i + 1]
                decompressed.extend([gid] * count)
                i += 2
            else:
                break
        else:  # Dictionary reference (0xC0-0xFF)
            dict_idx = byte & 0x3F  # Extract dict index (0-63)
            if dict_idx < len(dictionary):
                seq = dictionary[dict_idx]
                decompressed.append(seq[0])
                decompressed.append(seq[1])
            i += 1
    
    # Pad to expected size if needed
    while len(decompressed) < expected_size:
        decompressed.append(0)
    
    return decompressed[:expected_size]

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


def reserve_collapse_tile_gids(all_sprite_data, tile_mapping):
    """
    Reserve GIDs 24, 25, 26 for collapse tiles (sprite indices 23, 24, 25).
    Extract tile data and store separately for header generation.
    Note: These GIDs are reserved but not added to the main GID array yet.
    They will be added when we generate the shared header, ensuring they're at indices 24, 25, 26.
    """
    global collapse_tile_gid_data
    
    collapse_tile_gid_data = []
    collapse_gids = [24, 25, 26]
    collapse_sprite_indices = [23, 24, 25]  # COLLAPSE_TILE_SPRITE_1, 2, 3
    
    # Extract collapse tile data
    for i, (sprite_idx, target_gid) in enumerate(zip(collapse_sprite_indices, collapse_gids)):
        if sprite_idx < len(all_sprite_data):
            # Get optimized tile indices for this collapse tile
            tl_opt_idx = tile_mapping.get(sprite_idx * 4 + 0, 0)
            tr_opt_idx = tile_mapping.get(sprite_idx * 4 + 1, 0)
            bl_opt_idx = tile_mapping.get(sprite_idx * 4 + 2, 0)
            br_opt_idx = tile_mapping.get(sprite_idx * 4 + 3, 0)
            
            # Collapse tiles use background palette 0 (palette_idx_encoded = 0, bit 2 = 0 for background)
            palette_idx_encoded = 0  # Background palette 0
            flip_flags = 0  # No flip
            
            tile_entry = (tl_opt_idx, tr_opt_idx, bl_opt_idx, br_opt_idx, palette_idx_encoded, flip_flags)
            collapse_tile_gid_data.append(tile_entry)
            
            print(f"Prepared collapse tile sprite {sprite_idx} for GID {target_gid} (tiles: {tl_opt_idx}, {tr_opt_idx}, {bl_opt_idx}, {br_opt_idx})")
        else:
            # Sprite index out of range, use empty tile
            tile_entry = (0, 0, 0, 0, 0, 0)
            collapse_tile_gid_data.append(tile_entry)
            print(f"WARNING: Collapse tile sprite {sprite_idx} not found, using empty tile for GID {target_gid}")
    
    return collapse_tile_gid_data

def reserve_breakable_wall_gids(all_sprite_data, tile_mapping):
    """
    Reserve GID 27 for breakable walls (sprite index 64).
    Extract tile data and store separately for header generation.
    Note: This GID is reserved but not added to the main GID array yet.
    It will be added when we generate the shared header, ensuring it's at index 27.
    Breakable walls are 32x32 sprites (4x4 tiles = 16 tiles total).
    """
    global breakable_wall_gid_data
    
    breakable_wall_gid_data = []
    breakable_wall_gid = 27
    breakable_wall_sprite_index = 64  # BREAKABLE_WALL_SPRITE_1
    
    # Breakable walls are 32x32 (4x4 tiles), so we need to extract all 16 tiles
    # The sprite sheet has 16x16 sprites, so a 32x32 breakable wall spans 4 sprites
    # Arranged as:
    #   [Sprite 64] [Sprite 65]
    #   [Sprite 80] [Sprite 81]
    # Each sprite has 4 tiles: TL, TR, BL, BR
    # For a 4x4 tile grid in row-major order, we need:
    # Row 1: Sprite 64 (TL, TR) + Sprite 65 (TL, TR)
    # Row 2: Sprite 64 (BL, BR) + Sprite 65 (BL, BR)
    # Row 3: Sprite 80 (TL, TR) + Sprite 81 (TL, TR)
    # Row 4: Sprite 80 (BL, BR) + Sprite 81 (BL, BR)
    
    # Extract all 16 tiles for the 4x4 grid
    tiles_4x4 = []
    base_sprite_idx = breakable_wall_sprite_index
    
    # Sprites are: 64 (top-left), 65 (top-right), 80 (bottom-left), 81 (bottom-right)
    sprite_tl = base_sprite_idx      # Top-left 16x16 (64)
    sprite_tr = base_sprite_idx + 1  # Top-right 16x16 (65)
    sprite_bl = 80                   # Bottom-left 16x16 (80)
    sprite_br = 81                   # Bottom-right 16x16 (81)
    
    # Check if we have all 4 sprites (64, 65, 80, 81)
    if sprite_tr < len(all_sprite_data) and sprite_bl < len(all_sprite_data) and sprite_br < len(all_sprite_data):
        # Extract tiles in row-major order for the 4x4 grid
        # Row 1: TL of sprite 64, TR of sprite 64, TL of sprite 65, TR of sprite 65
        tiles_4x4.append(tile_mapping.get(sprite_tl * 4 + 0, 0))  # Sprite 64, TL
        tiles_4x4.append(tile_mapping.get(sprite_tl * 4 + 1, 0))  # Sprite 64, TR
        tiles_4x4.append(tile_mapping.get(sprite_tr * 4 + 0, 0))  # Sprite 65, TL
        tiles_4x4.append(tile_mapping.get(sprite_tr * 4 + 1, 0))  # Sprite 65, TR
        
        # Row 2: BL of sprite 64, BR of sprite 64, BL of sprite 65, BR of sprite 65
        tiles_4x4.append(tile_mapping.get(sprite_tl * 4 + 2, 0))  # Sprite 64, BL
        tiles_4x4.append(tile_mapping.get(sprite_tl * 4 + 3, 0))  # Sprite 64, BR
        tiles_4x4.append(tile_mapping.get(sprite_tr * 4 + 2, 0))  # Sprite 65, BL
        tiles_4x4.append(tile_mapping.get(sprite_tr * 4 + 3, 0))  # Sprite 65, BR
        
        # Row 3: TL of sprite 80, TR of sprite 80, TL of sprite 81, TR of sprite 81
        tiles_4x4.append(tile_mapping.get(sprite_bl * 4 + 0, 0))  # Sprite 80, TL
        tiles_4x4.append(tile_mapping.get(sprite_bl * 4 + 1, 0))  # Sprite 80, TR
        tiles_4x4.append(tile_mapping.get(sprite_br * 4 + 0, 0))  # Sprite 81, TL
        tiles_4x4.append(tile_mapping.get(sprite_br * 4 + 1, 0))  # Sprite 81, TR
        
        # Row 4: BL of sprite 80, BR of sprite 80, BL of sprite 81, BR of sprite 81
        tiles_4x4.append(tile_mapping.get(sprite_bl * 4 + 2, 0))  # Sprite 80, BL
        tiles_4x4.append(tile_mapping.get(sprite_bl * 4 + 3, 0))  # Sprite 80, BR
        tiles_4x4.append(tile_mapping.get(sprite_br * 4 + 2, 0))  # Sprite 81, BL
        tiles_4x4.append(tile_mapping.get(sprite_br * 4 + 3, 0))  # Sprite 81, BR
        
        # Breakable walls use background palette 0 (palette_idx_encoded = 0, bit 2 = 0 for background)
        palette_idx_encoded = 0  # Background palette 0
        flip_flags = 0  # No flip
        
        # Store as tuple: (16 tiles, palette, flip)
        tile_entry = (tuple(tiles_4x4), palette_idx_encoded, flip_flags)
        breakable_wall_gid_data.append(tile_entry)
        
        tiles_str = ", ".join(map(str, tiles_4x4))
        print(f"Prepared breakable wall sprite {breakable_wall_sprite_index} for GID {breakable_wall_gid} (16 tiles: {tiles_str})")
    else:
        # Not enough sprites, use empty tiles
        tiles_4x4 = [0] * 16
        tile_entry = (tuple(tiles_4x4), 0, 0)
        breakable_wall_gid_data.append(tile_entry)
        print(f"WARNING: Breakable wall sprite {breakable_wall_sprite_index} needs 4 sprites but not enough found, using empty tiles for GID {breakable_wall_gid}")
    
    return breakable_wall_gid_data

def reserve_monument_gids(all_sprite_data, tile_mapping):
    """
    Reserve GIDs 70, 71, 86, 87 for monuments.
    Extract tile data and store separately for header generation.
    Note: These GIDs are reserved but not added to the main GID array yet.
    They will be added when we generate the shared header, ensuring they're at indices 70, 71, 86, 87.
    Monuments are 32x32 sprites (4x4 tiles = 16 tiles total).
    For monuments, the GID value matches the sprite index (70, 71, 86, 87).
    Monuments are arranged as:
        [Sprite 70] [Sprite 71]
        [Sprite 86] [Sprite 87]
    Each sprite is 16x16 (4 tiles), together forming a 32x32 monument (16 tiles).
    """
    global monument_gid_data
    
    monument_gid_data = []
    monument_gids = [70, 71, 86, 87]
    # For monuments, GID equals sprite index
    # Sprites are: 70 (top-left), 71 (top-right), 86 (bottom-left), 87 (bottom-right)
    sprite_tl = 70  # Top-left 16x16
    sprite_tr = 71  # Top-right 16x16
    sprite_bl = 86  # Bottom-left 16x16
    sprite_br = 87  # Bottom-right 16x16
    
    # Check if we have all 4 sprites
    if sprite_tr < len(all_sprite_data) and sprite_bl < len(all_sprite_data) and sprite_br < len(all_sprite_data):
        # Extract tiles in row-major order for the 4x4 grid
        tiles_4x4 = []
        
        # Row 1: TL of sprite 70, TR of sprite 70, TL of sprite 71, TR of sprite 71
        tiles_4x4.append(tile_mapping.get(sprite_tl * 4 + 0, 0))  # Sprite 70, TL
        tiles_4x4.append(tile_mapping.get(sprite_tl * 4 + 1, 0))  # Sprite 70, TR
        tiles_4x4.append(tile_mapping.get(sprite_tr * 4 + 0, 0))  # Sprite 71, TL
        tiles_4x4.append(tile_mapping.get(sprite_tr * 4 + 1, 0))  # Sprite 71, TR
        
        # Row 2: BL of sprite 70, BR of sprite 70, BL of sprite 71, BR of sprite 71
        tiles_4x4.append(tile_mapping.get(sprite_tl * 4 + 2, 0))  # Sprite 70, BL
        tiles_4x4.append(tile_mapping.get(sprite_tl * 4 + 3, 0))  # Sprite 70, BR
        tiles_4x4.append(tile_mapping.get(sprite_tr * 4 + 2, 0))  # Sprite 71, BL
        tiles_4x4.append(tile_mapping.get(sprite_tr * 4 + 3, 0))  # Sprite 71, BR
        
        # Row 3: TL of sprite 86, TR of sprite 86, TL of sprite 87, TR of sprite 87
        tiles_4x4.append(tile_mapping.get(sprite_bl * 4 + 0, 0))  # Sprite 86, TL
        tiles_4x4.append(tile_mapping.get(sprite_bl * 4 + 1, 0))  # Sprite 86, TR
        tiles_4x4.append(tile_mapping.get(sprite_br * 4 + 0, 0))  # Sprite 87, TL
        tiles_4x4.append(tile_mapping.get(sprite_br * 4 + 1, 0))  # Sprite 87, TR
        
        # Row 4: BL of sprite 86, BR of sprite 86, BL of sprite 87, BR of sprite 87
        tiles_4x4.append(tile_mapping.get(sprite_bl * 4 + 2, 0))  # Sprite 86, BL
        tiles_4x4.append(tile_mapping.get(sprite_bl * 4 + 3, 0))  # Sprite 86, BR
        tiles_4x4.append(tile_mapping.get(sprite_br * 4 + 2, 0))  # Sprite 87, BL
        tiles_4x4.append(tile_mapping.get(sprite_br * 4 + 3, 0))  # Sprite 87, BR
        
        # Monuments use background palette 0 (palette_idx_encoded = 0, bit 2 = 0 for background)
        palette_idx_encoded = 0  # Background palette 0
        flip_flags = 0  # No flip
        
        # Store as tuple: (16 tiles, palette, flip) - same format as breakable walls
        tile_entry = (tuple(tiles_4x4), palette_idx_encoded, flip_flags)
        monument_gid_data.append(tile_entry)
        
        tiles_str = ", ".join(map(str, tiles_4x4))
        print(f"Prepared monument sprites 70, 71, 86, 87 for GID 70 (16 tiles: {tiles_str})")
    else:
        # Not enough sprites, use empty tiles
        tiles_4x4 = [0] * 16
        tile_entry = (tuple(tiles_4x4), 0, 0)
        monument_gid_data.append(tile_entry)
        print(f"WARNING: Monument sprites 70, 71, 86, 87 not all found, using empty tiles")
    
    return monument_gid_data

def reserve_big_chest_gids(all_sprite_data, tile_mapping):
    """
    Reserve GID 96 for big chest.
    Extract tile data and store separately for header generation.
    Note: This GID is reserved but not added to the main GID array yet.
    It will be added when we generate the shared header, ensuring it's at index 96.
    Big chest is 32x32 sprites (4x4 tiles = 16 tiles total).
    For big chest, the GID value matches the sprite index (96).
    Big chest is arranged as:
        [Sprite 96] [Sprite 97]
        [Sprite 112] [Sprite 113]
    Each sprite is 16x16 (4 tiles), together forming a 32x32 big chest (16 tiles).
    """
    global big_chest_gid_data
    
    big_chest_gid_data = []
    big_chest_gid = 96
    # For big chest, GID equals sprite index
    # Sprites are: 96 (top-left 16x16), 97 (top-right 16x16), 112 (bottom-left 16x16), 113 (bottom-right 16x16)
    sprite_tl = 96  # Top-left 16x16
    sprite_tr = 97  # Top-right 16x16
    sprite_bl = 112  # Bottom-left 16x16
    sprite_br = 113  # Bottom-right 16x16
    
    # Check if we have all 4 sprites
    if sprite_tl < len(all_sprite_data) and sprite_tr < len(all_sprite_data) and sprite_bl < len(all_sprite_data) and sprite_br < len(all_sprite_data):
        # Extract tiles in row-major order for the 4x4 grid
        tiles_4x4 = []
        
        # Row 1: TL of sprite 96, TR of sprite 96, TL of sprite 97, TR of sprite 97
        tiles_4x4.append(tile_mapping.get(sprite_tl * 4 + 0, 0))  # Sprite 96, TL
        tiles_4x4.append(tile_mapping.get(sprite_tl * 4 + 1, 0))  # Sprite 96, TR
        tiles_4x4.append(tile_mapping.get(sprite_tr * 4 + 0, 0))  # Sprite 97, TL
        tiles_4x4.append(tile_mapping.get(sprite_tr * 4 + 1, 0))  # Sprite 97, TR
        
        # Row 2: BL of sprite 96, BR of sprite 96, BL of sprite 97, BR of sprite 97
        tiles_4x4.append(tile_mapping.get(sprite_tl * 4 + 2, 0))  # Sprite 96, BL
        tiles_4x4.append(tile_mapping.get(sprite_tl * 4 + 3, 0))  # Sprite 96, BR
        tiles_4x4.append(tile_mapping.get(sprite_tr * 4 + 2, 0))  # Sprite 97, BL
        tiles_4x4.append(tile_mapping.get(sprite_tr * 4 + 3, 0))  # Sprite 97, BR
        
        # Row 3: TL of sprite 112, TR of sprite 112, TL of sprite 113, TR of sprite 113
        tiles_4x4.append(tile_mapping.get(sprite_bl * 4 + 0, 0))  # Sprite 112, TL
        tiles_4x4.append(tile_mapping.get(sprite_bl * 4 + 1, 0))  # Sprite 112, TR
        tiles_4x4.append(tile_mapping.get(sprite_br * 4 + 0, 0))  # Sprite 113, TL
        tiles_4x4.append(tile_mapping.get(sprite_br * 4 + 1, 0))  # Sprite 113, TR
        
        # Row 4: BL of sprite 112, BR of sprite 112, BL of sprite 113, BR of sprite 113
        tiles_4x4.append(tile_mapping.get(sprite_bl * 4 + 2, 0))  # Sprite 112, BL
        tiles_4x4.append(tile_mapping.get(sprite_bl * 4 + 3, 0))  # Sprite 112, BR
        tiles_4x4.append(tile_mapping.get(sprite_br * 4 + 2, 0))  # Sprite 113, BL
        tiles_4x4.append(tile_mapping.get(sprite_br * 4 + 3, 0))  # Sprite 113, BR
        
        # Big chest uses background palette 0 (palette_idx_encoded = 0, bit 2 = 0 for background)
        palette_idx_encoded = 0  # Background palette 0
        flip_flags = 0  # No flip
        
        # Store as tuple: (16 tiles, palette, flip) - same format as breakable walls and monuments
        tile_entry = (tuple(tiles_4x4), palette_idx_encoded, flip_flags)
        big_chest_gid_data.append(tile_entry)
        
        tiles_str = ", ".join(map(str, tiles_4x4))
        print(f"Prepared big chest sprites 96, 97, 112, 113 for GID 96 (16 tiles: {tiles_str})")
    else:
        # Not enough sprites, use empty tiles
        tiles_4x4 = [0] * 16
        tile_entry = (tuple(tiles_4x4), 0, 0)
        big_chest_gid_data.append(tile_entry)
        print(f"WARNING: Big chest sprites 96, 97, 112, 113 not all found, using empty tiles")
    
    return big_chest_gid_data

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




def generate_nes_tilemap_header(tile_data, layer_name, map_width, map_height, all_sprite_data, tile_mapping, unique_chr_tiles, all_palettes, object_palettes=None, object_palette_mapping=None, shared_gid_mapping=None, use_compression=False):
    global best_compression_config
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
    # Track collapse tiles found in this level's tilemap (they need to be in object sprite dictionary)
    collapse_tiles_in_tilemap = {}  # Maps collapse_tile_index -> background_palette_index

    # Collect unique palettes separately for background and sprite tiles
    # Background palettes: max 4, first color must be black
    background_palettes = []
    background_palette_to_index = {}  # Maps palette tuple to index in background_palettes

    # Sprite palettes: max 4
    # Initialize with object_palettes (3 palettes for indices 0-2, index 3 reserved for player)
    sprite_palettes = []
    sprite_palette_to_index = {}  # Maps palette tuple to index in sprite_palettes
    
    # Initialize sprite palettes with object_palettes if provided
    # Sprite palette 0: object_palettes[0] (for objects like strawberry, flying berry, etc.)
    # Sprite palette 1: will be set to background palette 0 (for collapse tiles)
    # Sprite palette 2: object_palettes[1] (for other objects)
    # Sprite palette 3: reserved for player
    if object_palettes is not None and len(object_palettes) > 0:
        # Set sprite palette 0 to object_palettes[0] (for regular objects)
        normalized_obj_pal_0 = list(object_palettes[0])
        while len(normalized_obj_pal_0) < 4:
            normalized_obj_pal_0.append((0, 0, 0))
        normalized_obj_pal_0 = normalized_obj_pal_0[:4]
        sprite_palettes.append(normalized_obj_pal_0)
        sprite_palette_to_index[tuple(normalized_obj_pal_0)] = 0
        print(f"Initialized sprite palette 0 with object palette 0: {normalized_obj_pal_0}")
        
        # Reserve slot 1 for background palette 0 (for collapse tiles), will be set later
        sprite_palettes.append([(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)])  # Placeholder for index 1
        
        # Set sprite palette 2 to object_palettes[1] (for objects like springs)
        if len(object_palettes) > 1:
            normalized_obj_pal_1 = list(object_palettes[1])
            while len(normalized_obj_pal_1) < 4:
                normalized_obj_pal_1.append((0, 0, 0))
            normalized_obj_pal_1 = normalized_obj_pal_1[:4]
            sprite_palettes.append(normalized_obj_pal_1)
            sprite_palette_to_index[tuple(normalized_obj_pal_1)] = 2
            print(f"Initialized sprite palette 2 with object palette 1: {normalized_obj_pal_1}")
        else:
            sprite_palettes.append([(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)])
    else:
        # No object palettes, initialize with placeholders
        sprite_palettes.append([(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)])  # Palette 0
        sprite_palettes.append([(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)])  # Palette 1 (for collapse tiles)
        sprite_palettes.append([(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)])  # Palette 2
    
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
            tilemap_data.append((0, 0, 0, 0, 0, 0, -1))  # Empty tile entry, tile_index = -1
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
            tilemap_data.append((0, 0, 0, 0, 0, 0, -1))  # Empty tile entry, tile_index = -1
            collision_data.append(0)
            continue

        # Skip tiles in arrMustBeObject - treat as empty black tiles in tilemap
        if tile_index in arrMustBeObject:
            # Add to object data but skip in tilemap (empty black tile)
            object_data.append((tile_index, map_x_tile, map_y_tile))
            tilemap_data.append((0, 0, 0, 0, 0, 0, -1))  # Empty tile entry, tile_index = -1
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

        # Determine if this is a background tile (includes far_background, solid, pointy, icy, and background tile objects)
        # Background tile objects (collapse tiles, breakable walls) use background palettes even though they're rendered as sprites
        is_background = tile_index in all_background_gids or tile_index in background_tile_object_indices
        
        # Background tile objects need to be added to object_data so the game can track them as objects
        # They appear in the tilemap as background tiles but are also objects
        if tile_index in background_tile_object_indices:
            object_data.append((tile_index, map_x_tile, map_y_tile))

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
            
            # Track background palette index for background tile objects (they're rendered as sprites but use background palettes)
            if tile_index in background_tile_object_indices:
                background_tile_object_palette[tile_index] = palette_idx
                if tile_index in collapse_tile_indices:
                    collapse_tiles_in_tilemap[tile_index] = palette_idx
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
                    # Find an available slot (2 only - skip 0 which is object_palettes[0], skip 1 which is background palette for collapse tiles, skip 3 which is reserved for player)
                    # Slot 0 is object_palettes[0], slot 1 is background palette 0 (for collapse tiles), slot 2 is object_palettes[1]
                    if 2 < len(sprite_palettes):
                        # Check if slot 2 is empty (all black) or can be reused
                        if sprite_palettes[2] == [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)]:
                            sprite_palettes[2] = normalized_palette
                            sprite_palette_to_index[palette_key] = 2
                            palette_idx = 2
                        else:
                            # Slot 2 is taken, use slot 2 anyway (will overwrite)
                            palette_idx = 2
                            sprite_palettes[2] = normalized_palette
                            sprite_palette_to_index[palette_key] = 2
                    else:
                        # Slot 2 doesn't exist, use slot 2 (will be created)
                        palette_idx = 2
                        while len(sprite_palettes) <= 2:
                            sprite_palettes.append([(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)])
                        sprite_palettes[2] = normalized_palette
                        sprite_palette_to_index[palette_key] = 2
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
        # Also store original tile_index for collision mapping
        tilemap_data.append((tl_opt_idx, tr_opt_idx, bl_opt_idx, br_opt_idx, palette_idx_encoded, flip_flags, tile_index))

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

    # Set sprite palette 1 to match background palette 0 (for collapse tiles)
    # Collapse tiles use background palettes but are rendered as sprites using sprite palette 1
    # This ensures collapse tiles render with the correct colors
    # Note: Sprite palette 0 is used by regular objects (strawberry, flying berry, etc.) and uses object_palettes[0]
    if len(background_palettes) > 0:
        # Ensure sprite_palettes has at least 4 entries
        while len(sprite_palettes) < 4:
            sprite_palettes.append([(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)])
        
        # Set sprite palette 1 to background palette 0 (collapse tiles use sprite palette 1)
        sprite_palettes[1] = list(background_palettes[0])
        print(f"Set sprite palette 1 to background palette 0 for collapse tiles: {background_palettes[0]}")
    
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
    # Track original tile_index for collision mapping
    gid_array = []
    collapse_sprite_indices = [23, 24, 25]  # COLLAPSE_TILE_SPRITE_1, 2, 3
    collapse_gids = [24, 25, 26]
    
    for i, tile_entry_with_index in enumerate(tilemap_data):
        # Extract tile entry (first 6 elements) and original tile_index (last element)
        tile_entry = tile_entry_with_index[:6]
        original_tile_index = tile_entry_with_index[6] if len(tile_entry_with_index) > 6 else -1
        
        # Check if this is a collapse tile - if so, map directly to reserved GIDs 24, 25, 26
        if original_tile_index in collapse_sprite_indices:
            collapse_idx = collapse_sprite_indices.index(original_tile_index)
            gid = collapse_gids[collapse_idx]
            # Create a normalized tile entry with palette 0 (collapse tiles always use background palette 0 in GID mapping)
            tl, tr, bl, br, _, flip = tile_entry
            normalized_tile_entry = (tl, tr, bl, br, 0, flip)  # Force palette to 0
            # Ensure this entry is in the mapping
            if normalized_tile_entry not in tile_to_gid:
                # Reserve the GID slot if not already reserved
                while len(gid_map_data) <= gid:
                    gid_map_data.append((0, 0, 0, 0, 0, 0))
                gid_map_data[gid] = normalized_tile_entry
                tile_to_gid[normalized_tile_entry] = gid
                if original_tile_index >= 0:
                    shared_gid_mapping_global['gid_to_original_tile_index'][gid] = original_tile_index
        elif tile_entry in tile_to_gid:
            # Use existing GID from shared mapping
            gid = tile_to_gid[tile_entry]
            # Update original tile_index if this is the first time we see a valid one for this GID
            if original_tile_index >= 0 and gid not in shared_gid_mapping_global['gid_to_original_tile_index']:
                shared_gid_mapping_global['gid_to_original_tile_index'][gid] = original_tile_index
        else:
            # Add new entry to shared mapping
            # Skip reserved GIDs for background tile objects (collapse tiles, breakable walls, monuments)
            monument_gids_list = [70, 71, 86, 87]
            big_chest_gids_list = [96, 97]
            while next_gid in collapse_gids or next_gid == 27 or next_gid in monument_gids_list or next_gid in big_chest_gids_list:
                next_gid += 1
                shared_gid_mapping_global['next_gid'] = next_gid
            
            if next_gid >= 256:
                # Out of GIDs, reuse GID 0 (empty tile) as fallback
                print(f"WARNING: Exceeded 256 GIDs limit. Reusing empty tile GID for layer '{layer_name}'.")
                gid = 0
            else:
                gid = next_gid
                tile_to_gid[tile_entry] = gid
                # Ensure gid_map_data is large enough
                while len(gid_map_data) <= gid:
                    gid_map_data.append((0, 0, 0, 0, 0, 0))
                gid_map_data[gid] = tile_entry
                # Track original tile_index for this GID
                if original_tile_index >= 0:
                    shared_gid_mapping_global['gid_to_original_tile_index'][gid] = original_tile_index
                next_gid += 1
                shared_gid_mapping_global['next_gid'] = next_gid
        gid_array.append(gid)
    
    print(f"Using shared GID mapping ({len(gid_map_data)} unique GIDs total) for layer '{layer_name}'")
    
    # Collect GID array for compression dictionary building (if not using compression yet)
    global temp_gid_arrays_collector
    if not use_compression:
        temp_gid_arrays_collector.append(gid_array[:])  # Store a copy
    
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
        f.write(f"#include \"gid_to_tile_shared.h\"\n")
        f.write(f"// Using shared compression dictionary (see compression_dict_shared.h)\n")
        f.write(f"#include \"compression_dict_shared.h\"\n\n")

        # Compress tilemap data using best configuration
        global best_compression_config
        compressed_data = compress_level(gid_array, shared_compression_dict, best_compression_config['rle_threshold'])
        
        # Determine PRG-ROM bank number based on level number (for UNROM-512 banking)
        # All levels go to bank 1
        level_num_match = re.search(r'(\d+)', safe_layer_name)
        bank_num = 0  # Default (no banking)
        if level_num_match:
            level_num = int(level_num_match.group(1))
            # All levels go to bank 1
            bank_num = 1
        
        # Section attribute for UNROM-512 banking
        section_attr = f'__attribute__((section(".prg_rom_{bank_num}")))' if bank_num > 0 else ''
        
        # Tilemap data - compressed array
        f.write(f"// Compressed tilemap data for layer '{layer_name}'\n")
        f.write(f"// Format: 0x00-0x7F = literal GID, 0x80-0xBF = RLE, 0xC0-0xFF = dict sequence\n")
        if section_attr:
            f.write(f"{section_attr} const unsigned char tilemap_{safe_layer_name}_compressed[] = {{\n")
        else:
            f.write(f"const unsigned char tilemap_{safe_layer_name}_compressed[] = {{\n")
        # Write 16 values per line
        for i in range(len(compressed_data)):
            if i % 16 == 0:
                f.write("    ")
            f.write(f"0x{compressed_data[i]:02x}")
            if i < len(compressed_data) - 1:
                f.write(",")
                if (i + 1) % 16 == 0:
                    f.write("\n")
                else:
                    f.write(" ")
        f.write("\n};\n\n")
        f.write(f"#define TILEMAP_{safe_layer_name.upper()}_COMPRESSED_SIZE {len(compressed_data)}\n")
        f.write(f"#define TILEMAP_{safe_layer_name.upper()}_COUNT {len(gid_array)}\n\n")

        # Background palette data - NES 6-bit format (4 palettes, 4 colors per palette)
        # Background palettes must have black as color 0
        f.write(f"// Background palette data for layer '{layer_name}' (NES 6-bit format)\n")
        f.write(f"// 4 background palettes, each with 4 colors: [color0=black, color1, color2, color3]\n")
        f.write(f"// Used by tiles in all_background_gids (far_background, solid, pointy, and icy)\n")
        if section_attr:
            f.write(f"{section_attr} const unsigned char palette_background_{safe_layer_name}[4][4] = {{\n")
        else:
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
        if section_attr:
            f.write(f"{section_attr} const unsigned char palette_sprite_{safe_layer_name}[4][4] = {{\n")
        else:
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

        # Collision data is now generated from tilemap GIDs at runtime, so it's omitted here

        # Object data
        f.write(f"// Object data for layer '{layer_name}'\n")
        if object_data:
            if section_attr:
                f.write(f"{section_attr} const unsigned char object_{safe_layer_name}[] = {{\n")
            else:
                f.write(f"const unsigned char object_{safe_layer_name}[] = {{\n")
            for obj_tile, obj_x, obj_y in object_data:
                f.write(f"    {obj_tile}, {obj_x}, {obj_y},\n")
            f.write("};\n\n")
            f.write(f"#define OBJECT_{safe_layer_name.upper()}_COUNT {len(object_data)}\n\n")
        else:
            if section_attr:
                f.write(f"{section_attr} const unsigned char object_{safe_layer_name}[] = {{}};\n\n")
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
            
            # Add key objects (gid 9 and 10) to object sprite data
            # These must always be included even if not present in this level's object_data
            key_object_tile_indices = [9, 10]  # Key object tile indices
            for key_tile_idx in key_object_tile_indices:
                if key_tile_idx < len(all_sprite_data):
                    unique_object_tiles.add(key_tile_idx)
            
            # Add balloon string objects (tiles 13, 14, 15) to object sprite data
            # These must always be included even if not present in this level's object_data
            # Similar to how key sprites are handled - these are extra animation frames
            balloon_string_tile_indices = [13, 14, 15]  # Balloon string tile indices
            for balloon_string_tile_idx in balloon_string_tile_indices:
                if balloon_string_tile_idx < len(all_sprite_data):
                    unique_object_tiles.add(balloon_string_tile_idx)
            
            # Add moving platform objects (tiles 11 and 12) to object sprite data
            # These must always be included even if not present in this level's object_data
            # PLATMOV_SPRITE_1 = 11 (left side), PLATMOV_SPRITE_2 = 12 (right side)
            platmov_tile_indices = [11, 12]  # Moving platform tile indices
            for platmov_tile_idx in platmov_tile_indices:
                if platmov_tile_idx < len(all_sprite_data):
                    unique_object_tiles.add(platmov_tile_idx)
            
            # Add flying berry wing objects (tiles 29, 30, 31) to object sprite data
            # These must always be included even if not present in this level's object_data
            # Similar to how key sprites are handled - these are extra animation frames for wings
            flying_berry_wing_tile_indices = [29, 30, 31]  # Flying berry wing tile indices (UP, MID, DOWN)
            for wing_tile_idx in flying_berry_wing_tile_indices:
                if wing_tile_idx < len(all_sprite_data):
                    unique_object_tiles.add(wing_tile_idx)
            
            # Add flying berry wing objects (tiles 45, 46, 47) to object sprite data
            # These must always be included even if not present in this level's object_data
            # Similar to how key sprites are handled - these are extra animation frames for wings
            flying_berry_wing_tile_indices_2 = [45, 46, 47]  # Flying berry wing tile indices (alternative set)
            for wing_tile_idx in flying_berry_wing_tile_indices_2:
                if wing_tile_idx < len(all_sprite_data):
                    unique_object_tiles.add(wing_tile_idx)
            
            # Add background tile objects found in this level's tilemap (they use background palettes)
            # Add collapse tiles
            for collapse_tile_idx, bg_palette_idx in collapse_tiles_in_tilemap.items():
                if collapse_tile_idx < len(all_sprite_data):
                    unique_object_tiles.add(collapse_tile_idx)
            # Add breakable walls (if present in tilemap)
            for tile_idx in background_tile_object_indices:
                if tile_idx in background_tile_object_palette and tile_idx < len(all_sprite_data):
                    unique_object_tiles.add(tile_idx)
            
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
                    # Collapse tiles (23, 24, 25) use background palette indices (0-3)
                    # Other objects use object palettes (0-2)
                    player_sprite_tile_indices = [1, 2, 3, 4, 5, 6, 7]
                    if obj_tile_idx in player_sprite_tile_indices:
                        # Player frames use sprite palette 3 (mark with 3)
                        palette_idx = 3
                    elif obj_tile_idx in background_tile_object_indices:
                        # Background tile objects (collapse tiles, breakable walls) use sprite palette 1 (which is set to background palette 0)
                        # All background tile objects use sprite palette 1 regardless of which background palette they use
                        palette_idx = 1
                    else:
                        # Get palette index for this object from object_palette_mapping (0-2 for object palettes)
                        # Map to sprite palette indices:
                        # object_palettes[0] -> sprite palette 0
                        # object_palettes[1] -> sprite palette 2 (springs)
                        # object_palettes[2] is not used (we only use 2 object palettes)
                        obj_palette_idx = object_palette_mapping.get(obj_tile_idx, 0)
                        if obj_palette_idx >= len(object_palettes):
                            obj_palette_idx = 0
                        
                        # Map object palette index to sprite palette index
                        if obj_palette_idx == 0:
                            palette_idx = 0  # object_palettes[0] -> sprite palette 0
                        elif obj_palette_idx == 1:
                            palette_idx = 2  # object_palettes[1] -> sprite palette 2 (springs)
                        else:
                            palette_idx = 0  # Default to sprite palette 0
                    
                    object_sprite_mapping[obj_tile_idx] = len(object_sprite_data)
                    object_sprite_data.append((obj_tile_idx, palette_idx, tl_opt_idx, tr_opt_idx, bl_opt_idx, br_opt_idx))
                    
                    # Collect into shared object sprite dictionary (for shared dictionary generation)
                    # Only add if not already present (first occurrence wins, or we could merge/validate)
                    if obj_tile_idx not in shared_object_sprite_dict:
                        shared_object_sprite_dict[obj_tile_idx] = (obj_tile_idx, palette_idx, tl_opt_idx, tr_opt_idx, bl_opt_idx, br_opt_idx)
            
            object_sprite_data_count = len(object_sprite_data)
            
            # Write object sprite data array
            if section_attr:
                f.write(f"{section_attr} const unsigned char object_sprite_{safe_layer_name}[] = {{\n")
            else:
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
            if section_attr:
                f.write(f"{section_attr} const unsigned char palette_object_{safe_layer_name}[3][4] = {{\n")
            else:
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
            if section_attr:
                f.write(f"{section_attr} const unsigned char palette_object_{safe_layer_name}[3][4] = {{\n")
            else:
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


def find_best_object_palette_for_sprite_group(tile_images, object_palettes):
    """
    Find the best object palette for a group of sprites by minimizing total color distance across all sprites.
    This ensures all sprites in the group use the same palette.
    Returns (palette_index, remapped_palette) where remapped_palette is the best palette for the group.
    """
    best_palette_idx = 0
    min_total_distance = float('inf')

    for pal_idx, palette in enumerate(object_palettes):
        total_distance = 0
        
        # Sum distances across all tiles in the group
        for tile_image in tile_images:
            tile_rgb = tile_image.convert('RGB')
            pixels = list(tile_rgb.getdata())
            
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

    # First pass: Determine shared palettes for sprite groups
    sprite_group_palette_map = {}  # Maps tile_index -> (palette_index, palette) for groups
    
    print("\nDetermining shared palettes for sprite groups...")
    for group in sprite_palette_groups:
        if not group:
            continue
        
        # Collect tile images for all sprites in this group
        group_tile_images = []
        valid_indices = []
        for tile_idx in group:
            if tile_idx < tiles_per_row * num_rows:
                y = tile_idx // tiles_per_row
                x = tile_idx % tiles_per_row
                box = (x * 8, y * 8, (x + 1) * 8, (y + 1) * 8)
                tile_8x8 = img.crop(box)
                group_tile_images.append(tile_8x8)
                valid_indices.append(tile_idx)
        
        if group_tile_images and all(idx in arrMustBeObject for idx in valid_indices):
            # Find best palette for the entire group
            best_palette_idx, palette = find_best_object_palette_for_sprite_group(group_tile_images, object_palettes)
            
            # Assign this palette to all sprites in the group
            for tile_idx in valid_indices:
                sprite_group_palette_map[tile_idx] = (best_palette_idx, palette)
            print(f"  Sprite group {group}: assigned object palette {best_palette_idx}")
    
    # Second pass: Process all tiles
    # Track mapping from tile_index to position in all_chr_tiles (accounts for excluded tiles)
    tile_index_to_chr_position = {}  # Maps tile_index -> starting position in all_chr_tiles
    
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
            elif tile_index in sprite_group_palette_map:
                # Use shared palette from sprite group
                best_palette_idx, palette = sprite_group_palette_map[tile_index]
                object_palette_mapping[tile_index] = best_palette_idx
            elif tile_index in arrMustBeObject:
                # Use optimized object palettes (individual assignment)
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

            # Exclude manually specified tile GIDs from CHR banks
            if tile_index not in exclude_tile_gids_from_chr:
                # Record the starting position in all_chr_tiles for this tile_index
                tile_index_to_chr_position[tile_index] = len(all_chr_tiles)
                # Add all 4 tiles to the flat CHR list
                all_chr_tiles.extend(sprite_4tiles)
            else:
                print(f"Excluded tile GID {tile_index} from CHR banks")

    print(f"\nProcessed {len(all_sprite_data)} original 8x8 tiles")
    print(f"Generated {len(all_chr_tiles)} 8x8 tiles for CHR output (4 tiles per sprite)")

    # Deduplicate tiles to optimize CHR usage
    print("\nDeduplicating tiles...")
    unique_chr_tiles, chr_position_mapping = deduplicate_tiles(all_chr_tiles)
    print(f"  Original tiles: {len(all_chr_tiles)}")
    print(f"  Unique tiles: {len(unique_chr_tiles)}")
    print(f"  Duplicates removed: {len(all_chr_tiles) - len(unique_chr_tiles)}")
    print(f"  Space saved: {((len(all_chr_tiles) - len(unique_chr_tiles)) * 16)} bytes")
    
    # Remap tile_mapping to account for excluded tiles
    # chr_position_mapping maps: position_in_all_chr_tiles -> optimized_index
    # tile_index_to_chr_position maps: tile_index -> starting_position_in_all_chr_tiles
    # We need: tile_mapping maps: tile_index * 4 + tile_offset -> optimized_index
    tile_mapping = {}
    for tile_index in range(len(all_sprite_data)):
        if tile_index in tile_index_to_chr_position:
            # This tile was included in CHR banks
            chr_start_pos = tile_index_to_chr_position[tile_index]
            # Map the 4 tiles (tl, tr, bl, br) to optimized indices
            for tile_offset in range(4):
                chr_pos = chr_start_pos + tile_offset
                if chr_pos in chr_position_mapping:
                    tile_mapping[tile_index * 4 + tile_offset] = chr_position_mapping[chr_pos]
                else:
                    # Fallback to 0 if mapping not found
                    tile_mapping[tile_index * 4 + tile_offset] = 0
        else:
            # This tile was excluded from CHR banks - use 0 as fallback
            for tile_offset in range(4):
                tile_mapping[tile_index * 4 + tile_offset] = 0

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

    # Reserve GIDs for background tile objects before processing levels
    print("\nReserving GIDs 24, 25, 26 for collapse tiles...")
    reserve_collapse_tile_gids(all_sprite_data, tile_mapping)
    print("\nReserving GID 27 for breakable walls...")
    reserve_breakable_wall_gids(all_sprite_data, tile_mapping)
    print("\nReserving GIDs 70, 71, 86, 87 for monuments...")
    reserve_monument_gids(all_sprite_data, tile_mapping)
    print("\nReserving GID 96 for big chest...")
    reserve_big_chest_gids(all_sprite_data, tile_mapping)

    # Process map JSON and create level previews and .h files
    tilemap_filename = os.path.join(script_dir, 'baseCelesteTileMap.json')
    if os.path.exists(tilemap_filename):
        print("\nProcessing map JSON file...")
        # Declare globals at the start of the function
        global shared_compression_dict, temp_gid_arrays_collector
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
                    temp_gid_arrays_collector = []  # Store GID arrays for compression dictionary
                    
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
                    
                    # First pass: generate headers to build GID arrays (for compression dictionary)
                    print("\nFirst pass: Generating headers to collect GID arrays...")
                    for mapNum in range(xMaps * yMaps):
                        level_name = f"level{mapNum+1}"
                        flat_submap_data = all_levels_data[mapNum][1]

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
                        # This will build the GID array internally
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
                            object_palette_mapping,
                            use_compression=False
                        )
                    
                    # Collect GID arrays from generated headers (they're stored in shared_gid_mapping_global during generation)
                    # Actually, we need to collect them differently - let's regenerate headers to collect GID arrays
                    # Better approach: modify generate_nes_tilemap_header to return GID array
                    # For now, let's do a second pass where we regenerate with compression
                    # But first, we need the GID arrays - let's collect them by reading the generated files or
                    # by calling a helper function
                    
                    # Build compression dictionary from all levels
                    # We need to collect GID arrays first - let's do a temporary pass
                    print("\nCollecting GID arrays for compression dictionary...")
                    temp_gid_arrays = []
                    for mapNum in range(xMaps * yMaps):
                        level_name = f"level{mapNum+1}"
                        flat_submap_data = all_levels_data[mapNum][1]
                        # Temporarily generate to get GID array - but this will overwrite files
                        # Better: extract GID generation logic
                        # For now, let's just build dict from a sample and regenerate
                    
                    # Actually, let's modify the approach: collect during first generation
                    # We'll need to modify generate_nes_tilemap_header to return the GID array
                    # For now, let's build a simple dictionary and regenerate headers
                    
                    # Test different compression configurations
                    print("\n" + "="*70)
                    print("TESTING COMPRESSION CONFIGURATIONS")
                    print(f"Testing {len(temp_gid_arrays_collector)} levels")
                    print("="*70)
                    
                    # Declare global at the start of this block
                    global best_compression_config
                    
                    if len(temp_gid_arrays_collector) > 0:
                        # Test different RLE thresholds and dictionary sizes
                        # Note: Dictionary sizes > 64 require format changes (currently 0xC0-0xFF = 64 entries max)
                        rle_thresholds = list(range(2, 6))  # RLE thresholds from 2 to 5
                        dict_sizes = list(range(64, 513, 32))  # Dictionary sizes: 64, 96, 128, ..., 512 (increments of 32)
                        
                        results = []
                        
                        for rle_thresh in rle_thresholds:
                            for dict_size in dict_sizes:
                                # Build dictionary with this size
                                test_dict = build_shared_compression_dictionary(temp_gid_arrays_collector, dict_size)
                                
                                # Compress all levels with these parameters
                                total_compressed_size = 0
                                for gid_array in temp_gid_arrays_collector:
                                    compressed = compress_level(gid_array, test_dict, rle_thresh)
                                    total_compressed_size += len(compressed)
                                
                                # Dictionary size (2 bytes per entry)
                                dict_size_bytes = dict_size * 2
                                
                                # Total size
                                total_size = total_compressed_size + dict_size_bytes
                                
                                results.append({
                                    'rle_threshold': rle_thresh,
                                    'dict_size': dict_size,
                                    'compressed_size': total_compressed_size,
                                    'dict_size_bytes': dict_size_bytes,
                                    'total_size': total_size
                                })
                        
                        # Sort by total size
                        results.sort(key=lambda x: x['total_size'])
                        
                        print("\nResults (sorted by total size):")
                        print(f"{'RLE':<6} {'Dict':<6} {'Compressed':<12} {'Dict Bytes':<12} {'Total':<12} {'Note':<20}")
                        print("-" * 80)
                        for r in results:
                            note = ""
                            if r['dict_size'] > 64:
                                note = "Format change needed"
                            print(f"{r['rle_threshold']:<6} {r['dict_size']:<6} {r['compressed_size']:<12} {r['dict_size_bytes']:<12} {r['total_size']:<12} {note:<20}")
                        
                        # Show best configuration (preferring ones that don't need format changes)
                        # First, try to find best that fits current format (dict_size <= 64)
                        best_no_format_change = None
                        for r in results:
                            if r['dict_size'] <= 64:
                                best_no_format_change = r
                                break
                        
                        if best_no_format_change:
                            best = best_no_format_change
                            print(f"\nBest configuration (no format change needed):")
                        else:
                            best = results[0]
                            print(f"\nBest configuration (requires format change):")
                        
                        print(f"  RLE threshold: {best['rle_threshold']}")
                        print(f"  Dictionary size: {best['dict_size']} entries ({best['dict_size_bytes']} bytes)")
                        if best['dict_size'] > 64:
                            print(f"  WARNING: Dictionary size > 64 requires format change to encoding scheme")
                        print(f"  Total compressed tilemap size: {best['compressed_size']} bytes")
                        print(f"  Total size (compressed + dictionary): {best['total_size']} bytes")
                        num_levels = len(temp_gid_arrays_collector)
                        uncompressed_size = num_levels * 256
                        print(f"  Uncompressed size ({num_levels} levels * 256 bytes): {uncompressed_size} bytes")
                        print(f"  Compression ratio: {best['total_size'] / uncompressed_size * 100:.1f}%")
                        print(f"  Space saved: {uncompressed_size - best['total_size']} bytes ({((uncompressed_size - best['total_size']) / uncompressed_size * 100):.1f}%)")
                        
                        # Use best configuration (but limit to 64 if it requires format change and user hasn't implemented it)
                        use_dict_size = best['dict_size']
                        if use_dict_size > 64:
                            print(f"\nWARNING: Best configuration uses dict_size={use_dict_size} which requires format changes.")
                            print(f"Using dict_size=64 instead. To use larger dictionaries, update the encoding format.")
                            use_dict_size = 64
                        
                        best_compression_config = {'rle_threshold': best['rle_threshold'], 'dict_size': use_dict_size}
                        shared_compression_dict = build_shared_compression_dictionary(temp_gid_arrays_collector, use_dict_size)
                        
                        print(f"\n{'='*70}")
                        print("FINAL CONFIGURATION BEING USED:")
                        print(f"{'='*70}")
                        print(f"  RLE threshold: {best_compression_config['rle_threshold']}")
                        print(f"  Dictionary size: {best_compression_config['dict_size']} entries ({best_compression_config['dict_size'] * 2} bytes)")
                        if use_dict_size != best['dict_size']:
                            print(f"  (Clamped from ideal dict_size={best['dict_size']} to fit current format)")
                        print(f"{'='*70}")
                    else:
                        # Fallback: empty dictionary
                        shared_compression_dict = [(0, 0)] * 64
                        best_compression_config = {'rle_threshold': 3, 'dict_size': 64}  # Use defaults
                        print("\nNo GID arrays collected, using default dictionary")
                    
                    # Generate compression dictionary header
                    compression_dict_header_filename = os.path.join(script_dir, 'compression_dict_shared.h')
                    print(f"\nGenerating compression dictionary header: {compression_dict_header_filename}")
                    with open(compression_dict_header_filename, 'w') as f:
                        f.write("// Shared compression dictionary for all levels\n")
                        f.write("// Generated from baseCelesteTileMap.json\n")
                        f.write("// Each entry represents a 2-GID sequence\n\n")
                        f.write("#ifndef COMPRESSION_DICT_SHARED_H\n")
                        f.write("#define COMPRESSION_DICT_SHARED_H\n\n")
                        dict_size = best_compression_config['dict_size']
                        f.write("#ifdef __NES_UNROM_512__\n")
                        f.write(f"__attribute__((section(\".prg_rom_5\"))) const unsigned char compression_dict_shared[{dict_size}][2] = {{\n")
                        f.write("#else\n")
                        f.write(f"const unsigned char compression_dict_shared[{dict_size}][2] = {{\n")
                        f.write("#endif\n")
                        for i, (gid1, gid2) in enumerate(shared_compression_dict):
                            f.write(f"    // Entry {i}\n")
                            f.write(f"    {{ {gid1}, {gid2} }}")
                            if i < len(shared_compression_dict) - 1:
                                f.write(",")
                            f.write("\n")
                        f.write("};\n\n")
                        f.write(f"#define COMPRESSION_DICT_SHARED_COUNT {dict_size}\n\n")
                        f.write("#endif // COMPRESSION_DICT_SHARED_H\n")
                    print(f"Generated compression dictionary with {len(shared_compression_dict)} entries")
                    
                    # Regenerate headers with compression enabled
                    print("\nSecond pass: Regenerating headers with compression...")
                    # Clear the collector for the second pass
                    temp_gid_arrays_collector = []
                    for mapNum in range(xMaps * yMaps):
                        level_name = f"level{mapNum+1}"
                        flat_submap_data = all_levels_data[mapNum][1]
                        
                        # Regenerate header with compression
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
                            object_palette_mapping,
                            use_compression=True
                        )
                    
                    # Generate shared GID mapping header after all levels are processed
                    # Ensure it's always generated, even if empty (for compilation)
                    # Ensure background tile objects are at their reserved GIDs
                    global collapse_tile_gid_data, breakable_wall_gid_data, monument_gid_data
                    collapse_gids = [24, 25, 26]
                    breakable_wall_gid = 27
                    monument_gids = [70, 71, 86, 87]
                    if collapse_tile_gid_data and len(collapse_tile_gid_data) == 3:
                        # Ensure gid_map_data is large enough
                        while len(shared_gid_mapping_global['gid_map_data']) <= collapse_gids[-1]:
                            shared_gid_mapping_global['gid_map_data'].append((0, 0, 0, 0, 0, 0))
                        # Insert collapse tile data at GIDs 24, 25, 26
                        for i, (target_gid, tile_entry) in enumerate(zip(collapse_gids, collapse_tile_gid_data)):
                            shared_gid_mapping_global['gid_map_data'][target_gid] = tile_entry
                            shared_gid_mapping_global['tile_to_gid'][tile_entry] = target_gid
                            # Track original tile_index
                            collapse_sprite_idx = [23, 24, 25][i]
                            shared_gid_mapping_global['gid_to_original_tile_index'][target_gid] = collapse_sprite_idx
                    
                    shared_gid_header_filename = os.path.join(script_dir, 'gid_to_tile_shared.h')
                    print(f"\nGenerating shared GID mapping header: {shared_gid_header_filename}")
                    with open(shared_gid_header_filename, 'w') as f:
                        f.write("// Shared GID to tile mapping for all levels\n")
                        f.write("// Generated from baseCelesteTileMap.json\n")
                        f.write("// Each entry: TL_tile, TR_tile, BL_tile, BR_tile, palette_idx, flip_flags\n")
                        f.write("// flip_flags: bit 0=H, bit 1=V, bit 2=D\n")
                        f.write("// GID 0 is reserved for empty tiles\n")
                        f.write("// GIDs 24, 25, 26 are reserved for collapse tiles (see gid_to_tile_collapse.h)\n")
                        f.write("// GID 27 is reserved for breakable walls (see gid_to_tile_breakable_wall.h)\n")
                        f.write("// GIDs 70, 71, 86, 87 are reserved for monuments (see gid_to_tile_monument.h)\n\n")
                        f.write("#ifndef GID_TO_TILE_SHARED_H\n")
                        f.write("#define GID_TO_TILE_SHARED_H\n\n")
                        if len(shared_gid_mapping_global['gid_map_data']) > 0:
                            f.write("#ifdef __NES_UNROM_512__\n")
                            f.write(f"__attribute__((section(\".prg_rom_5\"))) const unsigned char gid_to_tile_shared[{len(shared_gid_mapping_global['gid_map_data'])}][6] = {{\n")
                            f.write("#else\n")
                            f.write(f"const unsigned char gid_to_tile_shared[{len(shared_gid_mapping_global['gid_map_data'])}][6] = {{\n")
                            f.write("#endif\n")
                            for gid in range(len(shared_gid_mapping_global['gid_map_data'])):
                                tl, tr, bl, br, pal_idx, flip = shared_gid_mapping_global['gid_map_data'][gid]
                                f.write(f"    // GID {gid}\n")
                                f.write(f"    {{ {tl}, {tr}, {bl}, {br}, {pal_idx}, {flip} }}")
                                if gid < len(shared_gid_mapping_global['gid_map_data']) - 1:
                                    f.write(",")
                                f.write("\n")
                            f.write("};\n\n")
                            f.write(f"#define GID_TO_TILE_SHARED_COUNT {len(shared_gid_mapping_global['gid_map_data'])}\n\n")
                            
                            # Generate GID to collision mapping
                            f.write("// GID to collision flags mapping\n")
                            f.write("// Collision flags: 0 = no collision, 1 = solid, 4/8/16/32 = pointy variants\n")
                            f.write("#ifdef __NES_UNROM_512__\n")
                            f.write("__attribute__((section(\".prg_rom_5\"))) const unsigned char gid_to_collision[GID_TO_TILE_SHARED_COUNT] = {\n")
                            f.write("#else\n")
                            f.write("const unsigned char gid_to_collision[GID_TO_TILE_SHARED_COUNT] = {\n")
                            f.write("#endif\n")
                            gid_to_original = shared_gid_mapping_global['gid_to_original_tile_index']
                            for gid in range(len(shared_gid_mapping_global['gid_map_data'])):
                                original_tile_index = gid_to_original.get(gid, -1)
                                collision_flag = get_collision_for_tile_index(original_tile_index)
                                f.write(f"    {collision_flag}")
                                if gid < len(shared_gid_mapping_global['gid_map_data']) - 1:
                                    f.write(",")
                                f.write("\n")
                            f.write("};\n\n")
                            f.write("#define GID_TO_COLLISION_COUNT GID_TO_TILE_SHARED_COUNT\n\n")
                        else:
                            # Empty mapping (just empty tile)
                            f.write("#ifdef __NES_UNROM_512__\n")
                            f.write("__attribute__((section(\".prg_rom_5\"))) const unsigned char gid_to_tile_shared[1][6] = {\n")
                            f.write("#else\n")
                            f.write("const unsigned char gid_to_tile_shared[1][6] = {\n")
                            f.write("#endif\n")
                            f.write("    // GID 0 (empty tile)\n")
                            f.write("    { 0, 0, 0, 0, 0, 0 }\n")
                            f.write("};\n\n")
                            f.write("#define GID_TO_TILE_SHARED_COUNT 1\n\n")
                        f.write("#endif // GID_TO_TILE_SHARED_H\n")
                    print(f"Generated shared GID mapping with {len(shared_gid_mapping_global['gid_map_data']) if len(shared_gid_mapping_global['gid_map_data']) > 0 else 1} unique GIDs")
                    
                    # Generate separate collapse tile header
                    collapse_tile_header_filename = os.path.join(script_dir, 'gid_to_tile_collapse.h')
                    print(f"\nGenerating collapse tile header: {collapse_tile_header_filename}")
                    with open(collapse_tile_header_filename, 'w') as f:
                        f.write("// Collapse tile GID to tile mapping\n")
                        f.write("// Generated from baseCelesteSpriteSheet.png\n")
                        f.write("// GIDs 24, 25, 26 for collapse tiles (sprite indices 23, 24, 25)\n")
                        f.write("// Each entry: TL_tile, TR_tile, BL_tile, BR_tile, palette_idx, flip_flags\n")
                        f.write("// flip_flags: bit 0=H, bit 1=V, bit 2=D\n\n")
                        f.write("#ifndef GID_TO_TILE_COLLAPSE_H\n")
                        f.write("#define GID_TO_TILE_COLLAPSE_H\n\n")
                        if collapse_tile_gid_data and len(collapse_tile_gid_data) == 3:
                            f.write("#ifdef __NES_UNROM_512__\n")
                            f.write("__attribute__((section(\".prg_rom_5\")))\n")
                            f.write("#endif\n")
                            f.write("const unsigned char gid_to_tile_collapse[3][6] = {\n")
                            collapse_gids = [24, 25, 26]
                            collapse_sprite_names = ["COLLAPSE_TILE_SPRITE_1 (idle)", "COLLAPSE_TILE_SPRITE_2 (collapsing)", "COLLAPSE_TILE_SPRITE_3 (collapsing)"]
                            for i, tile_entry in enumerate(collapse_tile_gid_data):
                                tl, tr, bl, br, pal_idx, flip = tile_entry
                                f.write(f"    // GID {collapse_gids[i]} - {collapse_sprite_names[i]}\n")
                                f.write(f"    {{ {tl}, {tr}, {bl}, {br}, {pal_idx}, {flip} }}")
                                if i < len(collapse_tile_gid_data) - 1:
                                    f.write(",")
                                f.write("\n")
                            f.write("};\n\n")
                            f.write("#define GID_TO_TILE_COLLAPSE_COUNT 3\n\n")
                        else:
                            # Default empty entries
                            f.write("#ifdef __NES_UNROM_512__\n")
                            f.write("__attribute__((section(\".prg_rom_5\")))\n")
                            f.write("#endif\n")
                            f.write("const unsigned char gid_to_tile_collapse[3][6] = {\n")
                            f.write("    // GID 24 - COLLAPSE_TILE_SPRITE_1 (idle)\n")
                            f.write("    { 0, 0, 0, 0, 0, 0 },\n")
                            f.write("    // GID 25 - COLLAPSE_TILE_SPRITE_2 (collapsing)\n")
                            f.write("    { 0, 0, 0, 0, 0, 0 },\n")
                            f.write("    // GID 26 - COLLAPSE_TILE_SPRITE_3 (collapsing)\n")
                            f.write("    { 0, 0, 0, 0, 0, 0 }\n")
                            f.write("};\n\n")
                            f.write("#define GID_TO_TILE_COLLAPSE_COUNT 3\n\n")
                        f.write("#endif // GID_TO_TILE_COLLAPSE_H\n")
                    print(f"Generated collapse tile header with 3 entries")
                    
                    # Generate separate breakable wall header
                    breakable_wall_header_filename = os.path.join(script_dir, 'gid_to_tile_breakable_wall.h')
                    print(f"\nGenerating breakable wall header: {breakable_wall_header_filename}")
                    with open(breakable_wall_header_filename, 'w') as f:
                        f.write("// Breakable wall GID to tile mapping\n")
                        f.write("// Generated from baseCelesteSpriteSheet.png\n")
                        f.write("// GID 27 for breakable walls (sprite index 64)\n")
                        f.write("// Each entry: 16 tiles for 4x4 grid (row-major order), palette_idx, flip_flags\n")
                        f.write("// flip_flags: bit 0=H, bit 1=V, bit 2=D\n")
                        f.write("// Breakable walls are 32x32 sprites (4x4 tiles) extracted from sprite sheet\n\n")
                        f.write("#ifndef GID_TO_TILE_BREAKABLE_WALL_H\n")
                        f.write("#define GID_TO_TILE_BREAKABLE_WALL_H\n\n")
                        if breakable_wall_gid_data and len(breakable_wall_gid_data) == 1:
                            f.write("#ifdef __NES_UNROM_512__\n")
                            f.write("__attribute__((section(\".prg_rom_5\")))\n")
                            f.write("#endif\n")
                            f.write("const unsigned char gid_to_tile_breakable_wall[1][18] = {\n")
                            tile_entry = breakable_wall_gid_data[0]
                            tiles_4x4, pal_idx, flip = tile_entry
                            # tiles_4x4 is already a tuple of 16 tiles in row-major order
                            tiles_4x4_list = list(tiles_4x4)
                            row1_str = ", ".join(map(str, tiles_4x4_list[0:4]))
                            row2_str = ", ".join(map(str, tiles_4x4_list[4:8]))
                            row3_str = ", ".join(map(str, tiles_4x4_list[8:12]))
                            row4_str = ", ".join(map(str, tiles_4x4_list[12:16]))
                            f.write("    // GID 27 - BREAKABLE_WALL_SPRITE_1\n")
                            f.write(f"    // 4x4 grid: Row 1: {row1_str} | Row 2: {row2_str} | Row 3: {row3_str} | Row 4: {row4_str}\n")
                            tiles_str = ", ".join(map(str, tiles_4x4_list))
                            f.write(f"    {{ {tiles_str}, {pal_idx}, {flip} }}\n")
                            f.write("};\n\n")
                            f.write("#define GID_TO_TILE_BREAKABLE_WALL_COUNT 1\n\n")
                        else:
                            # Default empty entry
                            f.write("#ifdef __NES_UNROM_512__\n")
                            f.write("__attribute__((section(\".prg_rom_5\")))\n")
                            f.write("#endif\n")
                            f.write("const unsigned char gid_to_tile_breakable_wall[1][18] = {\n")
                            f.write("    // GID 27 - BREAKABLE_WALL_SPRITE_1\n")
                            f.write("    // 4x4 grid: Row 1: 0,0,0,0 | Row 2: 0,0,0,0 | Row 3: 0,0,0,0 | Row 4: 0,0,0,0\n")
                            f.write("    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }\n")
                            f.write("};\n\n")
                            f.write("#define GID_TO_TILE_BREAKABLE_WALL_COUNT 1\n\n")
                        f.write("#endif // GID_TO_TILE_BREAKABLE_WALL_H\n")
                    print(f"Generated breakable wall header with 1 entry")
                    
                    # Generate separate monument header
                    monument_header_filename = os.path.join(script_dir, 'gid_to_tile_monument.h')
                    print(f"\nGenerating monument header: {monument_header_filename}")
                    with open(monument_header_filename, 'w') as f:
                        f.write("// Monument GID to tile mapping\n")
                        f.write("// Generated from baseCelesteSpriteSheet.png\n")
                        f.write("// GID 70 for monuments (composed of sprites 70, 71, 86, 87)\n")
                        f.write("// Each entry: 16 tiles for 4x4 grid (row-major order), palette_idx, flip_flags\n")
                        f.write("// flip_flags: bit 0=H, bit 1=V, bit 2=D\n")
                        f.write("// Monuments are 32x32 sprites (4x4 tiles) extracted from sprite sheet\n")
                        f.write("// Arranged as: [Sprite 70] [Sprite 71] / [Sprite 86] [Sprite 87]\n\n")
                        f.write("#ifndef GID_TO_TILE_MONUMENT_H\n")
                        f.write("#define GID_TO_TILE_MONUMENT_H\n\n")
                        if monument_gid_data and len(monument_gid_data) == 1:
                            f.write("#ifdef __NES_UNROM_512__\n")
                            f.write("__attribute__((section(\".prg_rom_5\")))\n")
                            f.write("#endif\n")
                            f.write("const unsigned char gid_to_tile_monument[1][18] = {\n")
                            tile_entry = monument_gid_data[0]
                            tiles_4x4, pal_idx, flip = tile_entry
                            # tiles_4x4 is already a tuple of 16 tiles in row-major order
                            tiles_4x4_list = list(tiles_4x4)
                            row1_str = ", ".join(map(str, tiles_4x4_list[0:4]))
                            row2_str = ", ".join(map(str, tiles_4x4_list[4:8]))
                            row3_str = ", ".join(map(str, tiles_4x4_list[8:12]))
                            row4_str = ", ".join(map(str, tiles_4x4_list[12:16]))
                            f.write("    // GID 70 - MONUMENT (composed of sprites 70, 71, 86, 87)\n")
                            f.write(f"    // 4x4 grid: Row 1: {row1_str} | Row 2: {row2_str} | Row 3: {row3_str} | Row 4: {row4_str}\n")
                            tiles_str = ", ".join(map(str, tiles_4x4_list))
                            f.write(f"    {{ {tiles_str}, {pal_idx}, {flip} }}\n")
                            f.write("};\n\n")
                            f.write("#define GID_TO_TILE_MONUMENT_COUNT 1\n\n")
                        else:
                            # Default empty entry
                            f.write("#ifdef __NES_UNROM_512__\n")
                            f.write("__attribute__((section(\".prg_rom_5\")))\n")
                            f.write("#endif\n")
                            f.write("const unsigned char gid_to_tile_monument[1][18] = {\n")
                            f.write("    // GID 70 - MONUMENT (composed of sprites 70, 71, 86, 87)\n")
                            f.write("    // 4x4 grid: Row 1: 0,0,0,0 | Row 2: 0,0,0,0 | Row 3: 0,0,0,0 | Row 4: 0,0,0,0\n")
                            f.write("    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }\n")
                            f.write("};\n\n")
                            f.write("#define GID_TO_TILE_MONUMENT_COUNT 1\n\n")
                        f.write("#endif // GID_TO_TILE_MONUMENT_H\n")
                    print(f"Generated monument header with 1 entry")
                    
                    # Generate separate big chest header
                    big_chest_header_filename = os.path.join(script_dir, 'gid_to_tile_big_chest.h')
                    print(f"\nGenerating big chest header: {big_chest_header_filename}")
                    with open(big_chest_header_filename, 'w') as f:
                        f.write("// Big chest GID to tile mapping\n")
                        f.write("// Generated from baseCelesteSpriteSheet.png\n")
                        f.write("// GID 96 for big chest (composed of sprites 96, 97, 112, 113)\n")
                        f.write("// Each entry: 16 tiles for 4x4 grid (row-major order), palette_idx, flip_flags\n")
                        f.write("// flip_flags: bit 0=H, bit 1=V, bit 2=D\n")
                        f.write("// Big chest is 32x32 sprites (4x4 tiles) extracted from sprite sheet\n")
                        f.write("// Arranged as: [Sprite 96] [Sprite 97] / [Sprite 112] [Sprite 113]\n\n")
                        f.write("#ifndef GID_TO_TILE_BIG_CHEST_H\n")
                        f.write("#define GID_TO_TILE_BIG_CHEST_H\n\n")
                        if big_chest_gid_data and len(big_chest_gid_data) == 1:
                            f.write("#ifdef __NES_UNROM_512__\n")
                            f.write("__attribute__((section(\".prg_rom_5\")))\n")
                            f.write("#endif\n")
                            f.write("const unsigned char gid_to_tile_big_chest[1][18] = {\n")
                            tile_entry = big_chest_gid_data[0]
                            tiles_4x4, pal_idx, flip = tile_entry
                            # tiles_4x4 is already a tuple of 16 tiles in row-major order
                            tiles_4x4_list = list(tiles_4x4)
                            row1_str = ", ".join(map(str, tiles_4x4_list[0:4]))
                            row2_str = ", ".join(map(str, tiles_4x4_list[4:8]))
                            row3_str = ", ".join(map(str, tiles_4x4_list[8:12]))
                            row4_str = ", ".join(map(str, tiles_4x4_list[12:16]))
                            f.write("    // GID 96 - BIG_CHEST (composed of sprites 96, 97)\n")
                            f.write(f"    // 4x4 grid: Row 1: {row1_str} | Row 2: {row2_str} | Row 3: {row3_str} | Row 4: {row4_str}\n")
                            tiles_str = ", ".join(map(str, tiles_4x4_list))
                            f.write(f"    {{ {tiles_str}, {pal_idx}, {flip} }}\n")
                            f.write("};\n\n")
                            f.write("#define GID_TO_TILE_BIG_CHEST_COUNT 1\n\n")
                        else:
                            # Default empty entry
                            f.write("#ifdef __NES_UNROM_512__\n")
                            f.write("__attribute__((section(\".prg_rom_5\")))\n")
                            f.write("#endif\n")
                            f.write("const unsigned char gid_to_tile_big_chest[1][18] = {\n")
                            f.write("    // GID 96 - BIG_CHEST (composed of sprites 96, 97)\n")
                            f.write("    // 4x4 grid: Row 1: 0,0,0,0 | Row 2: 0,0,0,0 | Row 3: 0,0,0,0 | Row 4: 0,0,0,0\n")
                            f.write("    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }\n")
                            f.write("};\n\n")
                            f.write("#define GID_TO_TILE_BIG_CHEST_COUNT 1\n\n")
                        f.write("#endif // GID_TO_TILE_BIG_CHEST_H\n")
                    print(f"Generated big chest header with 1 entry")
                    
                    # Code-referenced objects: objects used dynamically in code but might not appear in level object data
                    # Format: (tile_index, palette_source, base_tile_or_none, palette_fallback)
                    # palette_source: 'inherit_from' = use palette from base_tile, 'mapping' = use object_palette_mapping, 'fixed' = use palette_fallback value, 'background' = use background palette index
                    code_referenced_objects = [
                        # Spring sprite 2 - uses same palette as spring sprite 1 (tile 18)
                        (19, 'inherit_from', 18, 2),  # SPRING_SPRITE_2, inherit from tile 18, fallback palette 2
                        # Balloon string sprites - use same palette as balloon sprite (tile 22)
                        (13, 'inherit_from', 22, 2),  # BALLOON_STRING_1, inherit from tile 22, fallback palette 2
                        (14, 'inherit_from', 22, 2),  # BALLOON_STRING_2, inherit from tile 22, fallback palette 2
                        (15, 'inherit_from', 22, 2),  # BALLOON_STRING_3, inherit from tile 22, fallback palette 2
                        # Balloon sprite - use palette 2 (from nes.c: oamProps = 0x36 means palette 2)
                        (22, 'fixed', None, 2),  # BALLOON_SPRITE_1, use palette 2
                        # Flying berry wing sprites (tiles 45, 46, 47) - use same palette as flying berry (tile 28)
                        (45, 'inherit_from', 28, 0),  # FLYING_BERRY_WING_1, inherit from tile 28, fallback palette 0
                        (46, 'inherit_from', 28, 0),  # FLYING_BERRY_WING_2, inherit from tile 28, fallback palette 0
                        (47, 'inherit_from', 28, 0),  # FLYING_BERRY_WING_3, inherit from tile 28, fallback palette 0
                        # Background tile object sprites - use background palette indices (they're treated as background tiles)
                        (23, 'background', None, 0),  # COLLAPSE_TILE_SPRITE_1, use background palette
                        (24, 'background', None, 0),  # COLLAPSE_TILE_SPRITE_2, use background palette
                        (25, 'background', None, 0),  # COLLAPSE_TILE_SPRITE_3, use background palette
                        (64, 'background', None, 0),  # BREAKABLE_WALL_SPRITE_1, use background palette
                        (70, 'background', None, 0),  # MONUMENT_SPRITE_1, use background palette
                        (71, 'background', None, 0),  # MONUMENT_SPRITE_2, use background palette
                        (86, 'background', None, 0),  # MONUMENT_SPRITE_3, use background palette
                        (87, 'background', None, 0),  # MONUMENT_SPRITE_4, use background palette
                        (96, 'background', None, 0),  # BIG_CHEST_SPRITE_1, use background palette
                        (97, 'background', None, 0),  # BIG_CHEST_SPRITE_2, use background palette
                    ]
                    
                    # Add code-referenced objects if they're missing
                    for tile_idx, palette_source, base_tile_or_none, palette_fallback in code_referenced_objects:
                        if tile_idx not in shared_object_sprite_dict and tile_idx < len(all_sprite_data):
                            # Get optimized tile indices for this sprite
                            tl_opt_idx = tile_mapping.get(tile_idx * 4 + 0, 0)
                            tr_opt_idx = tile_mapping.get(tile_idx * 4 + 1, 0)
                            bl_opt_idx = tile_mapping.get(tile_idx * 4 + 2, 0)
                            br_opt_idx = tile_mapping.get(tile_idx * 4 + 3, 0)
                            
                            # Determine palette index based on palette_source
                            if palette_source == 'inherit_from':
                                # Inherit palette from base tile (e.g., spring sprite 2 from spring sprite 1)
                                if base_tile_or_none in shared_object_sprite_dict:
                                    _, palette_idx, _, _, _, _ = shared_object_sprite_dict[base_tile_or_none]
                                else:
                                    palette_idx = palette_fallback
                            elif palette_source == 'background':
                                # Background tile objects use sprite palette 1 (which is set to background palette 0)
                                # All background tile objects use sprite palette 1 regardless of which background palette they use
                                palette_idx = 1
                            elif palette_source == 'mapping':
                                # Try to get from object_palette_mapping if available
                                if object_palette_mapping and tile_idx in object_palette_mapping:
                                    palette_idx = object_palette_mapping[tile_idx]
                                    if object_palettes and palette_idx >= len(object_palettes):
                                        palette_idx = palette_fallback
                                else:
                                    palette_idx = palette_fallback
                            elif palette_source == 'fixed':
                                # Use fixed palette value
                                palette_idx = palette_fallback
                            else:
                                palette_idx = palette_fallback
                            
                            shared_object_sprite_dict[tile_idx] = (tile_idx, palette_idx, tl_opt_idx, tr_opt_idx, bl_opt_idx, br_opt_idx)
                            print(f"  Added code-referenced object sprite: tile {tile_idx}, palette {palette_idx} (source: {palette_source}), tiles [{tl_opt_idx}, {tr_opt_idx}, {bl_opt_idx}, {br_opt_idx}]")
                    
        except Exception as e:
            print(f"Error processing map JSON: {e}")
            import traceback
            traceback.print_exc()
    else:
        print(f"\nMap JSON file '{tilemap_filename}' not found, skipping map preview generation.")
    
    # Ensure shared object sprite dictionary headers are always generated (even if no levels were processed)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # If shared_object_sprite_dict is empty, try to use data from existing header file as fallback
    if not shared_object_sprite_dict:
        old_header_filename = os.path.join(script_dir, 'object_sprite_dict_shared.h')
        if os.path.exists(old_header_filename):
            print(f"shared_object_sprite_dict is empty, attempting to parse existing header: {old_header_filename}")
            try:
                with open(old_header_filename, 'r') as f:
                    content = f.read()
                    # Try to parse NES format entries from the old header
                    import re
                    # Match NES format: /* index: comment */ { pal, tl, tr, bl, br }
                    nes_pattern = r'/\*\s*(\d+):[^}]*\{\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+)\s*\}'
                    matches = re.findall(nes_pattern, content)
                    for match in matches:
                        tile_idx = int(match[0])
                        pal_idx = int(match[1])
                        tl = int(match[2])
                        tr = int(match[3])
                        bl = int(match[4])
                        br = int(match[5])
                        # Only add non-zero entries (skip unused entries)
                        if pal_idx != 0 or tl != 0 or tr != 0 or bl != 0 or br != 0:
                            shared_object_sprite_dict[tile_idx] = (tile_idx, pal_idx, tl, tr, bl, br)
                    if shared_object_sprite_dict:
                        print(f"Parsed {len(shared_object_sprite_dict)} entries from existing header file")
            except Exception as e:
                print(f"Warning: Could not parse existing header file: {e}")
    
    # Generate shared object sprite dictionary headers for NES and SNES
    # NES format: sparse array indexed by tile_idx, format [pal_idx, tl, tr, bl, br]
    # SNES format: array with tile_idx field, format [tile_idx, pal_idx, tl, tr, bl, br]
    
    # NES uses tile indices up to 47 (flying berry wings: tiles 45, 46, 47)
    # Keep lookup table small - only include tiles we actually use in code
    # The lookup table is in bank 5, but we want to minimize its size
    nes_max_tile_idx = 47  # Flying berry wings are the highest tile indices we need
    
    # Generate NES format header (compact format with lookup table)
    object_sprite_dict_nes_filename = os.path.join(script_dir, 'object_sprite_dict_shared_nes.h')
    print(f"\nGenerating NES object sprite dictionary header: {object_sprite_dict_nes_filename}")
    
    # Build compact array with only used entries, sorted by tile_idx
    compact_entries = []
    for tile_idx in range(nes_max_tile_idx + 1):
        if tile_idx in shared_object_sprite_dict:
            t_idx, pal_idx, tl, tr, bl, br = shared_object_sprite_dict[tile_idx]
            compact_entries.append((tile_idx, pal_idx, tl, tr, bl, br))
    
    # Create lookup table: maps tile_idx -> compact_index (0xFF = not found)
    lookup_table = [0xFF] * (nes_max_tile_idx + 1)
    for compact_idx, (tile_idx, _, _, _, _, _) in enumerate(compact_entries):
        lookup_table[tile_idx] = compact_idx
    
    with open(object_sprite_dict_nes_filename, 'w') as f:
        f.write("// Shared object sprite dictionary for all levels (NES format - compact)\n")
        f.write("// Generated from baseCelesteTileMap.json\n")
        f.write("// Compact format: only used entries stored, with lookup table for fast access\n")
        f.write("// Compact array format: [pal_idx, tl, tr, bl, br] (tile_idx not stored, use lookup table)\n\n")
        f.write("#ifndef OBJECT_SPRITE_DICT_SHARED_NES_H\n")
        f.write("#define OBJECT_SPRITE_DICT_SHARED_NES_H\n\n")
        
        # Write lookup table
        f.write(f"// Lookup table: maps tile_idx -> compact_index (0xFF = not found)\n")
        f.write(f"const unsigned char object_sprite_lookup_table[{nes_max_tile_idx + 1}] = {{\n")
        for tile_idx in range(nes_max_tile_idx + 1):
            lookup_idx = lookup_table[tile_idx]
            f.write(f"    /* tile {tile_idx} */  {lookup_idx}")
            if tile_idx < nes_max_tile_idx:
                f.write(",")
            f.write("\n")
        f.write("};\n\n")
        
        # Write compact array
        f.write(f"// Compact sprite data array (only used entries)\n")
        f.write(f"// Format: [pal_idx, tl, tr, bl, br]\n")
        f.write(f"const unsigned char object_sprite_dict_compact[{len(compact_entries)}][5] = {{\n")
        for compact_idx, (tile_idx, pal_idx, tl, tr, bl, br) in enumerate(compact_entries):
            f.write(f"    /* compact_idx {compact_idx}: tile {tile_idx} */  {{ {pal_idx}, {tl}, {tr}, {bl}, {br} }}")
            if compact_idx < len(compact_entries) - 1:
                f.write(",")
            f.write("\n")
        f.write("};\n\n")
        
        f.write(f"#define OBJECT_SPRITE_DICT_LOOKUP_TABLE_SIZE {nes_max_tile_idx + 1}\n")
        f.write(f"#define OBJECT_SPRITE_DICT_COMPACT_COUNT {len(compact_entries)}\n\n")
        f.write("#endif // OBJECT_SPRITE_DICT_SHARED_NES_H\n")
    nes_entries_used = len(compact_entries)
    old_size = (nes_max_tile_idx + 1) * 5  # Old format: full sparse array
    new_size = (nes_max_tile_idx + 1) + (nes_entries_used * 5)  # New format: lookup table + compact array
    saved_bytes = old_size - new_size
    print(f"Generated NES object sprite dictionary: {nes_entries_used} used entries (compact format)")
    print(f"  Old format: {old_size} bytes, New format: {new_size} bytes, Saved: {saved_bytes} bytes")
    
    # Generate SNES format header
    object_sprite_dict_snes_filename = os.path.join(script_dir, 'object_sprite_dict_shared_snes.h')
    print(f"\nGenerating SNES object sprite dictionary header: {object_sprite_dict_snes_filename}")
    sorted_entries = sorted(shared_object_sprite_dict.items()) if shared_object_sprite_dict else []
    with open(object_sprite_dict_snes_filename, 'w') as f:
        f.write("// Shared object sprite dictionary for all levels (SNES format)\n")
        f.write("// Generated from baseCelesteTileMap.json\n")
        f.write("// SNES/Other: Format with tile_idx field for lookup\n")
        f.write("// Each entry: [tile_idx, pal_idx, tl, tr, bl, br]\n\n")
        f.write("#ifndef OBJECT_SPRITE_DICT_SHARED_SNES_H\n")
        f.write("#define OBJECT_SPRITE_DICT_SHARED_SNES_H\n\n")
        if sorted_entries:
            f.write(f"const unsigned char object_sprite_dict_shared[{len(sorted_entries)}][6] = {{\n")
            for i, (tile_idx, (t_idx, pal_idx, tl, tr, bl, br)) in enumerate(sorted_entries):
                f.write(f"    // Entry {i}\n")
                f.write(f"    {{ {t_idx}, {pal_idx}, {tl}, {tr}, {bl}, {br} }}")
                if i < len(sorted_entries) - 1:
                    f.write(",")
                f.write("\n")
            f.write("};\n\n")
            f.write(f"#define OBJECT_SPRITE_DICT_SHARED_COUNT {len(sorted_entries)}\n\n")
        else:
            # Empty array for compilation
            f.write("const unsigned char object_sprite_dict_shared[1][6] = {\n")
            f.write("    // Empty entry\n")
            f.write("    { 0, 0, 0, 0, 0, 0 }\n")
            f.write("};\n\n")
            f.write("#define OBJECT_SPRITE_DICT_SHARED_COUNT 1\n\n")
        f.write("#endif // OBJECT_SPRITE_DICT_SHARED_SNES_H\n")
    print(f"Generated SNES object sprite dictionary with {len(sorted_entries)} entries")
    
    # Ensure shared GID mapping header is always generated (even if no levels were processed)
    shared_gid_header_filename = os.path.join(script_dir, 'gid_to_tile_shared.h')
    if not os.path.exists(shared_gid_header_filename) or len(shared_gid_mapping_global['gid_map_data']) == 0:
        # Ensure background tile objects are at their reserved GIDs
        collapse_gids = [24, 25, 26]
        breakable_wall_gid = 27
        monument_gids = [70, 71, 86, 87]
        if collapse_tile_gid_data and len(collapse_tile_gid_data) == 3:
            # Ensure gid_map_data is large enough
            while len(shared_gid_mapping_global['gid_map_data']) <= collapse_gids[-1]:
                shared_gid_mapping_global['gid_map_data'].append((0, 0, 0, 0, 0, 0))
            # Insert collapse tile data at GIDs 24, 25, 26
            for i, (target_gid, tile_entry) in enumerate(zip(collapse_gids, collapse_tile_gid_data)):
                shared_gid_mapping_global['gid_map_data'][target_gid] = tile_entry
                shared_gid_mapping_global['tile_to_gid'][tile_entry] = target_gid
                # Track original tile_index
                collapse_sprite_idx = [23, 24, 25][i]
                shared_gid_mapping_global['gid_to_original_tile_index'][target_gid] = collapse_sprite_idx
        
        print(f"\nGenerating shared GID mapping header: {shared_gid_header_filename}")
        with open(shared_gid_header_filename, 'w') as f:
            f.write("// Shared GID to tile mapping for all levels\n")
            f.write("// Generated from baseCelesteTileMap.json\n")
            f.write("// Each entry: TL_tile, TR_tile, BL_tile, BR_tile, palette_idx, flip_flags\n")
            f.write("// flip_flags: bit 0=H, bit 1=V, bit 2=D\n")
            f.write("// GID 0 is reserved for empty tiles\n")
            f.write("// GIDs 24, 25, 26 are reserved for collapse tiles (see gid_to_tile_collapse.h)\n")
            f.write("// GID 27 is reserved for breakable walls (see gid_to_tile_breakable_wall.h)\n")
            f.write("// GIDs 70, 71, 86, 87 are reserved for monuments (see gid_to_tile_monument.h)\n\n")
            f.write("#ifndef GID_TO_TILE_SHARED_H\n")
            f.write("#define GID_TO_TILE_SHARED_H\n\n")
            if len(shared_gid_mapping_global['gid_map_data']) > 0:
                f.write("#ifdef __NES_UNROM_512__\n")
                f.write(f"__attribute__((section(\".prg_rom_5\"))) const unsigned char gid_to_tile_shared[{len(shared_gid_mapping_global['gid_map_data'])}][6] = {{\n")
                f.write("#else\n")
                f.write(f"const unsigned char gid_to_tile_shared[{len(shared_gid_mapping_global['gid_map_data'])}][6] = {{\n")
                f.write("#endif\n")
                for gid in range(len(shared_gid_mapping_global['gid_map_data'])):
                    tl, tr, bl, br, pal_idx, flip = shared_gid_mapping_global['gid_map_data'][gid]
                    f.write(f"    // GID {gid}\n")
                    f.write(f"    {{ {tl}, {tr}, {bl}, {br}, {pal_idx}, {flip} }}")
                    if gid < len(shared_gid_mapping_global['gid_map_data']) - 1:
                        f.write(",")
                    f.write("\n")
                f.write("};\n\n")
                f.write(f"#define GID_TO_TILE_SHARED_COUNT {len(shared_gid_mapping_global['gid_map_data'])}\n\n")
                
                # Generate GID to collision mapping
                f.write("// GID to collision flags mapping\n")
                f.write("// Collision flags: 0 = no collision, 1 = solid, 4/8/16/32 = pointy variants\n")
                f.write("#ifdef __NES_UNROM_512__\n")
                f.write("__attribute__((section(\".prg_rom_5\"))) const unsigned char gid_to_collision[GID_TO_TILE_SHARED_COUNT] = {\n")
                f.write("#else\n")
                f.write("const unsigned char gid_to_collision[GID_TO_TILE_SHARED_COUNT] = {\n")
                f.write("#endif\n")
                gid_to_original = shared_gid_mapping_global['gid_to_original_tile_index']
                for gid in range(len(shared_gid_mapping_global['gid_map_data'])):
                    original_tile_index = gid_to_original.get(gid, -1)
                    collision_flag = get_collision_for_tile_index(original_tile_index)
                    f.write(f"    {collision_flag}")
                    if gid < len(shared_gid_mapping_global['gid_map_data']) - 1:
                        f.write(",")
                    f.write("\n")
                f.write("};\n\n")
                f.write("#define GID_TO_COLLISION_COUNT GID_TO_TILE_SHARED_COUNT\n\n")
            else:
                # Empty mapping (just empty tile)
                f.write("#ifdef __NES_UNROM_512__\n")
                f.write("__attribute__((section(\".prg_rom_5\"))) const unsigned char gid_to_tile_shared[1][6] = {\n")
                f.write("#else\n")
                f.write("const unsigned char gid_to_tile_shared[1][6] = {\n")
                f.write("#endif\n")
                f.write("    // GID 0 (empty tile)\n")
                f.write("    { 0, 0, 0, 0, 0, 0 }\n")
                f.write("};\n\n")
                f.write("#define GID_TO_TILE_SHARED_COUNT 1\n\n")
            f.write("#endif // GID_TO_TILE_SHARED_H\n")
        print(f"Generated shared GID mapping with {len(shared_gid_mapping_global['gid_map_data']) if len(shared_gid_mapping_global['gid_map_data']) > 0 else 1} unique GIDs")
        
        # Generate separate background tile object headers
        # Collapse tile header
        collapse_tile_header_filename = os.path.join(script_dir, 'gid_to_tile_collapse.h')
        print(f"\nGenerating collapse tile header: {collapse_tile_header_filename}")
        with open(collapse_tile_header_filename, 'w') as f:
            f.write("// Collapse tile GID to tile mapping\n")
            f.write("// Generated from baseCelesteSpriteSheet.png\n")
            f.write("// GIDs 24, 25, 26 for collapse tiles (sprite indices 23, 24, 25)\n")
            f.write("// Each entry: TL_tile, TR_tile, BL_tile, BR_tile, palette_idx, flip_flags\n")
            f.write("// flip_flags: bit 0=H, bit 1=V, bit 2=D\n\n")
            f.write("#ifndef GID_TO_TILE_COLLAPSE_H\n")
            f.write("#define GID_TO_TILE_COLLAPSE_H\n\n")
            if collapse_tile_gid_data and len(collapse_tile_gid_data) == 3:
                f.write("const unsigned char gid_to_tile_collapse[3][6] = {\n")
                collapse_gids = [24, 25, 26]
                collapse_sprite_names = ["COLLAPSE_TILE_SPRITE_1 (idle)", "COLLAPSE_TILE_SPRITE_2 (collapsing)", "COLLAPSE_TILE_SPRITE_3 (collapsing)"]
                for i, tile_entry in enumerate(collapse_tile_gid_data):
                    tl, tr, bl, br, pal_idx, flip = tile_entry
                    f.write(f"    // GID {collapse_gids[i]} - {collapse_sprite_names[i]}\n")
                    f.write(f"    {{ {tl}, {tr}, {bl}, {br}, {pal_idx}, {flip} }}")
                    if i < len(collapse_tile_gid_data) - 1:
                        f.write(",")
                    f.write("\n")
                f.write("};\n\n")
                f.write("#define GID_TO_TILE_COLLAPSE_COUNT 3\n\n")
            else:
                # Default empty entries
                f.write("const unsigned char gid_to_tile_collapse[3][6] = {\n")
                f.write("    // GID 24 - COLLAPSE_TILE_SPRITE_1 (idle)\n")
                f.write("    { 0, 0, 0, 0, 0, 0 },\n")
                f.write("    // GID 25 - COLLAPSE_TILE_SPRITE_2 (collapsing)\n")
                f.write("    { 0, 0, 0, 0, 0, 0 },\n")
                f.write("    // GID 26 - COLLAPSE_TILE_SPRITE_3 (collapsing)\n")
                f.write("    { 0, 0, 0, 0, 0, 0 }\n")
                f.write("};\n\n")
                f.write("#define GID_TO_TILE_COLLAPSE_COUNT 3\n\n")
            f.write("#endif // GID_TO_TILE_COLLAPSE_H\n")
        print(f"Generated collapse tile header with 3 entries")
        
        # Breakable wall header
        breakable_wall_header_filename = os.path.join(script_dir, 'gid_to_tile_breakable_wall.h')
        print(f"\nGenerating breakable wall header: {breakable_wall_header_filename}")
        with open(breakable_wall_header_filename, 'w') as f:
            f.write("// Breakable wall GID to tile mapping\n")
            f.write("// Generated from baseCelesteSpriteSheet.png\n")
            f.write("// GID 27 for breakable walls (sprite index 64)\n")
            f.write("// Each entry: 16 tiles for 4x4 grid (row-major order), palette_idx, flip_flags\n")
            f.write("// flip_flags: bit 0=H, bit 1=V, bit 2=D\n")
            f.write("// Breakable walls are 32x32 sprites (4x4 tiles) extracted from sprite sheet\n\n")
            f.write("#ifndef GID_TO_TILE_BREAKABLE_WALL_H\n")
            f.write("#define GID_TO_TILE_BREAKABLE_WALL_H\n\n")
            if breakable_wall_gid_data and len(breakable_wall_gid_data) == 1:
                f.write("#ifdef __NES_UNROM_512__\n")
                f.write("__attribute__((section(\".prg_rom_5\")))\n")
                f.write("#endif\n")
                f.write("const unsigned char gid_to_tile_breakable_wall[1][18] = {\n")
                tile_entry = breakable_wall_gid_data[0]
                tiles_4x4, pal_idx, flip = tile_entry
                # tiles_4x4 is already a tuple of 16 tiles in row-major order
                tiles_4x4_list = list(tiles_4x4)
                row1_str = ", ".join(map(str, tiles_4x4_list[0:4]))
                row2_str = ", ".join(map(str, tiles_4x4_list[4:8]))
                row3_str = ", ".join(map(str, tiles_4x4_list[8:12]))
                row4_str = ", ".join(map(str, tiles_4x4_list[12:16]))
                f.write("    // GID 27 - BREAKABLE_WALL_SPRITE_1\n")
                f.write(f"    // 4x4 grid: Row 1: {row1_str} | Row 2: {row2_str} | Row 3: {row3_str} | Row 4: {row4_str}\n")
                tiles_str = ", ".join(map(str, tiles_4x4_list))
                f.write(f"    {{ {tiles_str}, {pal_idx}, {flip} }}\n")
                f.write("};\n\n")
                f.write("#define GID_TO_TILE_BREAKABLE_WALL_COUNT 1\n\n")
            else:
                # Default empty entry
                f.write("#ifdef __NES_UNROM_512__\n")
                f.write("__attribute__((section(\".prg_rom_5\")))\n")
                f.write("#endif\n")
                f.write("const unsigned char gid_to_tile_breakable_wall[1][18] = {\n")
                f.write("    // GID 27 - BREAKABLE_WALL_SPRITE_1\n")
                f.write("    // 4x4 grid: Row 1: 0,0,0,0 | Row 2: 0,0,0,0 | Row 3: 0,0,0,0 | Row 4: 0,0,0,0\n")
                f.write("    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }\n")
                f.write("};\n\n")
                f.write("#define GID_TO_TILE_BREAKABLE_WALL_COUNT 1\n\n")
            f.write("#endif // GID_TO_TILE_BREAKABLE_WALL_H\n")
        print(f"Generated breakable wall header with 1 entry")
        
        # Monument header
        monument_header_filename = os.path.join(script_dir, 'gid_to_tile_monument.h')
        print(f"\nGenerating monument header: {monument_header_filename}")
        with open(monument_header_filename, 'w') as f:
            f.write("// Monument GID to tile mapping\n")
            f.write("// Generated from baseCelesteSpriteSheet.png\n")
            f.write("// GID 70 for monuments (composed of sprites 70, 71, 86, 87)\n")
            f.write("// Each entry: 16 tiles for 4x4 grid (row-major order), palette_idx, flip_flags\n")
            f.write("// flip_flags: bit 0=H, bit 1=V, bit 2=D\n")
            f.write("// Monuments are 32x32 sprites (4x4 tiles) extracted from sprite sheet\n")
            f.write("// Arranged as: [Sprite 70] [Sprite 71] / [Sprite 86] [Sprite 87]\n\n")
            f.write("#ifndef GID_TO_TILE_MONUMENT_H\n")
            f.write("#define GID_TO_TILE_MONUMENT_H\n\n")
            if monument_gid_data and len(monument_gid_data) == 1:
                f.write("#ifdef __NES_UNROM_512__\n")
                f.write("__attribute__((section(\".prg_rom_5\")))\n")
                f.write("#endif\n")
                f.write("const unsigned char gid_to_tile_monument[1][18] = {\n")
                tile_entry = monument_gid_data[0]
                tiles_4x4, pal_idx, flip = tile_entry
                # tiles_4x4 is already a tuple of 16 tiles in row-major order
                tiles_4x4_list = list(tiles_4x4)
                row1_str = ", ".join(map(str, tiles_4x4_list[0:4]))
                row2_str = ", ".join(map(str, tiles_4x4_list[4:8]))
                row3_str = ", ".join(map(str, tiles_4x4_list[8:12]))
                row4_str = ", ".join(map(str, tiles_4x4_list[12:16]))
                f.write("    // GID 70 - MONUMENT (composed of sprites 70, 71, 86, 87)\n")
                f.write(f"    // 4x4 grid: Row 1: {row1_str} | Row 2: {row2_str} | Row 3: {row3_str} | Row 4: {row4_str}\n")
                tiles_str = ", ".join(map(str, tiles_4x4_list))
                f.write(f"    {{ {tiles_str}, {pal_idx}, {flip} }}\n")
                f.write("};\n\n")
                f.write("#define GID_TO_TILE_MONUMENT_COUNT 1\n\n")
            else:
                # Default empty entry
                f.write("#ifdef __NES_UNROM_512__\n")
                f.write("__attribute__((section(\".prg_rom_5\")))\n")
                f.write("#endif\n")
                f.write("const unsigned char gid_to_tile_monument[1][18] = {\n")
                f.write("    // GID 70 - MONUMENT (composed of sprites 70, 71, 86, 87)\n")
                f.write("    // 4x4 grid: Row 1: 0,0,0,0 | Row 2: 0,0,0,0 | Row 3: 0,0,0,0 | Row 4: 0,0,0,0\n")
                f.write("    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }\n")
                f.write("};\n\n")
                f.write("#define GID_TO_TILE_MONUMENT_COUNT 1\n\n")
            f.write("#endif // GID_TO_TILE_MONUMENT_H\n")
        print(f"Generated monument header with 1 entry")
        
        # Big chest header
        big_chest_header_filename = os.path.join(script_dir, 'gid_to_tile_big_chest.h')
        print(f"\nGenerating big chest header: {big_chest_header_filename}")
        with open(big_chest_header_filename, 'w') as f:
            f.write("// Big chest GID to tile mapping\n")
            f.write("// Generated from baseCelesteSpriteSheet.png\n")
            f.write("// GID 96 for big chest (composed of sprites 96, 97)\n")
            f.write("// Each entry: 16 tiles for 4x4 grid (row-major order), palette_idx, flip_flags\n")
            f.write("// flip_flags: bit 0=H, bit 1=V, bit 2=D\n")
            f.write("// Big chest is 32x32 sprites (4x4 tiles) extracted from sprite sheet\n")
            f.write("// Arranged as: [Sprite 96] [Sprite 97] / [Sprite 112] [Sprite 113]\n\n")
            f.write("#ifndef GID_TO_TILE_BIG_CHEST_H\n")
            f.write("#define GID_TO_TILE_BIG_CHEST_H\n\n")
            if big_chest_gid_data and len(big_chest_gid_data) == 1:
                f.write("#ifdef __NES_UNROM_512__\n")
                f.write("__attribute__((section(\".prg_rom_5\")))\n")
                f.write("#endif\n")
                f.write("const unsigned char gid_to_tile_big_chest[1][18] = {\n")
                tile_entry = big_chest_gid_data[0]
                tiles_4x4, pal_idx, flip = tile_entry
                # tiles_4x4 is already a tuple of 16 tiles in row-major order
                tiles_4x4_list = list(tiles_4x4)
                row1_str = ", ".join(map(str, tiles_4x4_list[0:4]))
                row2_str = ", ".join(map(str, tiles_4x4_list[4:8]))
                row3_str = ", ".join(map(str, tiles_4x4_list[8:12]))
                row4_str = ", ".join(map(str, tiles_4x4_list[12:16]))
                f.write("    // GID 96 - BIG_CHEST (composed of sprites 96, 97)\n")
                f.write(f"    // 4x4 grid: Row 1: {row1_str} | Row 2: {row2_str} | Row 3: {row3_str} | Row 4: {row4_str}\n")
                tiles_str = ", ".join(map(str, tiles_4x4_list))
                f.write(f"    {{ {tiles_str}, {pal_idx}, {flip} }}\n")
                f.write("};\n\n")
                f.write("#define GID_TO_TILE_BIG_CHEST_COUNT 1\n\n")
            else:
                # Default empty entry
                f.write("#ifdef __NES_UNROM_512__\n")
                f.write("__attribute__((section(\".prg_rom_5\")))\n")
                f.write("#endif\n")
                f.write("const unsigned char gid_to_tile_big_chest[1][18] = {\n")
                f.write("    // GID 96 - BIG_CHEST (composed of sprites 96, 97)\n")
                f.write("    // 4x4 grid: Row 1: 0,0,0,0 | Row 2: 0,0,0,0 | Row 3: 0,0,0,0 | Row 4: 0,0,0,0\n")
                f.write("    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }\n")
                f.write("};\n\n")
                f.write("#define GID_TO_TILE_BIG_CHEST_COUNT 1\n\n")
            f.write("#endif // GID_TO_TILE_BIG_CHEST_H\n")
        print(f"Generated big chest header with 1 entry")


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
