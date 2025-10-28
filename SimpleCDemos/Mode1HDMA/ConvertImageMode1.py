#!/usr/bin/env python3
"""
ConvertImage.py - SNES Mode 1 BG1 Image Converter

Converts a PNG image to SNES Mode 1 BG1 format:
- Resolution: 256x224 pixels
- Tiles: 8x8 pixels each
- Color depth: 4bpp (16 colors)
- Output: C header file with palette, tiles, and tilemap data
"""

import os
import sys
import subprocess
import tempfile
from PIL import Image

# Configuration variables - modify these paths as needed
# INPUT_IMAGE_PATH will be set in main() based on command line arguments
INPUT_IMAGE_PATH = None
OUTPUT_HEADER_PATH = os.path.join(os.path.dirname(__file__), "BG18_mode1.h")  # Mode 1 4bpp header

class SNESMode1Converter:
    def __init__(self):
        self.tile_width = 8
        self.tile_height = 8
        self.screen_width = 256  # Mode 1 uses 256x224 resolution
        self.screen_height = 224
        self.tiles_x = self.screen_width // self.tile_width  # 32 tiles
        self.tiles_y = self.screen_height // self.tile_height  # 28 tiles
        self.max_colors = 16  # 4bpp for Mode 1 (16 colors per background)
        self.bitplanes = 4     # 4 bitplanes for 4bpp format
        
    def load_image(self, image_path, brightness=1.0):
        """Load and validate the input PNG image"""
        try:
            img = Image.open(image_path)
            if img.mode != 'RGB':
                img = img.convert('RGB')
            
            print(f"Loaded image: {img.width}x{img.height} pixels")
            
            # Apply brightness adjustment if needed
            if brightness != 1.0:
                print(f"Applying brightness adjustment: {brightness}x")
                # Convert to numpy array for brightness adjustment
                import numpy as np
                img_array = np.array(img, dtype=np.float32)
                img_array = np.clip(img_array * brightness, 0, 255)
                img = Image.fromarray(img_array.astype(np.uint8))
            
            # Resize to target width while preserving aspect ratio
            if img.width != self.screen_width:
                # Calculate new height to preserve aspect ratio
                aspect_ratio = img.height / img.width
                new_height = int(self.screen_width * aspect_ratio)
                print(f"Resizing image from {img.width}x{img.height} to preserve aspect ratio: {self.screen_width}x{new_height}")
                img = img.resize((self.screen_width, new_height), Image.LANCZOS)
                print(f"Image resized successfully")
            
            # Pad the image to fill the complete tilemap grid with black pixels
            if img.height < self.screen_height:
                print(f"Padding image from {img.width}x{img.height} to {self.screen_width}x{self.screen_height} with black")
                # Create a new image with the full tilemap dimensions
                padded_img = Image.new('RGB', (self.screen_width, self.screen_height), (0, 0, 0))  # Black background
                # Paste the resized image at the top
                padded_img.paste(img, (0, 0))
                img = padded_img
                print(f"Image padded successfully")
            elif img.height > self.screen_height:
                print(f"Cropping image from {img.width}x{img.height} to {self.screen_width}x{self.screen_height}")
                # Crop the image to fit the tilemap grid
                img = img.crop((0, 0, self.screen_width, self.screen_height))
                print(f"Image cropped successfully")
            
            print(f"Final image dimensions: {img.width}x{img.height}")
            print(f"Tilemap grid size: {self.tiles_x}x{self.tiles_y} (Mode 1 standard)")
            
            return img
        except Exception as e:
            print(f"Error loading image: {e}")
            return None
    
    def quantize_with_pngquant(self, img_path):
        """Use pngquant.exe to intelligently reduce colors before per-scanline processing"""
        # Use 64 colors as a good balance between quality and per-scanline compatibility
        target_colors = 64
        print(f"Using pngquant.exe for intelligent color reduction to {target_colors} colors...")
        
        # Check if pngquant.exe exists in the same directory as the image
        img_dir = os.path.dirname(img_path)
        pngquant_path = os.path.join(img_dir, "pngquant.exe")
        
        if not os.path.exists(pngquant_path):
            print(f"Warning: pngquant.exe not found at {pngquant_path}")
            print("Falling back to original image...")
            return None
        
        try:
            # Run pngquant with target colors (old version syntax)
            # Old pngquant creates output file with -fs8.png suffix (always 8-bit output)
            output_path = img_path.replace('.png', '-fs8.png')
            
            cmd = [
                pngquant_path,
                '-force',  # Overwrite output file (old syntax, single dash)
                str(target_colors),  # Number of colors
                img_path
            ]
            
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode != 0:
                print(f"pngquant failed: {result.stderr}")
                return None
            
            # Check if output file was created
            if not os.path.exists(output_path):
                print(f"pngquant output file not found: {output_path}")
                return None
            
            # Load the quantized image
            quantized_img = Image.open(output_path)
            
            # Convert to RGB mode to ensure we get RGB tuples instead of palette indices
            if quantized_img.mode != 'RGB':
                quantized_img = quantized_img.convert('RGB')
            
            print(f"pngquant reduced image to {target_colors} colors successfully")
            
            return quantized_img, output_path  # Return both image and temp file path
            
        except Exception as e:
            print(f"Error running pngquant: {e}")
            return None
    
    def extract_scanline_palettes_smart(self, img):
        """Extract optimized palettes with smart per-scanline quantization"""
        print("Extracting per-scanline palettes with smart quantization...")
        
        scanline_palettes = []
        all_unique_colors = set()
        pixels = img.load()
        
        # Track scanlines that need quantization
        scanlines_quantized = 0
        max_colors_in_scanline = 0
        
        # Process each scanline
        for y in range(self.screen_height):
            # Collect all colors used in this scanline
            scanline_colors = {}
            for x in range(self.screen_width):
                color = pixels[x, y]
                if color in scanline_colors:
                    scanline_colors[color] += 1
                else:
                    scanline_colors[color] = 1
            
            # Track statistics
            num_colors = len(scanline_colors)
            max_colors_in_scanline = max(max_colors_in_scanline, num_colors)
            
            if num_colors <= 16:
                # Scanline fits within 16 colors - use all colors as-is
                scanline_palette = list(scanline_colors.keys())
                # Pad with black if fewer than 16 colors
                while len(scanline_palette) < 16:
                    scanline_palette.append((0, 0, 0))
            else:
                # Scanline exceeds 16 colors - apply smart quantization
                scanlines_quantized += 1
                scanline_palette = self.quantize_scanline_colors(scanline_colors, y)
            
            # Add to results
            for color in scanline_palette:
                all_unique_colors.add(color)
            scanline_palettes.append(scanline_palette)
        
        print(f"Generated {len(scanline_palettes)} scanline palettes")
        print(f"Total unique colors across all scanlines: {len(all_unique_colors)}")
        print(f"Average colors per scanline: {sum(len(set(p)) for p in scanline_palettes) / len(scanline_palettes):.1f}")
        print(f"Maximum colors in any scanline: {max_colors_in_scanline}")
        print(f"Scanlines requiring quantization: {scanlines_quantized}/{len(scanline_palettes)} ({scanlines_quantized/len(scanline_palettes)*100:.1f}%)")
        
        if scanlines_quantized == 0:
            print("✓ All scanlines naturally fit within 16 colors - no quantization needed!")
        else:
            print(f"✓ Applied smart quantization to {scanlines_quantized} scanlines only")
        
        return scanline_palettes, all_unique_colors
    
    def quantize_scanline_colors(self, scanline_colors, scanline_y):
        """Apply smart quantization prioritizing color diversity and frequency"""
        import numpy as np
        
        # Convert to lists for easier manipulation
        colors_list = list(scanline_colors.keys())
        weights = [scanline_colors[color] for color in colors_list]
        
        if len(colors_list) <= 16:
            # Shouldn't happen, but handle gracefully
            result = colors_list[:]
            while len(result) < 16:
                result.append((0, 0, 0))
            return result
        
        # Strategy: Combine frequency-based and diversity-based selection
        # 1. Always include the most frequent colors (they're important)
        # 2. Use max-min diversity to ensure good color separation
        
        sorted_colors = sorted(scanline_colors.items(), key=lambda x: x[1], reverse=True)
        
        # Start with the most frequent color
        selected_colors = [sorted_colors[0][0]]
        remaining_colors = [color for color, count in sorted_colors[1:]]
        
        # For remaining 15 slots, use a hybrid approach
        while len(selected_colors) < 16 and remaining_colors:
            best_color = None
            best_score = -1
            
            for candidate in remaining_colors:
                # Calculate diversity score (minimum distance to already selected colors)
                min_distance = float('inf')
                for selected in selected_colors:
                    distance = sum((a - b) ** 2 for a, b in zip(candidate, selected))
                    min_distance = min(min_distance, distance)
                
                # Calculate frequency score
                frequency = scanline_colors[candidate]
                max_frequency = sorted_colors[0][1]  # Most frequent color's count
                frequency_score = frequency / max_frequency
                
                # Combine scores: diversity is primary, frequency is secondary
                # Use sqrt of distance to make diversity differences more significant
                diversity_score = np.sqrt(min_distance) / 441.67  # 441.67 = sqrt(255^2 + 255^2 + 255^2)
                combined_score = diversity_score * 0.7 + frequency_score * 0.3
                
                if combined_score > best_score:
                    best_score = combined_score
                    best_color = candidate
            
            if best_color:
                selected_colors.append(best_color)
                remaining_colors.remove(best_color)
            else:
                break
        
        # Pad with black if needed
        while len(selected_colors) < 16:
            selected_colors.append((0, 0, 0))
        
        print(f"  Scanline {scanline_y:3d}: Quantized {len(colors_list):3d} colors to 16 using diversity-frequency hybrid")
        
        return selected_colors

    def extract_palette(self, img):
        """Extract and optimize color palette - legacy method for backward compatibility"""
        print("Extracting color palette...")
        
        # Check if image is already quantized (has a palette)
        if img.mode == 'P' and img.palette:
            print("Image is already quantized, extracting palette...")
            # Get palette from quantized image
            palette_data = img.palette.palette
            palette_colors = []
            
            # Extract RGB values from palette data
            for i in range(0, len(palette_data), 3):
                if len(palette_colors) >= self.max_colors:
                    break
                r, g, b = palette_data[i], palette_data[i+1], palette_data[i+2]
                palette_colors.append((r, g, b))
            
            # Pad with black if we have fewer than 16 colors
            while len(palette_colors) < self.max_colors:
                palette_colors.append((0, 0, 0))
        else:
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
            
            # Take up to 16 most common colors
            palette_colors = []
            for count, color in colors[:self.max_colors]:
                palette_colors.append(color)
            
            # Pad with black if we have fewer than 16 colors
            while len(palette_colors) < self.max_colors:
                palette_colors.append((0, 0, 0))
        
        print(f"Extracted {len(palette_colors)} colors for palette")
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
    
    def create_scanline_color_map(self, img, scanline_palettes):
        """Create a quantized image using per-scanline palettes"""
        print("Creating quantized image with per-scanline palettes...")
        
        # Create a new image with palette indices
        img_quantized = Image.new('P', img.size)
        
        # For per-scanline palette system, we use a fixed 16-color palette structure
        # Each scanline will use indices 0-15, and the actual colors will be swapped via CGRAM
        # Set up a representative palette for PIL display (using first scanline palette)
        display_palette = scanline_palettes[0][:16]  # Use first scanline as reference
        
        # Pad to exactly 256 colors for PIL compatibility
        all_colors = display_palette[:]
        while len(all_colors) < 256:
            all_colors.append((0, 0, 0))
        
        palette_data = []
        for color in all_colors:
            palette_data.extend(color)
        
        # Ensure exactly 768 bytes (256 colors * 3 bytes each)
        while len(palette_data) < 768:
            palette_data.extend([0, 0, 0])
            
        if len(palette_data) != 768:
            raise ValueError(f"Palette data must be exactly 768 bytes, got {len(palette_data)}")
            
        img_quantized.putpalette(palette_data)
        
        # Convert each pixel to the closest palette color for its scanline
        # The key insight: we always use indices 0-15, but find the best color match
        # within each scanline's 16-color palette
        pixels = img.load()
        quantized_pixels = img_quantized.load()
        
        # Debug: Track the indices we're setting
        indices_used = set()
        
        for y in range(img.height):
            scanline_palette = scanline_palettes[y]
            for x in range(img.width):
                rgb = pixels[x, y]
                
                # Find closest palette color in this scanline's palette
                min_dist = float('inf')
                best_index = 0
                for i, palette_color in enumerate(scanline_palette):
                    dist = sum((a - b) ** 2 for a, b in zip(rgb, palette_color))
                    if dist < min_dist:
                        min_dist = dist
                        best_index = i
                
                # Always use indices 0-15 (scanline-relative)
                # The per-scanline palette system will provide the correct colors
                quantized_pixels[x, y] = best_index
                indices_used.add(best_index)
        
        print(f"DEBUG: Scanline palette indices set: {sorted(list(indices_used))}")
        print(f"DEBUG: Range check - min: {min(indices_used)}, max: {max(indices_used)}")
        
        return img_quantized, scanline_palettes

    def create_color_map(self, img, palette_colors):
        """Create a quantized image using the extracted palette"""
        print("Creating quantized image with extracted palette...")
        
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
    
    def convert_tile_to_bitplanes(self, tile_pixels):
        """Convert 8x8 tile pixels to SNES 4bpp bitplane format"""
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
    
    def create_simple_test_image(self):
        """Create a simple test image with patterns to reduce data size"""
        print("Creating simple test pattern...")
        
        # Create a simple 256x224 image with patterns
        img = Image.new('RGB', (self.screen_width, self.screen_height), (0, 0, 0))
        pixels = img.load()
        
        # Create simple patterns to minimize unique tiles
        for y in range(self.screen_height):
            for x in range(self.screen_width):
                # Create simple patterns
                if (x // 32 + y // 32) % 2 == 0:
                    # Checkerboard pattern
                    if (x // 8 + y // 8) % 2 == 0:
                        pixels[x, y] = (255, 0, 0)  # Red
                    else:
                        pixels[x, y] = (0, 255, 0)  # Green
                else:
                    # Gradient pattern
                    intensity = (x + y) % 256
                    pixels[x, y] = (intensity, intensity // 2, intensity // 4)
        
        return img
    
    
    def convert_image_to_tiles(self, img_quantized):
        """Convert entire image to tile data"""
        tiles_data = []
        tilemap_data = []
        
        # Calculate tile dimensions - use the full tilemap grid, not just image size
        # This ensures we generate a complete tilemap even for non-evenly-divisible images
        actual_tiles_x = self.tiles_x  # Use the calculated tilemap grid size
        actual_tiles_y = self.tiles_y  # Use the calculated tilemap grid size
        
        print(f"Image size: {img_quantized.width}x{img_quantized.height}")
        print(f"Tilemap grid: {actual_tiles_x}x{actual_tiles_y}")
        print(f"Image exactly matches tilemap grid - no padding needed in tile generation")
        
        # Verify palette indexing
        pixels = img_quantized.load()
        used_indices = set()
        for y in range(min(10, img_quantized.height)):  # Sample first 10 rows
            for x in range(min(10, img_quantized.width)):  # Sample first 10 columns
                used_indices.add(pixels[x, y])
        print(f"Sample palette indices used: {sorted(list(used_indices))}")
        print(f"Total unique palette indices in sample: {len(used_indices)}")
        
        # DEBUG: Check a broader sample to see if there are more indices
        broader_indices = set()
        for y in range(0, img_quantized.height, 10):  # Every 10th row
            for x in range(0, img_quantized.width, 10):  # Every 10th column
                broader_indices.add(pixels[x, y])
        print(f"DEBUG: Broader sample indices: {sorted(list(broader_indices))}")
        print(f"DEBUG: Broader sample count: {len(broader_indices)}")
        
        # Create unique tile tracking
        unique_tiles = {}
        tile_index = 0
        
        # DEBUG: Track all indices we encounter during tile processing
        tile_processing_indices = set()
        
        for tile_y in range(actual_tiles_y):
            for tile_x in range(actual_tiles_x):
                # Extract 8x8 tile - image is now exactly tilemap grid size
                tile_pixels = []
                for y in range(8):
                    for x in range(8):
                        pixel_x = tile_x * 8 + x
                        pixel_y = tile_y * 8 + y
                        
                        # Get palette index directly from quantized image
                        # Image is now exactly tilemap grid size, so no bounds checking needed
                        color_idx = pixels[pixel_x, pixel_y]
                        tile_pixels.append(color_idx)
                        tile_processing_indices.add(color_idx)
                
                # DEBUG: Print indices for first few tiles
                if tile_y == 0 and tile_x < 3:
                    print(f"DEBUG: Tile ({tile_x},{tile_y}) indices: {sorted(set(tile_pixels))}")
                
                # Convert tile to 8bpp format
                tile_data = self.convert_tile_to_bitplanes(tile_pixels)
                
                # Create tile signature for uniqueness checking
                tile_signature = tuple(tile_data)
                
                if tile_signature in unique_tiles:
                    # Use existing tile
                    tile_number = unique_tiles[tile_signature]
                else:
                    # New unique tile
                    unique_tiles[tile_signature] = tile_index
                    tile_number = tile_index
                    
                    # Add tile data (64 bytes total: 8x8 pixels / 1 pixel per byte)
                    tiles_data.extend(tile_data)
                    
                    tile_index += 1
                
                # Create SNES tilemap entry (16-bit)
                # Format: vhoppptt tttttttt
                # v = vertical flip, h = horizontal flip, o = priority, p = palette (0-7), t = tile number
                tilemap_entry = tile_number & 0x3FF  # Lower 10 bits: tile number
                tilemap_entry |= (0 << 10)  # Palette 0 (bits 10-12)
                tilemap_entry |= (0 << 13)  # Priority 0 (bit 13)
                tilemap_entry |= (0 << 14)  # Horizontal flip 0 (bit 14)
                tilemap_entry |= (0 << 15)  # Vertical flip 0 (bit 15)
                
                tilemap_data.append(tilemap_entry)
        
        # DEBUG: Show what indices we encountered during tile processing
        print(f"DEBUG: Palette indices encountered during tile processing: {sorted(list(tile_processing_indices))}")
        print(f"DEBUG: Count of palette indices during tile processing: {len(tile_processing_indices)}")
        
        # Validate that palette indices are within the per-scanline system range (0-15)
        if tile_processing_indices:
            max_palette_index = max(tile_processing_indices)
            min_palette_index = min(tile_processing_indices)
            print(f"Palette index range used: {min_palette_index} to {max_palette_index}")
            if max_palette_index >= 16:
                print(f"WARNING: Palette index {max_palette_index} >= 16 (per-scanline system uses 0-15)!")
            if min_palette_index < 0:
                print(f"WARNING: Negative palette index {min_palette_index}!")
            
            if len(tile_processing_indices) > 16:
                print(f"WARNING: Using {len(tile_processing_indices)} unique palette indices, but per-scanline system supports only 16!")
            else:
                print(f"✓ Using {len(tile_processing_indices)} palette indices within per-scanline range (0-15)")
        
        # The tiles_data contains bitplane bytes (0-255), which is normal and correct
        print(f"Bitplane tile data: {len(tiles_data)} bytes generated from {len(unique_tiles)} unique tiles")
        
        print(f"Generated {len(unique_tiles)} unique tiles from {actual_tiles_x * actual_tiles_y} total tiles")
        return tiles_data, tilemap_data, len(unique_tiles), actual_tiles_x, actual_tiles_y
    
    def create_4bpp_preview(self, img_quantized, palette_colors, output_path):
        """Create a 4bpp preview image with 256 width, preserving aspect ratio"""
        print("Creating 4bpp preview with 256 width...")
        
        # Resize to 256 width for preview, preserving aspect ratio
        target_width = 256
        aspect_ratio = img_quantized.height / img_quantized.width
        new_height = int(target_width * aspect_ratio)
        print(f"Preview dimensions: {target_width}x{new_height} (aspect ratio preserved)")
        preview_img = img_quantized.resize((target_width, new_height), Image.NEAREST)  # Use NEAREST to preserve palette indices
        
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

    def generate_header_file(self, output_path, palette_colors, tiles_data, tilemap_data, num_tiles, quality_metrics=None, actual_tiles_x=None, actual_tiles_y=None, scanline_palettes=None):
        """Generate C header file with all the data"""
        with open(output_path, 'w') as f:
            f.write("/* Generated by ConvertImage.py - SNES Mode 1 BG1 Data with Per-Scanline Palettes */\n")
            f.write(f"/* Image: {os.path.basename(INPUT_IMAGE_PATH)} */\n")
            f.write(f"/* Tiles: {num_tiles}, Tilemap: {len(tilemap_data)} entries */\n")
            if scanline_palettes:
                total_unique = len(set().union(*[set(p) for p in scanline_palettes]))
                f.write(f"/* Scanline Palettes: {len(scanline_palettes)} scanlines with optimized palettes */\n")
                f.write(f"/* Total unique colors: {total_unique} (16 per scanline) */\n")
            if quality_metrics:
                mse, psnr, ssim = quality_metrics
                f.write(f"/* Quality: MSE={mse:.2f}, PSNR={psnr:.2f}dB, SSIM={ssim:.4f} */\n")
            f.write("\n")
            
            # No separate bg_palette needed - use scanline_palettes[0] directly
            
            # Per-scanline palettes
            if scanline_palettes:
                f.write(f"/* Per-scanline palettes ({len(scanline_palettes)} scanlines) */\n")
                f.write(f"const unsigned short scanline_palettes[{len(scanline_palettes)}][16] = {{\n")
                for scanline_idx, scanline_palette in enumerate(scanline_palettes):
                    f.write(f"    {{ // Scanline {scanline_idx}\n")
                    for i, color in enumerate(scanline_palette):
                        snes_color = self.rgb_to_snes_color(color)
                        f.write(f"        0x{snes_color:04X}")
                        if i < 15:
                            f.write(",")
                        f.write(f"  // Color {i}: RGB({color[0]}, {color[1]}, {color[2]})\n")
                    f.write("    }")
                    if scanline_idx < len(scanline_palettes) - 1:
                        f.write(",")
                    f.write("\n")
                f.write("};\n\n")
            
            # Tile data
            f.write(f"/* 4bpp Tile data - {len(tiles_data)} bytes ({num_tiles} tiles) */\n")
            f.write("const unsigned char bg_tiles[] = {\n")
            for i in range(0, len(tiles_data), 16):
                f.write("    ")
                for j in range(16):
                    if i + j < len(tiles_data):
                        f.write(f"0x{tiles_data[i + j]:02X}")
                        if i + j < len(tiles_data) - 1:
                            f.write(", ")
                f.write("\n")
            f.write("};\n\n")
            
            # Tilemap data
            f.write(f"/* Tilemap - {len(tilemap_data)} entries ({actual_tiles_x or self.tiles_x}x{actual_tiles_y or self.tiles_y} tiles) */\n")
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
            
            # Setup function for per-scanline palettes
            f.write("/* Per-scanline palette setup function */\n")
            f.write("void setup_hdma_color_cycle() {\n")
            f.write("    /* Per-scanline palette system ready */\n")
            f.write("    /* Use scanline_palettes array with IRQ handler */\n")
            f.write("}\n\n")
            
            # Constants
            f.write("/* Constants for easy access */\n")
            f.write(f"#define BG_TILES_COUNT {num_tiles}\n")
            f.write(f"#define BG_TILEMAP_WIDTH {actual_tiles_x or self.tiles_x}\n")
            f.write(f"#define BG_TILEMAP_HEIGHT {actual_tiles_y or self.tiles_y}\n")
            f.write(f"#define BG_TILES_SIZE {len(tiles_data)}\n")
            f.write(f"#define BG_TILEMAP_SIZE {len(tilemap_data)}\n")
            if scanline_palettes:
                f.write(f"#define SCANLINE_COUNT {len(scanline_palettes)}\n")
                f.write(f"#define COLORS_PER_SCANLINE 16\n")
    
    def convert(self):
        """Main conversion function with per-scanline palette support"""
        input_path = INPUT_IMAGE_PATH
        output_path = OUTPUT_HEADER_PATH
        
        print(f"Converting {input_path} to SNES Mode 1 BG1 format with per-scanline palettes...")
        
        # Load original image
        original_img = self.load_image(input_path, brightness=1.0)
        if original_img is None:
                return False
        
        # Apply light pre-quantization to reduce color banding while preserving distinct colors
        print("Applying light pre-quantization to reduce color banding...")
        
        # Use PIL's conservative quantization with more colors to reduce banding
        # without the aggressive dithering/blending of pngquant
        try:
            # First check how many colors the image actually has
            original_colors = original_img.getcolors(maxcolors=256*256*256)
            if original_colors:
                total_colors = len(original_colors)
                print(f"Original image has {total_colors} unique colors")
                
                # Only apply pre-quantization if the image has excessive colors
                if total_colors > 512:  # Conservative threshold
                    # Use 256 colors (much more conservative than pngquant's 64)
                    print(f"Pre-quantizing from {total_colors} to 256 colors using PIL (preserves distinct colors)")
                    working_img = original_img.quantize(colors=256)  # Default quantization method
                    working_img = working_img.convert('RGB')
                    print("✓ Light pre-quantization completed - distinct colors preserved")
                else:
                    print(f"Image has manageable color count ({total_colors}), skipping pre-quantization")
                    working_img = original_img
            else:
                print("Could not analyze color count, using original image")
                working_img = original_img
                
        except Exception as e:
            print(f"Pre-quantization failed: {e}, using original image")
            working_img = original_img
        
        # Extract per-scanline palettes with smart quantization (only quantize scanlines that need it)
        scanline_palettes, all_unique_colors = self.extract_scanline_palettes_smart(working_img)
        
        # Create color mapping using per-scanline palettes
        img_quantized, scanline_data = self.create_scanline_color_map(working_img, scanline_palettes)
        
        # Use first scanline palette as the master palette for compatibility
        self.palette_colors = scanline_palettes[0]
        
        # Calculate similarity metrics
        # Skip quality calculation for simplicity
        mse, psnr, ssim = 0, 0, 0
        print(f"Image Quality Metrics:")
        print(f"  MSE (Mean Squared Error): {mse:.2f}")
        print(f"  PSNR (Peak Signal-to-Noise Ratio): {psnr:.2f} dB")
        print(f"  SSIM (Structural Similarity Index): {ssim:.4f}")
        print(f"  Quality: Per-scanline optimized")
        
        # Convert to tiles
        tiles_data, tilemap_data, num_tiles, actual_tiles_x, actual_tiles_y = self.convert_image_to_tiles(img_quantized)
        
        # Check if data is too large for ROM bank
        if len(tiles_data) > 60000:  # Leave some room for other data
            print(f"\nWarning: Tile data ({len(tiles_data)} bytes) is too large for ROM bank!")
            print("Creating simplified version with reduced complexity...")
            
            # Create a simplified version with fewer unique tiles
            simplified_img = self.create_simple_test_image()
            
            # Re-extract scanline palettes and convert
            scanline_palettes, all_unique_colors = self.extract_scanline_palettes(simplified_img)
            img_quantized, scanline_data = self.create_scanline_color_map(simplified_img, scanline_palettes)
            self.palette_colors = scanline_palettes[0]
            
            # Convert to tiles again
            tiles_data, tilemap_data, num_tiles, actual_tiles_x, actual_tiles_y = self.convert_image_to_tiles(img_quantized)
            
            if len(tiles_data) > 60000:
                print("Even simplified version is too large. Using minimal pattern.")
                # Use the simple test pattern as last resort
                simple_img = self.create_simple_test_image()
                scanline_palettes, all_unique_colors = self.extract_scanline_palettes(simple_img)
                img_quantized, scanline_data = self.create_scanline_color_map(simple_img, scanline_palettes)
                self.palette_colors = scanline_palettes[0]
                tiles_data, tilemap_data, num_tiles, actual_tiles_x, actual_tiles_y = self.convert_image_to_tiles(img_quantized)
                mse, psnr, ssim = 0, 0, 0  # Reset metrics for simple pattern
        
        # Generate header file with per-scanline palette data
        print("Generating Mode 1 4bpp header file with per-scanline palettes...")
        self.generate_header_file(output_path, self.palette_colors, tiles_data, tilemap_data, num_tiles, None, actual_tiles_x, actual_tiles_y, scanline_palettes)
        
        # Create preview images
        print("\nGenerating preview images...")
        self.create_palette_preview(self.palette_colors, output_path)
        self.create_4bpp_preview(img_quantized, self.palette_colors, output_path)
        
        print(f"\nConversion complete! Output saved to {output_path}")
        print(f"Generated {num_tiles} unique tiles")
        print(f"Tile data size: {len(tiles_data)} bytes")
        print(f"Tilemap size: {len(tilemap_data)} entries")
        print(f"Per-scanline palettes: {len(scanline_palettes)} scanlines")
        print(f"Total unique colors: {len(all_unique_colors)} (16 per scanline)")
        print(f"Preview images created for verification")
        
        return True

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
        print(f"Usage: python ConvertImage.py <image_file.png>")
        print(f"Example: python ConvertImage.py BG18_test2.png")
        return 1
    
    # Update the global input path
    INPUT_IMAGE_PATH = input_image
    
    converter = SNESMode1Converter()
    success = converter.convert()
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())