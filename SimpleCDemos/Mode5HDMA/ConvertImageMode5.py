#!/usr/bin/env python3
"""
ConvertImageMode5.py - SNES Mode 5 BG1 Image Converter

Converts a PNG image to SNES Mode 5 BG1 format:
- Resolution: 512x224 pixels (double horizontal resolution)
- Tiles: 8x8 pixels each, but tilemap entries refer to 2 adjacent tiles (16x8 effective)
- Color depth: 4bpp (16 colors)
- Display: Each line displayed twice (interlaced effect)
- Output: C header file with palette, tiles, and tilemap data for vbcc65816
"""

import os
import sys
import subprocess
import tempfile
from PIL import Image
from dataclasses import dataclass
from typing import List, Dict, Set, Optional, Tuple

# Configuration variables - modify these paths as needed
INPUT_IMAGE_PATH = None
OUTPUT_HEADER_PATH = os.path.join(os.path.dirname(__file__), "BG18_mode5.h")  # Mode 5 4bpp header

@dataclass
class TileData:
    """Data structure to hold all tile and tilemap related information"""
    tiles_data: List[int]
    tilemap_data: List[int]
    num_tiles: int
    tiles_x: int
    tiles_y: int
    tile_partition_map: Dict[int, int]
    partition_0_tiles: List[int]
    partition_1_tiles: List[int]
    scanline_partition_usage: Dict[int, Set[int]]
    tile_storage_map: Dict[int, int]
    tile_partition_usage: Dict[int, Set[int]]

class SNESMode5Converter:
    """SNES Mode 5 Image Converter
    
    Converts images to SNES Mode 5 format with support for both 2bpp and 4bpp tile conversion.
    The convert_tile_to_bitplanes() method supports both formats and is used by both
    ConvertImageMode5.py (4bpp) and generate_font.py (2bpp).
    """
    def __init__(self):
        self.tile_width = 8
        self.tile_height = 8
        self.screen_width = 512  # Mode 5 uses 512x224 resolution (double horizontal)
        self.screen_height = 224
        self.tiles_x = self.screen_width // self.tile_width  # 64 tiles (512/8)
        self.tiles_y = self.screen_height // self.tile_height  # 28 tiles (224/8)
        self.max_colors = 16  # 4bpp for Mode 5 (16 colors per background)
        self.bitplanes = 4     # 4 bitplanes for 4bpp format
        
        # Mode 5 specific: tilemap entries refer to 2 adjacent 8x8 tiles
        self.tilemap_tiles_x = self.tiles_x // 2  # 32 tilemap entries (64/2)
        self.tilemap_tiles_y = self.tiles_y       # 28 tilemap entries
        
    def load_image(self, image_path, brightness=1.0):
        """Load and validate the input PNG image for Mode 5"""
        try:
            img = Image.open(image_path)
            if img.mode != 'RGB':
                img = img.convert('RGB')
            
            print(f"Loaded image: {img.width}x{img.height} pixels")
            
            # Apply brightness adjustment if needed
            if brightness != 1.0:
                print(f"Applying brightness adjustment: {brightness}x")
                import numpy as np
                img_array = np.array(img, dtype=np.float32)
                img_array = np.clip(img_array * brightness, 0, 255)
                img = Image.fromarray(img_array.astype(np.uint8))
            
            return img
        except Exception as e:
            print(f"Error loading image: {e}")
            return None
    
    def resize_image_for_mode5(self, img):
        """Step 1: Resize image to SNES Mode 5 dimensions with proper aspect ratio compensation"""
        print("=== STEP 1: Resizing image for SNES Mode 5 ===")
        
        # Resize directly to 512x224 with proper SNES Mode 5 aspect ratio compensation
        # SNES Mode 5: 512x224 pixels with non-square pixels (~2.61:1 effective display ratio)
        # We need to pre-warp the image to account for the SNES pixel aspect ratio
        
        # Calculate the effective display aspect ratio for SNES Mode 5
        # 512/224 = 2.2857, but SNES pixels are ~0.5 width, so effective ratio is ~2.61:1
        snes_pixel_aspect_ratio = 0.5  # SNES pixels are roughly half-width
        effective_display_ratio = (self.screen_width / self.screen_height) / snes_pixel_aspect_ratio
        
        print(f"SNES Mode 5 effective display ratio: {effective_display_ratio:.3f}:1")
        print(f"Resizing from {img.width}x{img.height} directly to {self.screen_width}x{self.screen_height}")
        print(f"Using aspect ratio compensation for non-square SNES pixels")
        
        # Resize directly to 512x224 with high-quality resampling
        img = img.resize((self.screen_width, self.screen_height), Image.LANCZOS)
        print(f"Image resized successfully to SNES Mode 5 dimensions")
        
        print(f"Final image dimensions: {img.width}x{img.height}")
        print(f"Tilemap grid size: {self.tiles_x}x{self.tiles_y} (Mode 5: 64x28 tiles)")
        print(f"Tilemap entries: {self.tilemap_tiles_x}x{self.tilemap_tiles_y} (Mode 5: 32x28 entries, each refers to 2 tiles)")
        
        return img
    
    def quantize_image_for_mode5(self, img):
        """Step 2: Quantize image colors for SNES Mode 5 4bpp format using pngquant"""
        print("=== STEP 2: Quantizing image for SNES Mode 5 using pngquant ===")
        
        # Save the resized image to a temporary file for pngquant processing
        temp_input = tempfile.NamedTemporaryFile(suffix='.png', delete=False)
        temp_output = tempfile.NamedTemporaryFile(suffix='.png', delete=False)
        temp_input.close()
        temp_output.close()
        
        try:
            # Save the resized image
            img.save(temp_input.name)
            print(f"Saved resized image to temporary file: {temp_input.name}")
            
            # Check how many colors the image actually has
            original_colors = img.getcolors(maxcolors=256*256*256)
            if original_colors:
                total_colors = len(original_colors)
                print(f"Resized image has {total_colors} unique colors")
            else:
                total_colors = 0
                print("Could not analyze color count")
            
            # Use pngquant to quantize to 16 colors (4bpp)
            print("Applying pngquant quantization to 16 colors (4bpp)...")
            
            # pngquant command: --force to overwrite output, 16 colors for 4bpp
            # Syntax: pngquant [options] <ncolors> [pngfile]
            pngquant_cmd = [
                'pngquant.exe',
                '--force',
                '16',  # Number of colors (positional argument)
                temp_input.name
            ]
            
            print(f"Running pngquant command: {' '.join(pngquant_cmd)}")
            # Use absolute paths for better Windows compatibility
            pngquant_path = os.path.join(os.path.dirname(__file__), 'pngquant.exe')
            if os.path.exists(pngquant_path):
                pngquant_cmd[0] = pngquant_path
            result = subprocess.run(pngquant_cmd, capture_output=True, text=True)
            
            if result.returncode != 0:
                print(f"pngquant failed with return code {result.returncode}")
                print(f"stderr: {result.stderr}")
                print("Falling back to PIL quantization...")
                # Fallback to PIL quantization
                working_img = img.quantize(colors=16)
                working_img = working_img.convert('RGB')
            else:
                print("pngquant completed successfully")
                # This version of pngquant creates output with -fs8.png suffix
                # Try to find the generated file
                base_name = os.path.splitext(temp_input.name)[0]
                possible_outputs = [
                    base_name + '-fs8.png',  # Floyd-Steinberg dithering
                    base_name + '-or8.png',  # Ordered dithering
                    temp_output.name  # Fallback to original expected name
                ]
                
                working_img = None
                for output_file in possible_outputs:
                    if os.path.exists(output_file):
                        working_img = Image.open(output_file)
                        print(f"Found pngquant output: {output_file}")
                        break
                
                if working_img is None:
                    print("Could not find pngquant output file, falling back to PIL quantization...")
                    working_img = img.quantize(colors=16)
                    working_img = working_img.convert('RGB')
                else:
                    if working_img.mode != 'RGB':
                        working_img = working_img.convert('RGB')
                    print(f"Loaded pngquant output: {working_img.width}x{working_img.height}")
                
        except Exception as e:
            print(f"pngquant processing failed: {e}")
            print("Falling back to PIL quantization...")
            # Fallback to PIL quantization
            try:
                working_img = img.quantize(colors=16)
                working_img = working_img.convert('RGB')
            except Exception as e2:
                print(f"PIL fallback also failed: {e2}")
                working_img = img
        finally:
            # Clean up temporary files
            try:
                os.unlink(temp_input.name)
                os.unlink(temp_output.name)
                # Also clean up pngquant output files if they exist
                base_name = os.path.splitext(temp_input.name)[0]
                for suffix in ['-fs8.png', '-or8.png']:
                    pngquant_output = base_name + suffix
                    if os.path.exists(pngquant_output):
                        os.unlink(pngquant_output)
            except:
                pass
        
        # Extract regular 4bpp palette (HDMA disabled)
        palette_colors = self.extract_palette(working_img)
        
        # Create color mapping using regular palette
        img_quantized, palette_data = self.create_color_map(working_img, palette_colors)
        
        print(f"Quantization complete: {len(palette_colors)} colors extracted")
        print(f"Quantized image dimensions: {img_quantized.width}x{img_quantized.height}")
        
        return img_quantized, palette_colors
    
    def generate_tiles_and_tilemap(self, img_quantized):
        """Step 3: Generate tiles and tilemap data for SNES Mode 5"""
        print("=== STEP 3: Generating tiles and tilemap for SNES Mode 5 ===")
        
        # Convert to tiles
        tiles_data, tilemap_data, num_tiles, actual_tiles_x, actual_tiles_y, tile_partition_map, partition_0_tiles, partition_1_tiles, scanline_partition_usage, tile_storage_map, tile_partition_usage = self.convert_image_to_tiles(img_quantized)
        
        # Check if data is too large for ROM bank
        if len(tiles_data) > 60000:  # Leave some room for other data
            print(f"\nWarning: Tile data ({len(tiles_data)} bytes) is too large for ROM bank!")
            print("Creating simplified version with reduced complexity...")
            
            # Create a simplified version with fewer unique tiles
            simplified_img = self.create_simple_test_image()
            
            # Re-extract regular palette and convert
            self.palette_colors = self.extract_palette(simplified_img)
            img_quantized, palette_data = self.create_color_map(simplified_img, self.palette_colors)
            
            # Convert to tiles again
            tiles_data, tilemap_data, num_tiles, actual_tiles_x, actual_tiles_y, tile_partition_map, partition_0_tiles, partition_1_tiles, scanline_partition_usage, tile_storage_map, tile_partition_usage = self.convert_image_to_tiles(img_quantized)
            
            if len(tiles_data) > 60000:
                print("Even simplified version is too large. Using minimal pattern.")
                # Use the simple test pattern as last resort
                simple_img = self.create_simple_test_image()
                self.palette_colors = self.extract_palette(simple_img)
                img_quantized, palette_data = self.create_color_map(simple_img, self.palette_colors)
                tiles_data, tilemap_data, num_tiles, actual_tiles_x, actual_tiles_y, tile_partition_map, partition_0_tiles, partition_1_tiles, scanline_partition_usage, tile_storage_map, tile_partition_usage = self.convert_image_to_tiles(img_quantized)
        
        print(f"Tiles and tilemap generation complete:")
        print(f"  Generated {num_tiles} unique tiles")
        print(f"  Tile data size: {len(tiles_data)} bytes")
        print(f"  Tilemap size: {len(tilemap_data)} entries (each refers to 2 tiles)")
        print(f"  Tilemap dimensions: {actual_tiles_x}x{actual_tiles_y}")
        
        # Return as structured data
        return TileData(
            tiles_data=tiles_data,
            tilemap_data=tilemap_data,
            num_tiles=num_tiles,
            tiles_x=actual_tiles_x,
            tiles_y=actual_tiles_y,
            tile_partition_map=tile_partition_map,
            partition_0_tiles=partition_0_tiles,
            partition_1_tiles=partition_1_tiles,
            scanline_partition_usage=scanline_partition_usage,
            tile_storage_map=tile_storage_map,
            tile_partition_usage=tile_partition_usage
        )
    
    def generate_header_file_for_mode5(self, output_path, palette_colors, tile_data: TileData, img_quantized):
        """Step 4: Generate C header file for SNES Mode 5"""
        print("=== STEP 4: Generating C header file for SNES Mode 5 ===")
        
        # Calculate similarity metrics
        mse, psnr, ssim = 0, 0, 0
        print(f"Image Quality Metrics:")
        print(f"  MSE (Mean Squared Error): {mse:.2f}")
        print(f"  PSNR (Peak Signal-to-Noise Ratio): {psnr:.2f} dB")
        print(f"  SSIM (Structural Similarity Index): {ssim:.4f}")
        print(f"  Quality: Regular 4bpp palette for Mode 5")
        
        # Generate header file with regular 4bpp palette data
        print("Generating Mode 5 4bpp header file with regular palette...")
        self.generate_header_file(
            output_path, palette_colors, tile_data.tiles_data, tile_data.tilemap_data, 
            tile_data.num_tiles, None, tile_data.tiles_x, tile_data.tiles_y, 
            None, tile_data.tile_partition_map, tile_data.partition_0_tiles, 
            tile_data.partition_1_tiles, tile_data.scanline_partition_usage, 
            tile_data.tile_storage_map, tile_data.tile_partition_usage
        )
        
        # Create preview images
        print("\nGenerating preview images...")
        self.create_palette_preview(palette_colors, output_path)
        self.create_4bpp_preview(img_quantized, palette_colors, output_path)
        
        print(f"Header file generation complete: {output_path}")
        print(f"Generated {tile_data.num_tiles} unique tiles")
        print(f"Tile data size: {len(tile_data.tiles_data)} bytes")
        print(f"Tilemap size: {len(tile_data.tilemap_data)} entries (each refers to 2 tiles)")
        print(f"Regular 4bpp palette: 16 colors")
        print(f"Total unique colors: {len(set(palette_colors))} (limited to 16)")
        print(f"Mode 5 resolution: 512x224 pixels (double horizontal resolution)")
        print(f"Preview images created for verification")
    
    def convert_image_pipeline(self, input_path, output_path):
        """Main pipeline method that calls all conversion steps in sequence"""
        print("=== SNES Mode 5 Image Conversion Pipeline ===")
        print(f"Input: {input_path}")
        print(f"Output: {output_path}")
        print()
        
        # Step 1: Load and resize image
        original_img = self.load_image(input_path, brightness=1.0)
        if original_img is None:
            print("ERROR: Failed to load image")
            return False
        
        resized_img = self.resize_image_for_mode5(original_img)
        
        # Step 2: Quantize image
        img_quantized, palette_colors = self.quantize_image_for_mode5(resized_img)
        
        # Step 3: Generate tiles and tilemap
        tile_data = self.generate_tiles_and_tilemap(img_quantized)
        
        # Step 4: Generate header file
        self.generate_header_file_for_mode5(output_path, palette_colors, tile_data, img_quantized)
        
        print("\n=== Pipeline Complete ===")
        print(f"Conversion successful! Output saved to {output_path}")
        return True
    
    

    def extract_palette(self, img):
        """Extract and optimize color palette for regular 4bpp mode"""
        print("Extracting regular 4bpp color palette...")
        
        # Get all unique colors from the image
        colors = img.getcolors(maxcolors=256*256*256)
        if colors is None:
            print("Image has too many colors, using quantization...")
            # Image has too many colors, need to quantize
            img_quantized = img.quantize(colors=self.max_colors)
            # Get colors from quantized image
            colors = img_quantized.getcolors(maxcolors=self.max_colors)
    
        if colors is None:
            colors = []
        
        # Sort by frequency (most common colors first)
        colors.sort(key=lambda x: x[0], reverse=True)
        
        # Take up to 16 most common colors, ensuring color 0 is pure black
        palette_colors = []
        
        # Always start with pure black as color 0
        palette_colors.append((0, 0, 0))
        
        # Add other colors, but avoid adding pure black again
        for count, color in colors[:self.max_colors]:
            if color != (0, 0, 0) and len(palette_colors) < self.max_colors:
                palette_colors.append(color)
        
        # Pad with black if we have fewer than 16 colors
        while len(palette_colors) < self.max_colors:
            palette_colors.append((0, 0, 0))
        
        print(f"Extracted {len(palette_colors)} colors for regular 4bpp palette")
        print("Palette colors (first 16):")
        display_count = min(16, len(palette_colors))
        for i, color in enumerate(palette_colors[:display_count]):
            print(f"  Color {i}: RGB({color[0]}, {color[1]}, {color[2]})")
        if len(palette_colors) > display_count:
            print(f"  ... and {len(palette_colors) - display_count} more colors")
        
        # Validate palette size
        if len(palette_colors) != self.max_colors:
            print(f"WARNING: Palette has {len(palette_colors)} colors, expected {self.max_colors}!")
        
        return palette_colors

    def rgb_to_snes_color(self, rgb):
        """Convert RGB (0-255) to SNES 15-bit color format"""
        r, g, b = rgb
        # Convert to 5-bit per component
        r_5bit = r >> 3
        g_5bit = g >> 3  
        b_5bit = b >> 3
        
        # SNES color format: 0bbbbbgg gggrrrrr
        snes_color = (b_5bit << 10) | (g_5bit << 5) | r_5bit
        return snes_color
    

    def create_color_map(self, img, palette_colors):
        """Create a quantized image using the extracted palette for regular 4bpp mode"""
        print("Creating quantized image with regular 4bpp palette...")
        
        # Create a new image with our palette
        img_quantized = Image.new('P', img.size)
        
        # Set up the palette
        palette_data = []
        for color in palette_colors:
            palette_data.extend(color)
        # Pad to 256 colors
        while len(palette_data) < 768:  # 256 * 3
            palette_data.extend([0, 0, 0])
        img_quantized.putpalette(palette_data)
        
        # Convert each pixel to the closest palette color
        pixels = img.load()
        quantized_pixels = img_quantized.load()
        
        for y in range(img.height):
            for x in range(img.width):
                rgb = pixels[x, y]
                
                # Find closest palette color
                min_dist = float('inf')
                closest_idx = 0
                for i, palette_color in enumerate(palette_colors):
                    dist = sum((a - b) ** 2 for a, b in zip(rgb, palette_color))
                    if dist < min_dist:
                        min_dist = dist
                        closest_idx = i
                
                quantized_pixels[x, y] = closest_idx
        
        return img_quantized, {}

    def convert_tile_to_bitplanes(self, tile_pixels, bpp=4):
        """Convert 8x8 tile pixels to SNES bitplane format
        
        Args:
            tile_pixels: List of 64 pixel values (8x8 tile)
            bpp: Bits per pixel (2 or 4)
                 2bpp: 2 bitplanes, 16 bytes per tile
                 4bpp: 4 bitplanes, 32 bytes per tile
        
        Returns:
            List of bytes in SNES bitplane format
        """
        if bpp == 2:
            # SNES 2bpp format: 16 bytes per tile (INTERLEAVED)
            # Byte 0: Row 0, Bitplane 0
            # Byte 1: Row 0, Bitplane 1
            # Byte 2: Row 1, Bitplane 0
            # Byte 3: Row 1, Bitplane 1
            # ... (pattern repeats for all 8 rows)
            
            tile_data = [0] * 16
            
            for y in range(8):
                row_pixels = tile_pixels[y * 8:(y + 1) * 8]
                
                # Build bitplane bytes for this row
                bitplane0 = 0
                bitplane1 = 0
                
                for x in range(8):
                    pixel_value = row_pixels[x]
                    
                    # Extract bits
                    bit0 = pixel_value & 0x01
                    bit1 = (pixel_value & 0x02) >> 1
                    
                    # Set bits in bitplanes (MSB first)
                    if bit0:
                        bitplane0 |= (1 << (7 - x))
                    if bit1:
                        bitplane1 |= (1 << (7 - x))
                
                # Store interleaved: plane 0, then plane 1 for this row
                tile_data[y * 2] = bitplane0
                tile_data[y * 2 + 1] = bitplane1
            
            return tile_data
            
        elif bpp == 4:
            # SNES 4bpp uses bitplane format: 32 bytes per tile
            # 4 bitplanes, each plane has 8 bytes (one per row)
            # Planes 0,1: bytes 0-15, Planes 2,3: bytes 16-31
            
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
                        # Set the bit in the appropriate position
                        byte_value |= (bit << (7 - x))  # MSB first
                    
                    # Store the byte for this plane and row
                    tile_data[byte_offset + y * 2 + bit_in_pair] = byte_value
            
            return tile_data
        else:
            raise ValueError(f"Unsupported bpp value: {bpp}. Must be 2 or 4.")
    
    def create_simple_test_image(self):
        """Create a simple test image with patterns to reduce data size for Mode 5"""
        print("Creating simple test pattern for Mode 5...")
        
        # Create a simple 512x224 image with patterns
        img = Image.new('RGB', (self.screen_width, self.screen_height), (0, 0, 0))
        pixels = img.load()
        
        # Create simple patterns to minimize unique tiles
        for y in range(self.screen_height):
            for x in range(self.screen_width):
                # Create simple patterns
                if (x // 64 + y // 32) % 2 == 0:
                    # Checkerboard pattern
                    if (x // 16 + y // 8) % 2 == 0:
                        pixels[x, y] = (255, 0, 0)  # Red
                    else:
                        pixels[x, y] = (0, 255, 0)  # Green
                else:
                    # Gradient pattern
                    intensity = (x + y) % 256
                    pixels[x, y] = (intensity, intensity // 2, intensity // 4)
        
        return img
    
    def convert_image_to_tiles(self, img_quantized):
        """Convert entire image to tile data for Mode 5 with optimized tile placement"""
        print("=== PHASE 1: Analyzing tile usage patterns ===")
        
        # Calculate tile dimensions - use the full tilemap grid
        actual_tiles_x = self.tiles_x  # 64 tiles
        actual_tiles_y = self.tiles_y  # 28 tiles
        
        print(f"Image size: {img_quantized.width}x{img_quantized.height}")
        print(f"Tilemap grid: {actual_tiles_x}x{actual_tiles_y}")
        print(f"Mode 5 tilemap entries: {self.tilemap_tiles_x}x{self.tilemap_tiles_y} (each entry refers to 2 tiles)")
        
        # Verify palette indexing
        pixels = img_quantized.load()
        used_indices = set()
        for y in range(min(10, img_quantized.height)):  # Sample first 10 rows
            for x in range(min(10, img_quantized.width)):  # Sample first 10 columns
                used_indices.add(pixels[x, y])
        print(f"Sample palette indices used: {sorted(list(used_indices))}")
        print(f"Total unique palette indices in sample: {len(used_indices)}")
        
        # SNES Mode 5 tile limit: 1024 tiles (0-1023) due to 10-bit tile index
        MAX_TILES_PER_PARTITION = 1024
        
        # PHASE 1: Analyze all tile pairs and their usage patterns
        unique_tile_pairs = {}  # Maps signature to tile data
        tilemap_entries = []    # Stores tilemap entry info for later processing
        tile_usage_by_scanline = {}  # Maps scanline to set of tile signatures used
        
        # DEBUG: Track all indices we encounter during tile processing
        tile_processing_indices = set()
        
        print("Analyzing tile usage across all scanlines...")
        
        # Process Mode 5 tilemap entries (each represents 2 adjacent 8x8 tiles)
        for tilemap_y in range(self.tilemap_tiles_y):
            for tilemap_x in range(self.tilemap_tiles_x):
                # Calculate the two tile positions for this tilemap entry
                left_tile_x = tilemap_x * 2
                right_tile_x = tilemap_x * 2 + 1
                
                # Get tile data for both tiles
                left_tile_pixels = []
                right_tile_pixels = []
                
                for y in range(8):
                    for x in range(8):
                        # Left tile (8x8)
                        left_pixel_x = left_tile_x * 8 + x
                        left_pixel_y = tilemap_y * 8 + y
                        left_color_idx = pixels[left_pixel_x, left_pixel_y]
                        left_tile_pixels.append(left_color_idx)
                        tile_processing_indices.add(left_color_idx)
                        
                        # Right tile (8x8)
                        right_pixel_x = right_tile_x * 8 + x
                        right_pixel_y = tilemap_y * 8 + y
                        right_color_idx = pixels[right_pixel_x, right_pixel_y]
                        right_tile_pixels.append(right_color_idx)
                        tile_processing_indices.add(right_color_idx)
                
                # Convert both tiles to bitplanes
                left_tile_data = self.convert_tile_to_bitplanes(left_tile_pixels)
                right_tile_data = self.convert_tile_to_bitplanes(right_tile_pixels)
                
                # Check if both tiles are empty (all black - palette index 0)
                left_is_empty = all(pixel == 0 for pixel in left_tile_pixels)
                right_is_empty = all(pixel == 0 for pixel in right_tile_pixels)
                
                if left_is_empty and right_is_empty:
                    # Empty tile pair - use special signature
                    tile_pair_signature = "EMPTY"
                    tile_pair_data = ([0] * 32, [0] * 32)  # Empty tile data
                else:
                    # Create a signature for the tile pair (both tiles must be consecutive)
                    tile_pair_signature = (tuple(left_tile_data), tuple(right_tile_data))
                    tile_pair_data = (left_tile_data, right_tile_data)
                
                # Store tile pair data if not already stored
                if tile_pair_signature not in unique_tile_pairs:
                    unique_tile_pairs[tile_pair_signature] = tile_pair_data
                
                # Track which scanline uses this tile pair
                scanline = tilemap_y * 8  # Each tilemap row is 8 scanlines tall
                if scanline not in tile_usage_by_scanline:
                    tile_usage_by_scanline[scanline] = set()
                tile_usage_by_scanline[scanline].add(tile_pair_signature)
                
                # Store tilemap entry info for later processing
                tilemap_entries.append({
                    'tilemap_x': tilemap_x,
                    'tilemap_y': tilemap_y,
                    'scanline': scanline,
                    'tile_pair_signature': tile_pair_signature,
                    'left_tile_data': left_tile_data,
                    'right_tile_data': right_tile_data
                })
        
        print(f"Found {len(unique_tile_pairs)} unique tile pairs")
        
        # PHASE 2: Determine partition split point and shared tiles
        print("=== PHASE 2: Determining partition split point ===")
        
        # First, simulate sequential tile allocation to find where we exceed 1024 tiles
        # This will tell us which scanlines need partition 1
        temp_tile_index = 2  # Start after empty tile
        temp_signature_to_index = {"EMPTY": 0}
        scanline_requires_partition_1 = {}
        
        for entry in tilemap_entries:
            signature = entry['tile_pair_signature']
            scanline = entry['scanline']
            
            if signature not in temp_signature_to_index:
                temp_signature_to_index[signature] = temp_tile_index
                temp_tile_index += 2
            
            # Check if this tile requires partition 1 (tile index >= 1024)
            tile_idx = temp_signature_to_index[signature]
            if tile_idx >= MAX_TILES_PER_PARTITION:
                scanline_requires_partition_1[scanline] = True
        
        # Determine which scanlines need partition 0 vs partition 1
        partition_0_scanlines = set()
        partition_1_scanlines = set()
        
        for scanline in range(0, self.screen_height, 8):
            if scanline in scanline_requires_partition_1:
                partition_1_scanlines.add(scanline)
            else:
                partition_0_scanlines.add(scanline)
        
        print(f"Partition 0 scanlines: {sorted(partition_0_scanlines)}")
        print(f"Partition 1 scanlines: {sorted(partition_1_scanlines)}")
        
        # Now find tiles used in both partitions
        partition_0_tile_signatures = set()
        partition_1_tile_signatures = set()
        
        for scanline, tile_signatures in tile_usage_by_scanline.items():
            if scanline in partition_0_scanlines:
                partition_0_tile_signatures.update(tile_signatures)
            if scanline in partition_1_scanlines:
                partition_1_tile_signatures.update(tile_signatures)
        
        # Find tiles used in both partitions
        shared_tile_signatures = partition_0_tile_signatures.intersection(partition_1_tile_signatures)
        partition_0_only_signatures = partition_0_tile_signatures - partition_1_tile_signatures
        partition_1_only_signatures = partition_1_tile_signatures - partition_0_tile_signatures
        
        print(f"Partition 0 only tiles: {len(partition_0_only_signatures)}")
        print(f"Partition 1 only tiles: {len(partition_1_only_signatures)}")
        print(f"Shared tiles: {len(shared_tile_signatures)}")
        
        # Check if partition 0 only tiles would overflow the 0-767 range
        MAX_PARTITION_0_ONLY_PAIRS = 384  # 768 tiles / 2 tiles per pair
        MAX_SHARED_PAIRS = 128  # 256 tiles / 2 tiles per pair (tiles 768-1023)
        
        if len(partition_0_only_signatures) >= MAX_PARTITION_0_ONLY_PAIRS:
            overflow_count = len(partition_0_only_signatures) - MAX_PARTITION_0_ONLY_PAIRS
            print(f"WARNING: Partition 0 only has {len(partition_0_only_signatures)} tile pairs, exceeds max of {MAX_PARTITION_0_ONLY_PAIRS}")
            print(f"  Overflow: {overflow_count} tile pairs")
            
            # Check if we have room in the shared section
            available_shared_space = MAX_SHARED_PAIRS - len(shared_tile_signatures)
            
            if overflow_count <= available_shared_space:
                print(f"  Solution: Moving {overflow_count} overflow tiles to shared section")
                print(f"  (Shared section has {available_shared_space} pairs available)")
                
                # Move overflow tiles from partition 0 only to shared
                # Convert to list so we can pop items
                partition_0_only_list = list(partition_0_only_signatures)
                overflow_signatures = partition_0_only_list[-overflow_count:]  # Take last N items
                
                for sig in overflow_signatures:
                    partition_0_only_signatures.remove(sig)
                    shared_tile_signatures.add(sig)
                
                print(f"  After adjustment:")
                print(f"    Partition 0 only: {len(partition_0_only_signatures)} tile pairs")
                print(f"    Shared: {len(shared_tile_signatures)} tile pairs")
            else:
                print(f"  ERROR: Cannot fit overflow in shared section")
                print(f"  Need {overflow_count} pairs, but only {available_shared_space} available")
                print(f"  Total needed: {len(partition_0_only_signatures) + len(shared_tile_signatures)} pairs")
                print(f"  Total available in partition 0 (0-1023): {MAX_PARTITION_0_ONLY_PAIRS + MAX_SHARED_PAIRS} pairs")
                raise ValueError(f"Too many partition 0 tiles. Reduce image complexity.")
        
        # Check if we actually need partition splitting
        total_unique_tiles_needed = (len(partition_0_only_signatures) + len(shared_tile_signatures) + len(partition_1_only_signatures)) * 2  # *2 for tile pairs
        if total_unique_tiles_needed < MAX_TILES_PER_PARTITION:
            print(f"Total tiles needed ({total_unique_tiles_needed}) < {MAX_TILES_PER_PARTITION}, no partition splitting required")
            partition_1_scanlines = set()  # Clear partition 1 requirement
            partition_1_only_signatures = set()
            shared_tile_signatures = set()
        
        # PHASE 3: Generate optimized tile layout
        print("=== PHASE 3: Generating optimized tile layout ===")
        print("Tile layout strategy:")
        print("  Tiles 0-767:    Partition 0 only (384 tile pairs max)")
        print("  Tiles 768-1023: Shared (accessible by both partitions, 128 tile pairs max)")
        print("  Tiles 1024+:    Partition 1 only (1024 tile pairs max)")
        
        tiles_data = []
        tilemap_data = []
        
        # Track which tiles are in which partition for HDMA switching
        tile_partition_map = {}  # Maps tile number to partition (0 or 1)
        tile_storage_map = {}    # Maps tile number to storage index in tiles_data
        partition_0_tiles = []   # Tiles in partition 0 (0-1023)
        partition_1_tiles = []   # Tiles in partition 1 (1024+)
        
        # Track which scanlines use which partition
        scanline_partition_usage = {}  # Maps scanline to set of partitions used
        
        # Track which tiles are used in which partitions for overlap analysis
        tile_partition_usage = {}  # Maps tile_number to set of partitions used
        
        # Create signature to tile number mapping
        signature_to_tile_number = {}
        tile_index = 0
        
        # Place partition 0 only tiles first (including EMPTY if it's partition 0 only)
        # These must stay in range 0-767 to avoid overlapping with shared tiles at 768-1023
        print("Placing partition 0 only tiles...")
        MAX_PARTITION_0_ONLY_TILE = 768  # Partition 0 only tiles must be < 768
        
        for signature in partition_0_only_signatures:
            # Check if we would overflow into the shared tile area
            # This should never happen now because we pre-adjust in PHASE 2, but keep as sanity check
            if tile_index >= MAX_PARTITION_0_ONLY_TILE:
                print(f"INTERNAL ERROR: Partition 0 only tiles overflow into shared area!")
                print(f"  Current tile_index: {tile_index}, max allowed: {MAX_PARTITION_0_ONLY_TILE - 2}")
                print(f"  This should have been handled in PHASE 2 overflow adjustment.")
                print(f"  Partition 0 only signatures: {len(partition_0_only_signatures)}")
                raise ValueError(f"Internal error: Partition 0 only tiles exceed available space (0-767)")
                
            tile_pair_data = unique_tile_pairs[signature]
            left_tile_data, right_tile_data = tile_pair_data
            
            # Track storage location for this tile (byte offset in tiles_data)
            tile_storage_map[tile_index] = len(tiles_data)
            
            # Add both tiles consecutively (64 bytes total: 2 tiles * 32 bytes each)
            tiles_data.extend(left_tile_data)   # Left tile (32 bytes)
            tiles_data.extend(right_tile_data)  # Right tile (32 bytes)
                        
            signature_to_tile_number[signature] = tile_index
            tile_partition_map[tile_index] = 0
            partition_0_tiles.append(tile_index)
            
            tile_index += 2  # Increment by 2 since we added 2 tiles
        
        print(f"Partition 0 only tiles placed: 0-{tile_index - 2} ({tile_index} tiles total)")
        
        # Place shared tiles at the end of partition 0 (tiles 768-1023)
        # This allows them to be accessed by both partitions with different tilemap indices
        print("Placing shared tiles at end of partition 0 (tiles 768-1023)...")
        shared_tile_start_index = 768  # Start of shared tile area (FIXED position for HDMA)
        current_shared_index = shared_tile_start_index
        
        for signature in shared_tile_signatures:
            # Check if we would overflow past the shared tile area (768-1023 = 256 tiles)
            # This should never happen now because we pre-check in PHASE 2, but keep as sanity check
            if current_shared_index >= MAX_TILES_PER_PARTITION:
                print(f"INTERNAL ERROR: Shared tiles overflow past tile 1023!")
                print(f"  Current shared_index: {current_shared_index}, max allowed: {MAX_TILES_PER_PARTITION - 2}")
                print(f"  Shared area can only hold {(MAX_TILES_PER_PARTITION - shared_tile_start_index) // 2} tile pairs (256 tiles)")
                print(f"  This should have been caught in PHASE 2 overflow check.")
                print(f"  Shared signatures: {len(shared_tile_signatures)}")
                raise ValueError(f"Internal error: Too many shared tiles. Shared area (768-1023) can only hold 256 tiles")
                
            tile_pair_data = unique_tile_pairs[signature]
            left_tile_data, right_tile_data = tile_pair_data
            
            # Track storage location for this tile (byte offset in tiles_data)
            tile_storage_map[current_shared_index] = len(tiles_data)
            
            # Add both tiles consecutively (64 bytes total: 2 tiles * 32 bytes each)
            # Note: Even for EMPTY tiles, we duplicate the data to maintain consistency
            # in VRAM layout. Trying to reuse the tile 0 empty data causes indexing issues.
            tiles_data.extend(left_tile_data)   # Left tile (32 bytes)
            tiles_data.extend(right_tile_data)  # Right tile (32 bytes)
            
            signature_to_tile_number[signature] = current_shared_index
            tile_partition_map[current_shared_index] = 0  # Shared tiles are in partition 0
            partition_0_tiles.append(current_shared_index)
            
            tile_index += 2  # Increment by 2 since we added 2 tiles
            current_shared_index += 2  # Increment shared tile index by 2
        
        print(f"Shared tiles placed: {shared_tile_start_index}-{current_shared_index - 2} ({current_shared_index - shared_tile_start_index} tiles total)")
        
        # Place partition 1 only tiles in partition 1 area (1024+)
        print("Placing partition 1 only tiles...")
        partition_1_tile_index = MAX_TILES_PER_PARTITION  # Start at tile 1024
        
        for signature in partition_1_only_signatures:
            tile_pair_data = unique_tile_pairs[signature]
            left_tile_data, right_tile_data = tile_pair_data
            
            # Track storage location for this tile (byte offset in tiles_data)
            tile_storage_map[partition_1_tile_index] = len(tiles_data)
            
            # Add both tiles consecutively (64 bytes total: 2 tiles * 32 bytes each)
            tiles_data.extend(left_tile_data)   # Left tile (32 bytes)
            tiles_data.extend(right_tile_data)  # Right tile (32 bytes)
            
            signature_to_tile_number[signature] = partition_1_tile_index
            tile_partition_map[partition_1_tile_index] = 1
            partition_1_tiles.append(partition_1_tile_index)
            
            tile_index += 2  # Increment physical storage index by 2 since we added 2 tiles
            partition_1_tile_index += 2  # Increment partition 1 tile index by 2
        
        print(f"Tile layout complete:")
        print(f"  Partition 0 only: {len(partition_0_only_signatures)} tile pairs")
        print(f"  Shared tiles: {len(shared_tile_signatures)} tile pairs (placed at tiles 768-1023)")
        print(f"  Partition 1 only: {len(partition_1_only_signatures)} tile pairs")
        
        # PHASE 4: Generate tilemap with correct indices
        print("=== PHASE 4: Generating tilemap with optimized indices ===")
        
        # First, recalculate which scanlines need which partition based on ACTUAL tile placement
        # This is critical because PHASE 2's simulation may not match the actual placement in PHASE 3
        actual_partition_0_scanlines = set()
        actual_partition_1_scanlines = set()
        
        for entry in tilemap_entries:
            signature = entry['tile_pair_signature']
            scanline = entry['scanline']
            
            # Get the ACTUAL tile number assigned in PHASE 3
            actual_tile_number = signature_to_tile_number[signature]
            
            # Determine partition based on actual tile position
            if actual_tile_number < MAX_TILES_PER_PARTITION:
                # Tile is in partition 0 range (0-1023)
                actual_partition_0_scanlines.add(scanline)
            else:
                # Tile is in partition 1 range (1024+)
                actual_partition_1_scanlines.add(scanline)
        
        print(f"Scanlines requiring partition 0 (based on actual tile placement): {sorted(actual_partition_0_scanlines)}")
        print(f"Scanlines requiring partition 1 (based on actual tile placement): {sorted(actual_partition_1_scanlines)}")
        
        # Generate tilemap entries using the actual tile positions
        for entry in tilemap_entries:
            tilemap_x = entry['tilemap_x']
            tilemap_y = entry['tilemap_y']
            scanline = entry['scanline']
            signature = entry['tile_pair_signature']
            
            # Get the tile number for this signature (from PHASE 3 placement)
            tile_number = signature_to_tile_number[signature]
            
            # Determine which partition this scanline should use based on ACTUAL tile placement
            if scanline in actual_partition_0_scanlines and scanline not in actual_partition_1_scanlines:
                # Scanline only uses partition 0 tiles
                adjusted_tile_number = tile_number
                current_partition = 0
            elif scanline in actual_partition_1_scanlines and scanline not in actual_partition_0_scanlines:
                # Scanline only uses partition 1 tiles
                if tile_number >= 768 and tile_number < MAX_TILES_PER_PARTITION:
                    # Shared tile (768-1023): When partition 1 is active (offset 0x6000 / 0x300 word address),
                    # tile 768 in VRAM becomes tilemap index 0 (768 - 768 = 0)
                    # tile 1023 in VRAM becomes tilemap index 255 (1023 - 768 = 255)
                    adjusted_tile_number = tile_number - 768  # Subtract 768 to get partition 1 index
                else:
                    # Partition 1 only tile (1024+): apply 0x300 offset
                    # Tile 1024+ becomes tilemap index (tile_number - 768)
                    adjusted_tile_number = tile_number - 0x300  # Subtract 768 (0x300)
                current_partition = 1
            else:
                # Scanline uses tiles from both partitions - use the partition matching this tile
                if tile_number < 768:
                    # Partition 0 only tile (0-767): no offset needed
                    adjusted_tile_number = tile_number
                    current_partition = 0
                elif tile_number >= 768 and tile_number < MAX_TILES_PER_PARTITION:
                    # Shared tile (768-1023): use partition 1 offset when partition 1 is active
                    # This handles tiles that were moved from partition 0 only to shared due to overflow
                    if scanline in actual_partition_1_scanlines:
                        adjusted_tile_number = tile_number - 768  # Subtract 768 for partition 1
                        current_partition = 1
                    else:
                        adjusted_tile_number = tile_number  # No offset for partition 0
                        current_partition = 0
                else:
                    # Partition 1 only tile (1024+): apply 0x300 offset
                    adjusted_tile_number = tile_number - 0x300
                    current_partition = 1
                
            # Create SNES Mode 5 tilemap entry (16-bit)
            # Format: vhoppptt tttttttt
            tilemap_entry = adjusted_tile_number & 0x3FF  # Lower 10 bits: tile number
            tilemap_entry |= (7 << 10)  # Palette 7 (bits 10-12) for 4bpp tiles
            tilemap_entry |= (0 << 13)  # Priority 0 (bit 13)
            tilemap_entry |= (0 << 14)  # Horizontal flip 0 (bit 14)
            tilemap_entry |= (0 << 15)  # Vertical flip 0 (bit 15)
            
            # Track which scanline uses which partition
            if scanline not in scanline_partition_usage:
                scanline_partition_usage[scanline] = set()
            scanline_partition_usage[scanline].add(current_partition)
            
            # Track which tiles are used in which partitions
            if tile_number not in tile_partition_usage:
                tile_partition_usage[tile_number] = set()
            tile_partition_usage[tile_number].add(current_partition)
            
            tilemap_data.append(tilemap_entry)
        
        # Validate that palette indices are within the 4bpp range (0-15)
        if tile_processing_indices:
            max_palette_index = max(tile_processing_indices)
            min_palette_index = min(tile_processing_indices)
            print(f"Palette index range used: {min_palette_index} to {max_palette_index}")
            if max_palette_index >= 16:
                print(f"WARNING: Palette index {max_palette_index} >= 16 (4bpp uses 0-15)!")
            if min_palette_index < 0:
                print(f"WARNING: Negative palette index {min_palette_index}!")
            
            if len(tile_processing_indices) > 16:
                print(f"WARNING: Using {len(tile_processing_indices)} unique palette indices, but 4bpp supports only 16!")
            else:
                print(f"Using {len(tile_processing_indices)} palette indices within 4bpp range (0-15)")
        
        # Calculate total unique tiles (each pair counts as 2 tiles)
        # tile_index represents the next available tile number, so total tiles = tile_index
        total_unique_tiles = tile_index
        
        print(f"Bitplane tile data: {len(tiles_data)} bytes generated from {len(unique_tile_pairs)} unique tile pairs")
        print(f"Generated {total_unique_tiles} unique tiles from {self.tilemap_tiles_x * self.tilemap_tiles_y} tilemap entries")
        print(f"Mode 5 tilemap entries: {len(tilemap_data)} (each refers to 2 consecutive tiles)")
        
        # Print partition statistics
        print(f"Partition 0 tiles: {len(partition_0_tiles)} (0-{MAX_TILES_PER_PARTITION-1})")
        print(f"Partition 1 tiles: {len(partition_1_tiles)} ({MAX_TILES_PER_PARTITION}+)")
        print(f"Scanlines using partition 0: {len([s for s in scanline_partition_usage if 0 in scanline_partition_usage[s]])}")
        print(f"Scanlines using partition 1: {len([s for s in scanline_partition_usage if 1 in scanline_partition_usage[s]])}")
        
        # Print optimization statistics
        print(f"Optimization results:")
        print(f"  Shared tiles placed at end of partition 0: {len(shared_tile_signatures)} tile pairs")
        print(f"  Shared tiles accessible by both partitions with different tilemap indices")
        print(f"  This reduces HDMA switching requirements and improves performance")
        
        return tiles_data, tilemap_data, total_unique_tiles, self.tilemap_tiles_x, self.tilemap_tiles_y, tile_partition_map, partition_0_tiles, partition_1_tiles, scanline_partition_usage, tile_storage_map, tile_partition_usage
    
    def create_4bpp_preview(self, img_quantized, palette_colors, output_path):
        """Create a 4bpp preview image for Mode 5"""
        print("Creating 4bpp preview for Mode 5...")
        
        # The img_quantized is already the correct 512x224 size for Mode 5
        # Just save it as-is to show the stretched image
        print(f"Preview dimensions: {img_quantized.width}x{img_quantized.height} (Mode 5 stretched)")
        preview_img = img_quantized  # Use the quantized image directly
        
        preview_path = output_path.replace('.h', '_4bpp_preview.png')
        preview_img.save(preview_path)
        print(f"4bpp preview saved to {preview_path}")
        return preview_path
    
    def create_palette_preview(self, palette_colors, output_path):
        """Create a palette preview showing all colors"""
        print("Creating palette preview...")
        
        # Create a grid image showing all colors (16x16 for 256 colors)
        colors_per_row = 16
        color_size = 10
        palette_img = Image.new('RGB', (colors_per_row * color_size, colors_per_row * color_size))
        palette_pixels = palette_img.load()
        
        for i, color in enumerate(palette_colors):
            if i >= len(palette_colors):
                break
            row = i // colors_per_row
            col = i % colors_per_row
            for x in range(color_size):
                for y in range(color_size):
                    palette_pixels[col * color_size + x, row * color_size + y] = color
        
        palette_path = output_path.replace('.h', '_palette.png')
        palette_img.save(palette_path)
        print(f"Palette preview saved to {palette_path}")
        return palette_path

    def generate_hdma_effect_code(self, f, scanline_palettes, tiles_data, tilemap_data, tile_partition_map=None, partition_0_tiles=None, partition_1_tiles=None, scanline_partition_usage=None, tile_storage_map=None, tile_partition_usage=None):
        """Generate HDMA effect code for tilemap base address updates (Mode 5 tile limit workaround)"""
        # For Mode 5, we need HDMA to update BG1 tilemap base address
        # This allows us to use more than 1024 tiles by switching tile partitions mid-render
        
        f.write("/* HDMA Tables for Mode 5 Tile Limit Workaround */\n")
        f.write("/* This allows using more than 1024 tiles by updating tilemap base address */\n\n")
        
        if tile_partition_map is None or not partition_1_tiles:
            f.write("/* No HDMA data generated - tile limit not exceeded */\n")
            return
        
        # Find scanlines where partition changes are needed
        # Each tilemap row is 8 scanlines tall, so we need to align switches to tile boundaries
        hdma_entries = []
        
        # Analyze scanline partition usage to find where switches are needed
        current_partition = 0  # Start with partition 0
        current_scanline = 0
        
        # Process each scanline and add HDMA entries when partition changes
        for scanline in range(0, self.screen_height, 8):  # Check every 8 scanlines (tile boundaries)
            # Determine which partition this scanline should use
            if scanline in scanline_partition_usage:
                # This scanline has tiles, check which partition they use
                partitions_used = scanline_partition_usage[scanline]
                if 1 in partitions_used and 0 not in partitions_used:
                    # This scanline only uses partition 1
                    target_partition = 1
                elif 0 in partitions_used and 1 not in partitions_used:
                    # This scanline only uses partition 0
                    target_partition = 0
                else:
                    # This scanline uses both partitions - switch to partition 1
                    # since partition 1 contains the higher tile numbers that are needed
                    target_partition = 1
            else:
                # This scanline has no tiles, keep current partition
                target_partition = current_partition
            
            # If partition changed, add HDMA entry
            if target_partition != current_partition:
                # Switch to partition 1 one scanline early to ensure proper HDMA timing
                switch_scanline = scanline - 1 if target_partition == 1 and scanline > 0 else scanline
                
                # Calculate scanline count for current partition
                scanline_count = switch_scanline - current_scanline
                if scanline_count > 0:
                    hdma_entries.append((current_scanline, scanline_count, current_partition))
                
                # Switch to new partition
                current_partition = target_partition
                current_scanline = switch_scanline
        
        # Add final entry for remaining scanlines
        if current_scanline < self.screen_height:
            final_scanline_count = self.screen_height - current_scanline
            hdma_entries.append((current_scanline, final_scanline_count, current_partition))
        
        # Generate HDMA table based on partition usage
        
        # Analyze shared tiles between partitions
        shared_tiles = []
        for tile_number, partitions in tile_partition_usage.items():
            if len(partitions) > 1:
                shared_tiles.append(tile_number)
        
        print(f"Found {len(shared_tiles)} tiles used in both partitions")
        if shared_tiles:
            print(f"Shared tiles: {sorted(shared_tiles)[:10]}{'...' if len(shared_tiles) > 10 else ''}")
        
        f.write(f"/* HDMA Table for BG1 Tilemap Base Address Updates */\n")
        f.write(f"/* Format: [count, bg12nba_value] for each partition change */\n")
        f.write(f"/* BG12NBA values: 0x00 = tiles at 0x0000, 0x03 = tiles at 0x6000 */\n")
        f.write(f"/* BG12NBA uses word addresses: 0x6000 byte = 0x3000 word = 0x03 in register */\n")
        f.write(f"/* Partition 0: tilemap indices 0-1023 refereCannce tiles 0-1023 */\n")
        f.write(f"/* Partition 1: tilemap indices 0-241 reference tiles 768-1009 (0x300 offset) */\n")
        f.write(f"/* HDMA table must end with 0x00 to terminate */\n")
        
        # Generate HDMA table based on partition usage
        
        # Calculate total HDMA entries needed (including chunks)
        total_hdma_entries = 0
        for start_scanline, scanline_count, partition in hdma_entries:
            chunks_needed = (scanline_count + 127) // 128  # Round up division
            total_hdma_entries += chunks_needed
        
        f.write(f"const unsigned char hdma_bg12nba_table[{total_hdma_entries * 2 + 2}] = {{\n")
        
        # Generate HDMA table entries only for scanlines that need partition changes
        for start_scanline, scanline_count, partition in hdma_entries:
            if partition == 0:
                bg12nba_value = 0x60  # BG1 at 0x0000, BG2 at 0xC000 (word address 0x6000)
            else:
                bg12nba_value = 0x63  # BG1 at 0x3000, BG2 at 0xC000 (word address 0x6000)
            
            # For HDMA, we can use a single entry for the entire range if it's <= 128 scanlines
            # Only break into chunks if absolutely necessary (over 128 scanlines)
            if scanline_count <= 128:
                end_scanline = start_scanline + scanline_count - 1
                f.write(f"    0x{scanline_count:02X}, 0x{bg12nba_value:02X},  // Scanlines {start_scanline}-{end_scanline}: Tiles at 0x{partition * 0x6000:04X} (count={scanline_count}, 0x{scanline_count:02X})\n")
            else:
                # For entries over 128 scanlines, we need to break them into chunks
                # But we should try to minimize the number of chunks
                remaining_scanlines = scanline_count
                current_scanline = start_scanline
                
                while remaining_scanlines > 0:
                    # Use the maximum chunk size (128) for all but the last chunk
                    if remaining_scanlines > 128:
                        chunk_size = 128
                    else:
                        chunk_size = remaining_scanlines
                    
                    end_scanline = current_scanline + chunk_size - 1
                    f.write(f"    0x{chunk_size:02X}, 0x{bg12nba_value:02X},  // Scanlines {current_scanline}-{end_scanline}: Tiles at 0x{partition * 0x6000:04X} (count={chunk_size}, 0x{chunk_size:02X})\n")
                    current_scanline += chunk_size
                    remaining_scanlines -= chunk_size
        
        # Add terminator entry
        f.write(f"    0x00, 0x00,  // Terminator entry (required for HDMA)\n")
        f.write("};\n\n")
        
        # Add tile partition information
        # Count actual 8x8 tiles in each partition
        partition_0_count = len([t for t in tile_partition_map.values() if t == 0]) * 2  # Each tile pair = 2 tiles
        partition_1_count = len([t for t in tile_partition_map.values() if t == 1]) * 2  # Each tile pair = 2 tiles
        
        f.write("/* Tile Partition Information */\n")
        f.write(f"#define TILES_IN_PARTITION_0 {partition_0_count}\n")
        f.write(f"#define TILES_IN_PARTITION_1 {partition_1_count}\n")
        f.write(f"#define TOTAL_TILES {len(tile_partition_map) * 2 if tile_partition_map else 0}\n")
        f.write(f"#define MAX_TILES_PER_PARTITION 1024\n")
        f.write(f"#define HDMA_ENTRIES {len(hdma_entries)}\n\n")
        
        # Generate separate tile arrays for each partition
        f.write("/* Partition 0 Tiles (0-1023) - Load at VRAM base address 0x0000 */\n")
        f.write(f"const unsigned char bg_tiles_partition_0[] = {{\n")
        
        # Extract partition 0 tiles from the main tiles_data array
        # Partition 0 contains tiles 0-1023 (first 1024 tiles)
        # Note: Tiles are stored in pairs (LEFT, RIGHT) in tiles_data
        partition_0_tiles_data = []
        for tile_number in range(1024):  # Tiles 0-1023
            if tile_number % 2 == 0:
                # Even tile number (LEFT tile)
                if tile_number in tile_storage_map:
                    byte_offset = tile_storage_map[tile_number]  # Byte offset in tiles_data
                    partition_0_tiles_data.extend(tiles_data[byte_offset:byte_offset + 32])
                else:
                    # Fill with empty tile data for unused tile numbers
                    partition_0_tiles_data.extend([0] * 32)
            else:
                # Odd tile number (RIGHT tile)
                left_tile_number = tile_number - 1
                if left_tile_number in tile_storage_map:
                    # RIGHT tile is 32 bytes after the LEFT tile
                    byte_offset = tile_storage_map[left_tile_number] + 32
                    partition_0_tiles_data.extend(tiles_data[byte_offset:byte_offset + 32])
                else:
                    # Fill with empty tile data for unused tile numbers
                    partition_0_tiles_data.extend([0] * 32)
        
        # Write partition 0 tile data
        for i in range(0, len(partition_0_tiles_data), 16):
            f.write("    ")
            for j in range(16):
                if i + j < len(partition_0_tiles_data):
                    f.write(f"0x{partition_0_tiles_data[i + j]:02X}")
                    if i + j < len(partition_0_tiles_data) - 1:
                        f.write(", ")
            f.write("\n")
        
        f.write("};\n\n")
        
        f.write("/* Partition 1 Tiles (1024+) - Load at VRAM base address 0x6000 (overlapping) */\n")
        f.write("const unsigned char bg_tiles_partition_1[] = {\n")
        
        # Extract partition 1 tiles from the main tiles_data array
        # Partition 1 contains tiles 1024+ (remaining tiles)
        # Note: Tiles are stored in pairs (LEFT, RIGHT) in tiles_data
        partition_1_tiles_data = []
        for tile_number in range(1024, 2048):  # Tiles 1024-2047
            if tile_number % 2 == 0:
                # Even tile number (LEFT tile)
                if tile_number in tile_storage_map:
                    byte_offset = tile_storage_map[tile_number]  # Byte offset in tiles_data
                    partition_1_tiles_data.extend(tiles_data[byte_offset:byte_offset + 32])
                else:
                    # Fill with empty tile data for unused tile numbers
                    partition_1_tiles_data.extend([0] * 32)
            else:
                # Odd tile number (RIGHT tile)
                left_tile_number = tile_number - 1
                if left_tile_number in tile_storage_map:
                    # RIGHT tile is 32 bytes after the LEFT tile
                    byte_offset = tile_storage_map[left_tile_number] + 32
                    partition_1_tiles_data.extend(tiles_data[byte_offset:byte_offset + 32])
                else:
                    # Fill with empty tile data for unused tile numbers
                    partition_1_tiles_data.extend([0] * 32)
        
        # Write partition 1 tile data
        for i in range(0, len(partition_1_tiles_data), 16):
            f.write("    ")
            for j in range(16):
                if i + j < len(partition_1_tiles_data):
                    f.write(f"0x{partition_1_tiles_data[i + j]:02X}")
                    if i + j < len(partition_1_tiles_data) - 1:
                        f.write(", ")
            f.write("\n")
        
        f.write("};\n\n")
        
        # HDMA setup function declaration
        f.write("/* HDMA Setup Function Declaration for Mode 5 Tile Limit Workaround */\n")
        f.write("/* Implementation should be in your main C file */\n")
        f.write("void setup_hdma_bg1_tilemap(void);\n\n")

    def generate_header_file(self, output_path, palette_colors, tiles_data, tilemap_data, num_tiles, quality_metrics=None, actual_tiles_x=None, actual_tiles_y=None, scanline_palettes=None, tile_partition_map=None, partition_0_tiles=None, partition_1_tiles=None, scanline_partition_usage=None, tile_storage_map=None, tile_partition_usage=None):
        """Generate C header file with all the data for vbcc65816"""
        with open(output_path, 'w') as f:
            f.write("/* Generated by ConvertImageMode5.py - SNES Mode 5 BG1 Data */\n")
            f.write("/* Compiler: vbcc65816 */\n")
            f.write(f"/* Image: {os.path.basename(INPUT_IMAGE_PATH)} */\n")
            f.write(f"/* Resolution: 512x224 pixels (Mode 5 double horizontal resolution) */\n")
            f.write(f"/* Tiles: {num_tiles}, Tilemap: {len(tilemap_data)} entries (each refers to 2 tiles) */\n")
            if scanline_palettes:
                total_unique = len(set().union(*[set(p) for p in scanline_palettes]))
                f.write(f"/* HDMA Effect: {len(scanline_palettes)} scanlines with optimized palettes */\n")
                f.write(f"/* Total unique colors: {total_unique} (16 per scanline) */\n")
            if quality_metrics:
                mse, psnr, ssim = quality_metrics
                f.write(f"/* Quality: MSE={mse:.2f}, PSNR={psnr:.2f}dB, SSIM={ssim:.4f} */\n")
            f.write("\n")
            
            # Generate HDMA effect code for tile partition switching
            self.generate_hdma_effect_code(f, scanline_palettes, tiles_data, tilemap_data, tile_partition_map, partition_0_tiles, partition_1_tiles, scanline_partition_usage, tile_storage_map, tile_partition_usage)
            
            # Regular 4bpp palette
            f.write("/* 4bpp Palette - 16 colors */\n")
            f.write("const unsigned short bg_palette[16] = {\n")
            for i, color in enumerate(palette_colors):
                snes_color = self.rgb_to_snes_color(color)
                f.write(f"    0x{snes_color:04X}")
                if i < 15:
                    f.write(",")
                f.write(f"  // Color {i}: RGB({color[0]}, {color[1]}, {color[2]})\n")
            f.write("};\n\n")
            
            # Tile data is now generated as separate partition arrays above
            
            # Tilemap data
            f.write(f"/* Mode 5 Tilemap - {len(tilemap_data)} entries ({actual_tiles_x or self.tilemap_tiles_x}x{actual_tiles_y or self.tilemap_tiles_y} entries) */\n")
            f.write("/* Each tilemap entry refers to 2 adjacent 8x8 tiles (16x8 effective) */\n")
            f.write("const unsigned short bg_tilemap[] = {\n")
            for i in range(0, len(tilemap_data), 16):
                f.write("    ")
                for j in range(16):
                    if i + j < len(tilemap_data):
                        f.write(f"0x{tilemap_data[i + j]:04X}")
                        if i + j < len(tilemap_data) - 1:
                            f.write(", ")
                f.write("\n")
            f.write("};\n\n")
            
            # Constants
            f.write("/* Constants for easy access */\n")
            f.write(f"#define BG_TILES_COUNT {num_tiles}\n")
            f.write(f"#define BG_TILEMAP_WIDTH {actual_tiles_x or self.tilemap_tiles_x}\n")
            f.write(f"#define BG_TILEMAP_HEIGHT {actual_tiles_y or self.tilemap_tiles_y}\n")
            f.write(f"#define BG_TILES_SIZE {len(tiles_data)}\n")
            f.write(f"#define BG_TILEMAP_SIZE {len(tilemap_data)}\n")
            f.write("#define BG_SCREEN_WIDTH 512\n")
            f.write("#define BG_SCREEN_HEIGHT 224\n")
            f.write("#define BG_TILE_WIDTH 8\n")
            f.write("#define BG_TILE_HEIGHT 8\n")
            f.write("#define BG_TILES_PER_ENTRY 2\n")  # Mode 5 specific
            f.write("#define BG_PALETTE_SIZE 16\n")
    
    def convert(self):
        """Main conversion function with regular 4bpp palette for Mode 5"""
        input_path = INPUT_IMAGE_PATH
        output_path = OUTPUT_HEADER_PATH
        
        print(f"Converting {input_path} to SNES Mode 5 BG1 format with regular 4bpp palette...")
        
        # Use the new pipeline method
        return self.convert_image_pipeline(input_path, output_path)

def main():
    """Main function"""
    import sys
    global INPUT_IMAGE_PATH
    
    # Check for command line argument
    if len(sys.argv) > 1:
        input_image = sys.argv[1]
    else:
        # Default to bg_001.png if no argument provided (for debugging)
        input_image = os.path.join(os.path.dirname(__file__), "bg_001.png")
    
    if not os.path.exists(input_image):
        print(f"Error: {input_image} not found")
        print(f"Usage: python ConvertImageMode5.py <image_file.png>")
        print(f"Example: python ConvertImageMode5.py bg_001.png")
        return 1
    
    # Update the global input path
    INPUT_IMAGE_PATH = input_image
    
    converter = SNESMode5Converter()
    success = converter.convert()
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
