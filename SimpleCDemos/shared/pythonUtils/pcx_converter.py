"""
PCX to SNES converter utility.

Converts PCX image files to SNES format:
- 4bpp tile data (.4bpp file)
- 16-color palette in SNES format (.clr file)

This replaces the need for the external pcx2snes tool.
"""

import os
import sys
from typing import List, Tuple, Optional
from PIL import Image


def rgb_to_snes_color(rgb: Tuple[int, int, int]) -> int:
    """
    Convert RGB (0-255) to SNES 15-bit color format.
    
    Args:
        rgb: Tuple of (r, g, b) values (0-255)
    
    Returns:
        SNES color value (15-bit, little-endian format)
    """
    r, g, b = rgb
    # Convert to 5-bit per component
    r_5bit = r >> 3
    g_5bit = g >> 3
    b_5bit = b >> 3
    
    # SNES color format: 0bbbbbgg gggrrrrr (15-bit)
    snes_color = (b_5bit << 10) | (g_5bit << 5) | r_5bit
    return snes_color


def convert_tile_to_4bpp_bitplanes(tile_pixels: List[int]) -> List[int]:
    """
    Convert 8x8 tile pixels to SNES 4bpp bitplane format.
    
    SNES 4bpp format: 32 bytes per tile
    - 4 bitplanes, each plane has 8 bytes (one per row)
    - Planes 0,1: bytes 0-15, Planes 2,3: bytes 16-31
    - Layout: [plane0_row0, plane1_row0, plane0_row1, plane1_row1, ...]
              [plane2_row0, plane3_row0, plane2_row1, plane3_row1, ...]
    
    Args:
        tile_pixels: List of 64 pixel values (8x8 tile), each value is 0-15 (palette index)
    
    Returns:
        List of 32 bytes in SNES 4bpp bitplane format
    """
    tile_data = [0] * 32  # Initialize 32-byte tile
    
    # Process each row (y=0 to 7)
    for y in range(8):
        # Extract the 8 pixels for this row
        row_pixels = tile_pixels[y * 8:(y + 1) * 8]
        
        # Process each bitplane (0-3)
        for plane in range(4):
            # Calculate the bitplane pair (0-1) and which bit within the pair (0 or 1)
            plane_pair = plane // 2  # 0 or 1
            bit_in_pair = plane % 2  # 0 or 1
            
            # Calculate the byte offset for this plane pair
            byte_offset = plane_pair * 16  # 0 or 16
            
            # Process each pixel in the row (x=0 to 7)
            byte_value = 0
            for x in range(8):
                pixel_value = row_pixels[x]
                # Extract the bit for this plane from the pixel value
                bit = (pixel_value >> plane) & 1
                # Set the bit in the appropriate position (MSB first)
                byte_value |= (bit << (7 - x))
            
            # Store the byte for this plane and row
            tile_data[byte_offset + y * 2 + bit_in_pair] = byte_value
    
    return tile_data


def quantize_image_to_16_colors(img: Image.Image) -> Tuple[Image.Image, List[Tuple[int, int, int]]]:
    """
    Quantize an image to 16 colors using PIL's quantization.
    
    Args:
        img: PIL Image (RGB mode)
    
    Returns:
        Tuple of (quantized_image, palette_colors)
        - quantized_image: Image with palette mode, indices 0-15
        - palette_colors: List of 16 RGB tuples
    """
    # Quantize to 16 colors (use MEDIANCUT method if available, otherwise default)
    try:
        # Try newer PIL API first
        quantized = img.quantize(colors=16, method=Image.Quantize.MEDIANCUT)
    except (AttributeError, TypeError):
        # Fall back to older PIL API
        quantized = img.quantize(colors=16)
    
    # Get the palette - PIL's getpalette() returns a list of up to 768 bytes (256 colors * 3)
    palette = quantized.getpalette()
    if palette is None:
        raise ValueError("Failed to get palette from quantized image")
    
    # Extract 16 colors from the palette with bounds checking
    palette_colors = []
    for i in range(16):
        idx = i * 3
        # Check bounds to avoid index errors
        if idx + 2 < len(palette):
            r = palette[idx]
            g = palette[idx + 1]
            b = palette[idx + 2]
            palette_colors.append((r, g, b))
        else:
            # Pad with black if palette is shorter than expected
            palette_colors.append((0, 0, 0))
    
    # Ensure we have exactly 16 colors
    while len(palette_colors) < 16:
        palette_colors.append((0, 0, 0))
    
    palette_colors = palette_colors[:16]
    
    return quantized, palette_colors


def generate_c_header(
    output_path: str,
    base_name: str,
    tiles_data: List[int],
    palette_colors: List[Tuple[int, int, int]],
    tiles_x: int,
    tiles_y: int,
    generate_tilemap: bool = False,
    palette_index: int = 0,
    tilemap_data: Optional[List[int]] = None,
    verbose: bool = True
) -> bool:
    """
    Generate a C header file with tile data and palette.
    
    Args:
        output_path: Path to output .h file
        base_name: Base name for the resource (used for variable names)
        tiles_data: List of tile data bytes
        palette_colors: List of 16 RGB color tuples
        tiles_x: Number of tiles horizontally
        tiles_y: Number of tiles vertically
        verbose: If True, print progress messages
    
    Returns:
        True if successful, False otherwise
    """
    try:
        # Convert base_name to camelCase for variable names
        # e.g., "business-man" -> "businessMan", "player" -> "player"
        var_name_parts = base_name.replace('-', '_').split('_')
        var_base = var_name_parts[0] + ''.join(word.capitalize() for word in var_name_parts[1:])
        
        num_tiles = tiles_x * tiles_y
        num_unique_tiles = len(tiles_data) // 32  # 32 bytes per 4bpp tile
        
        if verbose:
            print(f"Writing C header file to: {output_path}")
        
        with open(output_path, 'w') as f:
            # Header comment
            f.write(f"/* Generated by pcx_converter.py from {base_name}.pcx */\n")
            f.write(f"/* Image size: {tiles_x * 8}x{tiles_y * 8} pixels */\n")
            f.write(f"/* Tiles: {tiles_x}x{tiles_y} ({num_tiles} tilemap entries, {num_unique_tiles} unique tiles, {len(tiles_data)} bytes) */\n")
            f.write("/* Format: 4bpp bitplane (32 bytes per tile) */\n")
            f.write(f"/* Palette: 16 colors */\n\n")
            
            # Palette array
            f.write(f"/* {var_base} Palette - 16 colors */\n")
            f.write(f"const unsigned short {var_base}Palette[16] = {{\n")
            for i, color in enumerate(palette_colors):
                snes_color = rgb_to_snes_color(color)
                f.write(f"    0x{snes_color:04X}")
                if i < 15:
                    f.write(",")
                f.write(f"  // Color {i}: RGB({color[0]}, {color[1]}, {color[2]})\n")
            f.write("};\n\n")
            
            # Tile data array
            f.write(f"/* {var_base} Tile data - {len(tiles_data)} bytes ({num_unique_tiles} unique tiles) */\n")
            f.write(f"const unsigned char {var_base}Tiles[] = {{\n")
            for i in range(0, len(tiles_data), 16):
                f.write("    ")
                for j in range(16):
                    if i + j < len(tiles_data):
                        f.write(f"0x{tiles_data[i + j]:02X}")
                        if i + j < len(tiles_data) - 1:
                            f.write(", ")
                f.write("\n")
            f.write("};\n\n")
            
            # Tilemap (for background images)
            if generate_tilemap:
                # Use provided tilemap_data if available (with deduplication), otherwise sequential
                # Each tilemap entry: tile_index | (palette << 10)
                f.write(f"/* {var_base} Tilemap - {num_tiles} entries ({tiles_x}x{tiles_y} tiles) */\n")
                f.write(f"/* Each entry: tile_index | (palette << 10) */\n")
                f.write(f"const unsigned short {var_base}Map[] = {{\n")
                for y in range(tiles_y):
                    f.write("    ")
                    for x in range(tiles_x):
                        if tilemap_data is not None:
                            # Use deduplicated tilemap data
                            tile_index = tilemap_data[y * tiles_x + x]
                        else:
                            # Fallback to sequential (for backwards compatibility)
                            tile_index = y * tiles_x + x
                        tilemap_entry = tile_index | (palette_index << 10)
                        f.write(f"0x{tilemap_entry:04X}")
                        if y < tiles_y - 1 or x < tiles_x - 1:
                            f.write(",")
                        if x < tiles_x - 1:
                            f.write(" ")
                    f.write("\n")
                f.write("};\n\n")
            
            # Constants
            f.write("/* Constants */\n")
            f.write(f"#define {var_base.upper()}TILES_COUNT {num_unique_tiles}\n")  # Unique tiles, not tilemap entries
            f.write(f"#define {var_base.upper()}TILES_SIZE {len(tiles_data)}\n")
            f.write(f"#define {var_base.upper()}TILES_X {tiles_x}\n")
            f.write(f"#define {var_base.upper()}TILES_Y {tiles_y}\n")
            f.write(f"#define {var_base.upper()}PALETTE_SIZE 16\n")
            f.write(f"#define {var_base.upper()}BYTES_PER_TILE 32\n")
            if generate_tilemap:
                f.write(f"#define {var_base.upper()}MAP_SIZE {num_tiles * 2}\n")
        
        if verbose:
            print(f"  Generated header file with {num_tiles} tiles and 16-color palette")
        
        return True
    
    except Exception as e:
        if verbose:
            print(f"Error generating header file: {e}", file=sys.stderr)
        return False


def convert_pcx_to_snes(
    pcx_path: str,
    output_header_path: Optional[str] = None,
    verbose: bool = True
) -> bool:
    """
    Convert a PCX file to SNES format and generate a C header file.
    
    This function mimics the behavior of: pcx2snes -n -s8 -c16 -o16
    but generates a C header file instead of binary files.
    
    Args:
        pcx_path: Path to input PCX file
        output_header_path: Path to output .h file
                           If None, uses same directory and base name as PCX
        verbose: If True, print progress messages
    
    Returns:
        True if conversion successful, False otherwise
    """
    try:
        # Load PCX image
        if verbose:
            print(f"Loading PCX file: {pcx_path}")
        
        img = Image.open(pcx_path)
        
        # Convert to RGB if needed
        if img.mode != 'RGB':
            img = img.convert('RGB')
        
        if verbose:
            print(f"  Image size: {img.width}x{img.height}")
        
        # Quantize to 16 colors
        if verbose:
            print("Quantizing to 16 colors...")
        
        quantized_img, palette_colors = quantize_image_to_16_colors(img)
        
        if verbose:
            print(f"  Extracted {len(palette_colors)} colors")
        
        # Determine output path and base name
        if output_header_path is None:
            base_path = os.path.splitext(pcx_path)[0]
            output_header_path = base_path + ".h"
        
        base_name = os.path.splitext(os.path.basename(pcx_path))[0]
        
        # Calculate number of tiles (8x8 tiles)
        tile_width = 8
        tile_height = 8
        tiles_x = (img.width + tile_width - 1) // tile_width  # Round up
        tiles_y = (img.height + tile_height - 1) // tile_height  # Round up
        
        if verbose:
            print(f"Converting to {tiles_x}x{tiles_y} tiles ({tiles_x * tiles_y} total)...")
        
        # Convert image to tiles with deduplication
        tiles_data = []
        tilemap_data = []  # Will store tile indices for tilemap
        unique_tiles = {}  # Maps tile signature (tuple) to tile index
        tile_index = 0
        pixels = quantized_img.load()
        
        for tile_y_idx in range(tiles_y):
            for tile_x_idx in range(tiles_x):
                # Extract 8x8 tile
                tile_pixels = []
                for y in range(tile_height):
                    for x in range(tile_width):
                        px = tile_x_idx * tile_width + x
                        py = tile_y_idx * tile_height + y
                        
                        if px < img.width and py < img.height:
                            # Get palette index (0-15)
                            palette_index = pixels[px, py]
                        else:
                            # Pad with transparent (index 0)
                            palette_index = 0
                        
                        tile_pixels.append(palette_index)
                
                # Convert tile to 4bpp bitplane format
                tile_data = convert_tile_to_4bpp_bitplanes(tile_pixels)
                
                # Create tile signature for uniqueness checking
                tile_signature = tuple(tile_data)
                
                if tile_signature in unique_tiles:
                    # Use existing tile
                    tile_number = unique_tiles[tile_signature]
                else:
                    # New unique tile
                    unique_tiles[tile_signature] = tile_index
                    tile_number = tile_index
                    
                    # Add tile data (32 bytes for 4bpp)
                    tiles_data.extend(tile_data)
                    
                    tile_index += 1
                
                # Store tile number for tilemap
                tilemap_data.append(tile_number)
        
        num_unique_tiles = len(unique_tiles)
        if verbose:
            print(f"  Generated {len(tiles_data)} bytes of tile data")
            print(f"  Unique tiles: {num_unique_tiles} (out of {tiles_x * tiles_y} total)")
            if num_unique_tiles < tiles_x * tiles_y:
                print(f"  Deduplication saved {tiles_x * tiles_y - num_unique_tiles} tiles")
        
        # Determine if this is a background image (needs tilemap)
        # Background images are typically 256x224 (32x28 tiles)
        is_background = (img.width == 256 and img.height == 224)
        
        # Generate C header file
        success = generate_c_header(
            output_header_path,
            base_name,
            tiles_data,
            palette_colors,
            tiles_x,
            tiles_y,
            generate_tilemap=is_background,
            palette_index=0,  # Use palette 0 for backgrounds
            tilemap_data=tilemap_data if is_background else None,  # Pass tilemap data
            verbose=verbose
        )
        
        if verbose and success:
            print("Conversion complete!")
        
        return success
    
    except Exception as e:
        if verbose:
            print(f"Error converting {pcx_path}: {e}", file=sys.stderr)
        return False
