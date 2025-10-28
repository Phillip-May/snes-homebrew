#!/usr/bin/env python3
"""
generate_font.py - SNES Mode 5 BG1 Font Generator

Generates font glyph data for SNES Mode 5 BG1:
- Render size: 16x16 pixels (or multiples) with square pixels for proper aspect ratio
- Storage size: 16x8 pixels (2 adjacent 8x8 tiles)
- Display: Mode 5 renders at half-width, stretching 16x8 back to appear as 32x16 on screen
- Font: ASCII 32-122 (space to lowercase z)
- Format: 4bpp bitplane (same as ConvertImageMode5.py)

The font is rendered at square aspect ratio, then compressed horizontally for storage.
When Mode 5 displays it with half-width pixels, it appears at the correct aspect ratio.

This script reuses the tile conversion logic from ConvertImageMode5.py
to ensure compatibility with SNES Mode 5 rendering.
"""

import os
import sys
import argparse
import platform
from PIL import Image, ImageDraw, ImageFont, ImageFilter, ImageEnhance
from ConvertImageMode5 import SNESMode5Converter


# ============================================================================
# FONT RENDERING CONFIGURATION
# ============================================================================

# Render scale multiplier for the 16x16 base glyph size
# 1 = 16x16, 2 = 32x32, 3 = 48x48, etc.
# Higher values give better anti-aliasing but may look softer
RENDER_SCALE = 8

# Font size (in pixels, will be scaled to fit in square glyph space)
FONT_SIZE = 16

# Downscaling filter for resizing from render size to final size
# Options: Image.NEAREST, Image.BILINEAR, Image.BICUBIC, Image.LANCZOS
# NEAREST = sharp/blocky, BILINEAR = smooth/fast, BICUBIC = smoother, LANCZOS = highest quality
DOWNSCALE_FILTER = Image.LANCZOS

# Sharpness enhancement (0.0 = no sharpening, 1.0 = normal, 2.0+ = strong)
# Use higher values to compensate for smoothing from anti-aliasing
# Recommended: 0.0 for RENDER_SCALE=1, 1.0-2.0 for RENDER_SCALE=2+
SHARPNESS = 2.5

# Vertical offset for descender characters (g, j, p, q, y)
# Negative = shift up, Positive = shift down
DESCENDER_OFFSET_Y = -1


class SNESMode5FontGenerator:
    """SNES Mode 5 Font Generator
    
    Generates font data for Mode 5 with proper aspect ratio handling:
    1. Render at NxN (square pixels, proper aspect ratio)
    2. Scale to 16x8 (compressed horizontally)
    3. Store as 2 adjacent 8x8 tiles
    4. Mode 5 displays at half-width, stretching back to correct aspect ratio
    """
    
    def __init__(self, render_scale=RENDER_SCALE, sharpness=SHARPNESS, downscale_filter=DOWNSCALE_FILTER):
        # Final glyph dimensions (storage)
        self.glyph_width = 16   # Two 8x8 tiles side by side
        self.glyph_height = 8   # One tile tall
        
        # Render scale, sharpness, and downscale filter
        self.render_scale = render_scale
        self.sharpness = sharpness
        self.downscale_filter = downscale_filter
        
        # Render glyph dimensions (square aspect ratio)
        self.render_glyph_size = 16 * self.render_scale  # Square rendering (16x16, 32x32, etc.)
        
        # ASCII range for font characters
        self.ascii_start = 32  # space
        self.ascii_end = 122   # lowercase z
        self.char_count = self.ascii_end - self.ascii_start + 1
        
        # Characters per row in the font atlas
        self.chars_per_row = 16
        self.rows = (self.char_count + self.chars_per_row - 1) // self.chars_per_row
        
        # Render atlas dimensions (square aspect ratio)
        self.atlas_render_width = self.chars_per_row * self.render_glyph_size
        self.atlas_render_height = self.rows * self.render_glyph_size
        
        # Final atlas dimensions (compressed for Mode 5 storage)
        self.atlas_width = self.chars_per_row * self.glyph_width
        self.atlas_height = self.rows * self.glyph_height
        
        # Use shared converter for tile conversion (4bpp Mode 5 format)
        self.converter = SNESMode5Converter()
        
    def render_font_atlas(self, font_path=None, font_size=FONT_SIZE):
        """Render font atlas with all characters at square aspect ratio
        
        Renders at NxN (square pixels) then scales to 16x8 (compressed).
        Mode 5's half-width pixel rendering will restore the correct aspect ratio.
        """
        print(f"Creating font atlas for ASCII {self.ascii_start}-{self.ascii_end} ({self.char_count} characters)")
        print(f"Render size: {self.render_glyph_size}x{self.render_glyph_size} per glyph (square aspect ratio)")
        print(f"Storage size: {self.glyph_width}x{self.glyph_height} per glyph (2 tiles, compressed)")
        print(f"Display: Mode 5 half-width pixels stretch back to correct aspect ratio")
        print(f"Render atlas: {self.atlas_render_width}x{self.atlas_render_height}")
        print(f"Final atlas: {self.atlas_width}x{self.atlas_height}")
        print(f"Layout: {self.chars_per_row} chars per row, {self.rows} rows")
        
        # Create atlas image at square aspect ratio
        render_atlas = Image.new('RGB', (self.atlas_render_width, self.atlas_render_height), (0, 0, 0))
        draw = ImageDraw.Draw(render_atlas)
        
        # Load font
        # Font size should scale with render_scale to maintain same relative size
        scaled_font_size = int(font_size * self.render_scale)
        
        # Try to use specified font, or find a system monospaced font
        font_to_use = font_path
        if not font_to_use or not os.path.exists(font_to_use):
            if not font_to_use:
                print("No font specified, searching for system monospaced font...")
            else:
                print(f"Font not found: {font_to_use}")
                print("Searching for system monospaced font...")
            
            font_to_use = self.find_monospaced_font()
            if font_to_use:
                print(f"Found system font: {font_to_use}")
            else:
                print("ERROR: No suitable monospaced font found on system!")
                print("Please specify a font with --font parameter")
                print("Example: --font C:/Windows/Fonts/consola.ttf")
                sys.exit(1)
        
        try:
            font = ImageFont.truetype(font_to_use, scaled_font_size)
            print(f"Using font: {os.path.basename(font_to_use)} (size {scaled_font_size})")
            use_default_font = False
        except Exception as e:
            print(f"Error loading font {font_to_use}: {e}")
            print("Please specify a valid TrueType font with --font parameter")
            sys.exit(1)
        
        # Track character positions
        char_positions = {}
        
        # Render each character centered in its square glyph space
        for i in range(self.char_count):
            ascii_code = self.ascii_start + i
            char = chr(ascii_code)
            
            # Calculate position in render atlas (square glyphs)
            row = i // self.chars_per_row
            col = i % self.chars_per_row
            glyph_x = col * self.render_glyph_size
            glyph_y = row * self.render_glyph_size
            
            char_positions[ascii_code] = (col * self.glyph_width, row * self.glyph_height)
            
            # Render TrueType character centered in glyph space
            try:
                bbox = draw.textbbox((0, 0), char, font=font)
                char_width = bbox[2] - bbox[0]
                char_height = bbox[3] - bbox[1]
                
                # Center character in the square glyph space
                offset_x = glyph_x + (self.render_glyph_size - char_width) // 2 - bbox[0]
                offset_y = glyph_y + (self.render_glyph_size - char_height) // 2 - bbox[1] + (DESCENDER_OFFSET_Y * self.render_scale)
                
                draw.text((offset_x, offset_y), char, font=font, fill=(255, 255, 255))
            except Exception as e:
                # Fallback if textbbox fails
                offset_x = glyph_x + self.render_glyph_size // 4
                offset_y = glyph_y + self.render_glyph_size // 4
                draw.text((offset_x, offset_y), char, font=font, fill=(255, 255, 255))
            
            if (i + 1) % 16 == 0:
                print(f"  Rendered {i + 1}/{self.char_count} characters...")
        
        # Scale down to final dimensions (compresses horizontally by 2x)
        filter_names = {
            Image.NEAREST: 'NEAREST', 
            Image.BILINEAR: 'BILINEAR', 
            Image.BICUBIC: 'BICUBIC', 
            Image.LANCZOS: 'LANCZOS'
        }
        filter_name = filter_names.get(self.downscale_filter, 'UNKNOWN')
        print(f"Scaling from {self.atlas_render_width}x{self.atlas_render_height} to {self.atlas_width}x{self.atlas_height}")
        print(f"  (horizontal compression by {self.atlas_render_width / self.atlas_width:.1f}x, filter: {filter_name})")
        atlas = render_atlas.resize((self.atlas_width, self.atlas_height), self.downscale_filter)
        
        # Apply sharpening to compensate for anti-aliasing smoothing
        if self.sharpness > 0:
            print(f"Applying sharpness enhancement (strength: {self.sharpness:.1f}x)")
            # Apply unsharp mask for better quality sharpening
            atlas = atlas.filter(ImageFilter.UnsharpMask(radius=1, percent=int(self.sharpness * 100), threshold=0))
            # Optionally boost contrast slightly for even sharper edges
            if self.sharpness >= 2.0:
                enhancer = ImageEnhance.Contrast(atlas)
                atlas = enhancer.enhance(1.1)  # 10% contrast boost
        
        print("Font atlas rendered successfully")
        return atlas, char_positions
    
    def find_monospaced_font(self):
        """Find a suitable monospaced font on the system"""
        system = platform.system()
        
        # Common monospaced fonts to try, in order of preference
        if system == 'Windows':
            font_paths = [
                'C:/Windows/Fonts/consola.ttf',      # Consolas (preferred)
                'C:/Windows/Fonts/cour.ttf',         # Courier New
                'C:/Windows/Fonts/lucon.ttf',        # Lucida Console
                'C:/Windows/Fonts/CONSOLA.TTF',      # Consolas (alt case)
                'C:/Windows/Fonts/COUR.TTF',         # Courier New (alt case)
            ]
        elif system == 'Darwin':  # macOS
            font_paths = [
                '/System/Library/Fonts/Monaco.dfont',
                '/Library/Fonts/Courier New.ttf',
                '/System/Library/Fonts/Menlo.ttc',
            ]
        else:  # Linux and others
            font_paths = [
                '/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf',
                '/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf',
                '/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf',
                '/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf',
            ]
        
        # Try to find an available font
        for font_path in font_paths:
            if os.path.exists(font_path):
                return font_path
        
        return None
    
    def extract_font_palette(self, img, max_colors=4):
        """Extract a simple grayscale palette for 2bpp font (4 colors)"""
        print(f"Extracting {max_colors}-color grayscale palette for 2bpp font...")
        
        # For fonts, use a simple grayscale palette
        # This ensures good readability with anti-aliasing
        palette_colors = [
            (0, 0, 0),       # Color 0: Black (transparent/background)
            (85, 85, 85),    # Color 1: Dark gray
            (170, 170, 170), # Color 2: Light gray  
            (255, 255, 255)  # Color 3: White (foreground)
        ]
        
        return palette_colors[:max_colors]
    
    def convert_atlas_to_mode5(self, atlas, output_path):
        """Convert font atlas to SNES Mode 5 format using ConvertImageMode5 pipeline"""
        print("\nConverting font atlas to SNES Mode 5 format...")
        
        # Step 1: Quantize the atlas to 2bpp (4 colors) for font
        print("Quantizing to 2bpp (4 colors)...")
        # Extract palette but limit to 4 colors for 2bpp
        palette_colors = self.extract_font_palette(atlas, max_colors=4)
        img_quantized, _ = self.converter.create_color_map(atlas, palette_colors)
        
        # Step 2: Convert to tiles using Mode 5 tile conversion
        print("Converting to Mode 5 tiles...")
        tile_data = self.generate_font_tiles(img_quantized)
        
        # Step 3: Generate header file
        print("Generating header file...")
        self.write_font_header(output_path, palette_colors, tile_data)
        
        # Step 4: Generate single preview image
        print("Generating preview image...")
        self.create_preview_image(img_quantized, palette_colors, output_path)
        
        print("\nFont generation complete!")
        print(f"Output file: {output_path}")
        print(f"Preview: {output_path.replace('.h', '_preview.png')}")
        print(f"Characters: {self.char_count}")
        print(f"Glyphs: {self.char_count} (each {self.glyph_width}x{self.glyph_height} pixels = 2 tiles)")
        print(f"Total tiles: {self.char_count * 2}")
        print(f"Tile data: {self.char_count * 2 * 16} bytes (2bpp)")
        
    def generate_font_tiles(self, img_quantized):
        """Generate font tile data from quantized atlas image
        
        Returns dictionary with tile data and character mapping.
        """
        print("Extracting tile data from font atlas...")
        
        tiles_data = []
        pixels = img_quantized.load()
        
        # Process each character glyph (16x8 = 2 adjacent 8x8 tiles)
        for i in range(self.char_count):
            ascii_code = self.ascii_start + i
            
            # Calculate glyph position in atlas
            row = i // self.chars_per_row
            col = i % self.chars_per_row
            glyph_x = col * self.glyph_width
            glyph_y = row * self.glyph_height
            
            # Each glyph is 16x8 = 2 adjacent 8x8 tiles (LEFT and RIGHT)
            # Extract LEFT tile (8x8)
            left_tile_pixels = []
            for y in range(8):
                for x in range(8):
                    pixel_x = glyph_x + x
                    pixel_y = glyph_y + y
                    color_idx = pixels[pixel_x, pixel_y]
                    left_tile_pixels.append(color_idx)
            
            # Extract RIGHT tile (8x8)
            right_tile_pixels = []
            for y in range(8):
                for x in range(8):
                    pixel_x = glyph_x + 8 + x  # Offset by 8 for right tile
                    pixel_y = glyph_y + y
                    color_idx = pixels[pixel_x, pixel_y]
                    right_tile_pixels.append(color_idx)
            
            # Convert both tiles to 2bpp bitplanes using shared converter
            left_tile_data = self.converter.convert_tile_to_bitplanes(left_tile_pixels, bpp=2)
            right_tile_data = self.converter.convert_tile_to_bitplanes(right_tile_pixels, bpp=2)
            
            # Add both tiles to the tile data
            tiles_data.extend(left_tile_data)   # LEFT tile (16 bytes)
            tiles_data.extend(right_tile_data)  # RIGHT tile (16 bytes)
        
        print(f"Extracted {self.char_count} glyphs ({self.char_count * 2} tiles)")
        print(f"Tile data size: {len(tiles_data)} bytes")
        
        return {
            'tiles_data': tiles_data,
            'tile_count': self.char_count * 2
        }
    
    def write_font_header(self, output_path, palette_colors, tile_data):
        """Write font data to C header file"""
        print(f"Writing font header to {output_path}")
        
        tiles_data = tile_data['tiles_data']
        tile_count = tile_data['tile_count']
        
        with open(output_path, 'w') as f:
            f.write("/* Generated by generate_font.py - SNES Mode 5 Font Data */\n")
            f.write(f"/* Rendered at: {self.render_glyph_size}x{self.render_glyph_size} (square aspect ratio) */\n")
            f.write(f"/* Stored as: {self.glyph_width}x{self.glyph_height} (2 adjacent 8x8 tiles, compressed) */\n")
            f.write("/* Mode 5 display: Half-width pixels stretch back to correct aspect ratio */\n")
            f.write("/* Format: 2bpp bitplane (16 bytes per tile, 32 bytes per glyph) */\n")
            f.write(f"/* Characters: ASCII {self.ascii_start}-{self.ascii_end} ({self.char_count} glyphs) */\n")
            f.write(f"/* Total tiles: {tile_count} (2 tiles per glyph) */\n\n")
            
            # Write palette
            f.write("/* 2bpp Font Palette - 4 colors (grayscale) */\n")
            f.write("const unsigned short font_palette[4] = {\n")
            for i in range(4):
                color = palette_colors[i] if i < len(palette_colors) else (0, 0, 0)
                # Convert to SNES 15-bit BGR format
                r5 = (color[0] * 31) // 255
                g5 = (color[1] * 31) // 255
                b5 = (color[2] * 31) // 255
                snes_color = (b5 << 10) | (g5 << 5) | r5
                
                f.write(f"    0x{snes_color:04X}")
                if i < 3:
                    f.write(",")
                f.write(f"  // Color {i}: RGB({color[0]}, {color[1]}, {color[2]})\n")
            f.write("};\n\n")
            
            # Write tile data
            f.write(f"/* Font tile data - {len(tiles_data)} bytes ({tile_count} tiles) */\n")
            f.write("/* Each glyph uses 2 consecutive tiles (LEFT, RIGHT) = 32 bytes */\n")
            f.write("const unsigned char font_tiles[] = {\n")
            for i in range(0, len(tiles_data), 16):
                f.write("    ")
                for j in range(16):
                    if i + j < len(tiles_data):
                        f.write(f"0x{tiles_data[i + j]:02X}")
                        if i + j < len(tiles_data) - 1:
                            f.write(", ")
                f.write("\n")
            f.write("};\n\n")
            
            # Write constants
            f.write("/* Font constants */\n")
            f.write(f"#define FONT_GLYPH_WIDTH {self.glyph_width}\n")
            f.write(f"#define FONT_GLYPH_HEIGHT {self.glyph_height}\n")
            f.write(f"#define FONT_TILES_PER_GLYPH 2\n")
            f.write(f"#define FONT_GLYPH_COUNT {self.char_count}\n")
            f.write(f"#define FONT_TILES_COUNT {tile_count}\n")
            f.write(f"#define FONT_TILES_SIZE {len(tiles_data)}\n")
            f.write(f"#define FONT_PALETTE_SIZE 4\n")
            f.write(f"#define FONT_CHARS_START {self.ascii_start}\n")
            f.write(f"#define FONT_CHARS_END {self.ascii_end}\n")
            f.write(f"#define FONT_BYTES_PER_TILE 16\n")
            f.write(f"#define FONT_BYTES_PER_GLYPH 32\n")
            f.write(f"#define FONT_BPP 2\n\n")
            
            # Write helper functions
            f.write("/* Helper functions */\n")
            f.write("/* Get tile index from ASCII code: tile_index = (ascii_code - FONT_CHARS_START) * 2 */\n")
            f.write("static inline unsigned char get_char_tile(unsigned char ascii_code) {\n")
            f.write(f"    if (ascii_code < {self.ascii_start} || ascii_code > {self.ascii_end}) return 0xFF;\n")
            f.write(f"    return (ascii_code - {self.ascii_start}) * 2;\n")
            f.write("}\n\n")
            
            f.write("static inline int is_printable_char(unsigned char ascii_code) {\n")
            f.write(f"    return (ascii_code >= {self.ascii_start} && ascii_code <= {self.ascii_end});\n")
            f.write("}\n\n")
            
            f.write("/* Font tile data access macro */\n")
            f.write("/* Returns pointer to first tile of glyph (32 bytes = 2 tiles) */\n")
            f.write("#define FONT_GLYPH_DATA(tile_index) (&font_tiles[(tile_index) * 16])\n")
        
        print("Font header file written successfully")
    
    def create_preview_image(self, img_quantized, palette_colors, output_path):
        """Create a single preview image showing the font atlas"""
        print("Creating preview image...")
        
        # Convert the quantized palette image to RGB for preview
        preview = Image.new('RGB', (self.atlas_width, self.atlas_height), (0, 0, 0))
        pixels_in = img_quantized.load()
        pixels_out = preview.load()
        
        for y in range(self.atlas_height):
            for x in range(self.atlas_width):
                palette_idx = pixels_in[x, y]
                if palette_idx < len(palette_colors):
                    pixels_out[x, y] = palette_colors[palette_idx]
                else:
                    pixels_out[x, y] = (0, 0, 0)
        
        preview_path = output_path.replace('.h', '_preview.png')
        preview.save(preview_path)
        print(f"Preview image saved: {preview_path}")
        print(f"Preview shows glyphs as stored (16x8, compressed)")
        print(f"Mode 5 will display them stretched back to correct aspect ratio")
        
        return preview_path
    
    def generate_font(self, font_path=None, output_path="font_data.h", font_size=FONT_SIZE):
        """Main function to generate the complete font"""
        print("=" * 70)
        print("SNES Mode 5 Font Generator")
        print("=" * 70)
        
        # Step 1: Render font atlas
        atlas, char_positions = self.render_font_atlas(font_path, font_size)
        
        # Step 2: Convert to Mode 5 format
        self.convert_atlas_to_mode5(atlas, output_path)
        
        print("=" * 70)


def main():
    parser = argparse.ArgumentParser(
        description='Generate SNES Mode 5 font data with proper aspect ratio',
        epilog='Renders at square aspect ratio, compresses to 16x8 for storage. '
               'Mode 5 half-width pixels stretch back to correct aspect ratio on display.'
    )
    parser.add_argument('--font', '-f', help='Path to TTF font file (optional)')
    parser.add_argument('--output', '-o', default='font_data.h', help='Output header file path')
    parser.add_argument('--size', '-s', type=int, default=FONT_SIZE, help='Font size in pixels')
    parser.add_argument('--scale', type=int, default=RENDER_SCALE, help='Render scale multiplier (1=16x16, 2=32x32, etc.)')
    parser.add_argument('--filter', choices=['nearest', 'bilinear', 'bicubic', 'lanczos'], default='lanczos', 
                        help='Downscaling filter (nearest=sharp, bilinear=smooth, bicubic=smoother, lanczos=highest quality)')
    parser.add_argument('--sharpness', type=float, default=SHARPNESS, help='Sharpness enhancement (0.0=none, 1.0=normal, 2.0+=strong)')
    
    args = parser.parse_args()
    
    # Map filter argument to PIL constant
    filter_map = {
        'nearest': Image.NEAREST,
        'bilinear': Image.BILINEAR,
        'bicubic': Image.BICUBIC,
        'lanczos': Image.LANCZOS
    }
    downscale_filter = filter_map.get(args.filter, Image.LANCZOS)
    
    # Get the directory where this script is located
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # If output path is relative, make it relative to the script directory
    output_path = args.output
    if not os.path.isabs(output_path):
        output_path = os.path.join(script_dir, output_path)
    
    # Ensure the output directory exists
    output_dir = os.path.dirname(output_path)
    if output_dir and not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    print(f"Script directory: {script_dir}")
    print(f"Output will be saved to: {output_path}")
    print()
    
    generator = SNESMode5FontGenerator(render_scale=args.scale, sharpness=args.sharpness, downscale_filter=downscale_filter)
    generator.generate_font(args.font, output_path, args.size)


if __name__ == "__main__":
    main()