from PIL import Image
import os

"""
SNES Sprite Sheet Converter with Advanced Palette Optimization

This script converts sprite sheets to SNES-compatible formats (2bpp and 4bpp).
For 4bpp sprites, it implements a comprehensive palette optimization system that
dramatically reduces memory usage through multiple optimization strategies.

Palette Optimization Strategies:
1. Duplicate Detection: Identifies and reuses exact duplicate palettes
2. Subset Detection: Finds palettes that are subsets of existing ones (with color rearrangement)
3. Similar Palette Sharing: Uses upper/lower halves of existing palettes
4. Palette Combination: Merges compatible palettes into shared 16-color palettes

Palette Sharing System:
- 4bpp palettes are 16 colors (indices 0-15)
- Upper half: colors 0-7 (indices 0-7)
- Lower half: colors 8-15 (indices 8-15)
- Sprites can share palettes by using either upper or lower halves
- Graphics data is automatically adjusted for sprites using lower halves
- Colors can be rearranged to maximize palette reuse
- This significantly reduces the total number of unique palettes needed
"""

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

def convert_tile_to_4bpp(tile_image, palette, use_lower_half=False, color_mapping=None, original_palette=None):
    """Converts an 8x8 tile image to SNES 4bpp format (32 bytes)."""
    tile_4bpp = bytearray(32)
    tile_rgb = tile_image.convert('RGB')
    pixels = list(tile_rgb.getdata())

    # Use original palette for pixel-to-index conversion if available, otherwise use the provided palette
    if original_palette and color_mapping:
        color_to_palette_index = {color: i for i, color in enumerate(original_palette)}
        search_palette = original_palette
    else:
        color_to_palette_index = {color: i for i, color in enumerate(palette)}
        search_palette = palette

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
            for i, pal_color in enumerate(search_palette):
                pr, pg, pb = pal_color
                dist = (r-pr)**2 + (g-pg)**2 + (b-pb)**2
                if dist < min_dist:
                    min_dist = dist
                    best_idx = i
            pixel_indices.append(best_idx)

    # Apply color mapping if provided
    if color_mapping:
        mapped_indices = []
        for pixel_index in pixel_indices:
            if pixel_index in color_mapping:
                mapped_indices.append(color_mapping[pixel_index])
            else:
                # If no mapping found, keep original index
                mapped_indices.append(pixel_index)
        pixel_indices = mapped_indices

    for y in range(8):
        row_pixels = pixel_indices[y*8 : y*8+8]

        bp0, bp1, bp2, bp3 = 0, 0, 0, 0

        for i, pixel_index in enumerate(row_pixels):
            # If using lower half, add 8 to the palette index
            adjusted_index = pixel_index + 8 if use_lower_half else pixel_index
            bp0 |= (adjusted_index & 1) << (7 - i)
            bp1 |= ((adjusted_index >> 1) & 1) << (7 - i)
            bp2 |= ((adjusted_index >> 2) & 1) << (7 - i)
            bp3 |= ((adjusted_index >> 3) & 1) << (7 - i)

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

def convert_tile_to_4bpp_16x16(tile_image, palette, use_lower_half=False, color_mapping=None, original_palette=None):
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
    top_data.extend(convert_tile_to_4bpp(tl, palette, use_lower_half, color_mapping, original_palette))
    top_data.extend(convert_tile_to_4bpp(tr, palette, use_lower_half, color_mapping, original_palette))

    bottom_data = bytearray()
    bottom_data.extend(convert_tile_to_4bpp(bl, palette, use_lower_half, color_mapping, original_palette))
    bottom_data.extend(convert_tile_to_4bpp(br, palette, use_lower_half, color_mapping, original_palette))

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


def find_similar_palette(target_palette, existing_palettes, threshold=0.7):
    """
    Find a palette that can share colors with the target palette.
    Returns (palette_index, use_lower_half) or (None, False) if no match found.
    """
    if not existing_palettes:
        return None, False
    
    # Convert palettes to sets of colors for easier comparison
    target_colors = set(target_palette)
    
    best_match = None
    best_score = 0
    best_use_lower = False
    
    for i, existing_palette in enumerate(existing_palettes):
        existing_colors = set(existing_palette)
        
        # Check if we can use the upper half (colors 0-7)
        upper_colors = set(existing_palette[:8])
        upper_score = len(target_colors.intersection(upper_colors)) / len(target_colors) if target_colors else 0
        if upper_score >= threshold and upper_score > best_score:
            best_match = i
            best_score = upper_score
            best_use_lower = False
        
        # Check if we can use the lower half (colors 8-15)
        lower_colors = set(existing_palette[8:])
        lower_score = len(target_colors.intersection(lower_colors)) / len(target_colors) if target_colors else 0
        if lower_score >= threshold and lower_score > best_score:
            best_match = i
            best_score = lower_score
            best_use_lower = True
    
    return best_match, best_use_lower


def can_combine_palettes(palette1, palette2):
    """
    Check if two palettes can be combined into a single 16-color palette.
    Returns True if they can be combined, False otherwise.
    """
    # Count unique colors in both palettes
    colors1 = set(palette1)
    colors2 = set(palette2)
    
    # Check if we have room for both (max 16 unique colors)
    total_unique = len(colors1.union(colors2))
    return total_unique <= 16


def find_duplicate_palette(target_palette, existing_palettes):
    """
    Find an exact duplicate of the target palette.
    Returns palette index or None if no duplicate found.
    """
    for i, existing_palette in enumerate(existing_palettes):
        if len(target_palette) == len(existing_palette):
            # Check if all colors match (allowing for different order)
            if set(target_palette) == set(existing_palette):
                return i
    return None


def find_subset_palette(target_palette, existing_palettes):
    """
    Find if the target palette is a subset of an existing palette.
    Returns (palette_index, use_lower_half, color_mapping) or (None, False, None).
    Color mapping maps target palette indices to existing palette indices.
    """
    target_colors = list(target_palette)
    
    for i, existing_palette in enumerate(existing_palettes):
        existing_colors = list(existing_palette)
        
        # Check if target is a subset of upper half (colors 0-7)
        upper_colors = existing_colors[:8]
        if len(set(target_colors).difference(set(upper_colors))) == 0:
            # Create color mapping from target to upper half
            color_mapping = {}
            for j, target_color in enumerate(target_colors):
                if target_color in upper_colors:
                    color_mapping[j] = upper_colors.index(target_color)
            return i, False, color_mapping
        
        # Check if target is a subset of lower half (colors 8-15)
        lower_colors = existing_colors[8:]
        if len(set(target_colors).difference(set(lower_colors))) == 0:
            # Create color mapping from target to lower half
            color_mapping = {}
            for j, target_color in enumerate(target_colors):
                if target_color in lower_colors:
                    color_mapping[j] = lower_colors.index(target_color) + 8
            return i, True, color_mapping
    
    return None, False, None


def rearrange_palette_colors(tile_image, target_palette, new_palette, color_mapping):
    """
    Rearrange colors in a tile image to match a new palette arrangement.
    Returns a new tile image with remapped colors.
    """
    tile_rgb = tile_image.convert('RGB')
    pixels = list(tile_rgb.getdata())
    
    # Create mapping from old color to new color
    old_to_new = {}
    for old_idx, new_idx in color_mapping.items():
        old_to_new[target_palette[old_idx]] = new_palette[new_idx]
    
    # Remap pixels
    new_pixels = []
    for pixel in pixels:
        if pixel in old_to_new:
            new_pixels.append(old_to_new[pixel])
        else:
            # Find closest color in new palette
            min_dist = float('inf')
            best_color = pixel
            for color in new_palette:
                r, g, b = pixel
                pr, pg, pb = color
                dist = (r-pr)**2 + (g-pg)**2 + (b-pb)**2
                if dist < min_dist:
                    min_dist = dist
                    best_color = color
            new_pixels.append(best_color)
    
    # Create new image with remapped colors
    new_image = Image.new('RGB', tile_rgb.size)
    new_image.putdata(new_pixels)
    return new_image


def create_shared_palette(palette1, palette2):
    """
    Create a 16-color palette by combining two 4-8 color palettes.
    palette1 goes in the upper half (0-7), palette2 goes in the lower half (8-15).
    """
    shared_palette = [0] * 16  # Initialize with black
    
    # Fill upper half with palette1
    for i, color in enumerate(palette1[:8]):
        shared_palette[i] = color
    
    # Fill lower half with palette2
    for i, color in enumerate(palette2[:8]):
        shared_palette[i + 8] = color
    
    return shared_palette


def optimize_palette_for_sprite(original_palette, existing_palettes, tile_image):
    """
    Apply aggressive palette optimization to a 16x16 sprite.
    Returns (optimized_palette, use_lower_half, optimization_type, color_mapping)
    """
    # Skip optimization for palettes with very few colors (less than 3 non-black colors)
    non_black_colors = [c for c in original_palette if c != (0, 0, 0)]
    if len(non_black_colors) < 3:
        return original_palette, False, "new", None
    
    # First, check for exact duplicates
    duplicate_idx = find_duplicate_palette(original_palette, existing_palettes)
    if duplicate_idx is not None:
        return existing_palettes[duplicate_idx], False, "duplicate", None
    
    # Try aggressive palette sharing with color rearrangement
    best_match = find_best_palette_match(original_palette, existing_palettes, tile_image)
    if best_match:
        return best_match
    
    # Try to create a new shared palette by combining with existing ones
    best_combination = find_best_palette_combination(original_palette, existing_palettes, tile_image)
    if best_combination:
        return best_combination
    
    # No optimization possible, return original palette (ensuring black is at index 0)
    return ensure_black_at_index_zero(original_palette), False, "new", None


def find_best_palette_match(target_palette, existing_palettes, tile_image):
    """
    Find the best palette match with aggressive color rearrangement.
    Returns (optimized_palette, use_lower_half, optimization_type, color_mapping) or None.
    """
    if not existing_palettes:
        return None
    
    target_colors = set(target_palette)
    target_non_black = [c for c in target_palette if c != (0, 0, 0)]
    
    best_match = None
    best_score = 0
    best_use_lower = False
    best_color_mapping = None
    
    for i, existing_palette in enumerate(existing_palettes):
        existing_colors = set(existing_palette)
        
        # Check upper half (colors 0-7)
        upper_colors = set(existing_palette[:8])
        upper_intersection = target_colors.intersection(upper_colors)
        upper_score = len(upper_intersection) / len(target_colors) if target_colors else 0
        
        if upper_score >= 0.6 and upper_score > best_score:  # Lower threshold for more aggressive matching
            # Create optimal color mapping for upper half
            color_mapping = create_optimal_color_mapping(target_palette, existing_palette[:8], 0)
            if color_mapping and evaluate_color_mapping_quality(target_palette, color_mapping):
                best_match = i
                best_score = upper_score
                best_use_lower = False
                best_color_mapping = color_mapping
        
        # Check lower half (colors 8-15)
        lower_colors = set(existing_palette[8:])
        lower_intersection = target_colors.intersection(lower_colors)
        lower_score = len(lower_intersection) / len(target_colors) if target_colors else 0
        
        if lower_score >= 0.6 and lower_score > best_score:
            # Create optimal color mapping for lower half
            color_mapping = create_optimal_color_mapping(target_palette, existing_palette[8:], 8)
            if color_mapping and evaluate_color_mapping_quality(target_palette, color_mapping):
                best_match = i
                best_score = lower_score
                best_use_lower = True
                best_color_mapping = color_mapping
        
        # Check if we can use the full palette with rearrangement
        full_intersection = target_colors.intersection(existing_colors)
        full_score = len(full_intersection) / len(target_colors) if target_colors else 0
        
        if full_score >= 0.7 and full_score > best_score:
            # Try to fit target palette into existing palette with rearrangement
            color_mapping = create_optimal_color_mapping(target_palette, existing_palette, 0)
            if color_mapping and evaluate_color_mapping_quality(target_palette, color_mapping):
                best_match = i
                best_score = full_score
                best_use_lower = False
                best_color_mapping = color_mapping
    
    if best_match is not None:
        return existing_palettes[best_match], best_use_lower, "similar", best_color_mapping
    
    return None


def find_best_palette_combination(target_palette, existing_palettes, tile_image):
    """
    Find the best palette to combine with the target palette.
    Returns (optimized_palette, use_lower_half, optimization_type, color_mapping) or None.
    """
    if not existing_palettes:
        return None
    
    target_colors = set(target_palette)
    target_non_black = [c for c in target_palette if c != (0, 0, 0)]
    
    best_combination = None
    best_score = 0
    best_color_mapping = None
    
    for i, existing_palette in enumerate(existing_palettes):
        existing_colors = set(existing_palette)
        
        # Check if we can combine palettes (max 16 unique colors)
        combined_colors = target_colors.union(existing_colors)
        if len(combined_colors) <= 16:
            # Calculate combination score based on overlap and utilization
            overlap = len(target_colors.intersection(existing_colors))
            total_target = len(target_colors)
            total_existing = len(existing_colors)
            
            # Score based on overlap and how well we can utilize the combined palette
            overlap_score = overlap / total_target if total_target > 0 else 0
            utilization_score = len(combined_colors) / 16  # Prefer palettes that use more colors
            combination_score = overlap_score * 0.7 + utilization_score * 0.3
            
            if combination_score > best_score and combination_score >= 0.4:  # Lower threshold
                # Create combined palette
                combined_palette = create_optimized_combined_palette(existing_palette, target_palette)
                if combined_palette:
                    # Create color mapping for the combined palette
                    color_mapping = create_optimal_color_mapping(target_palette, combined_palette, 0)
                    if color_mapping and evaluate_color_mapping_quality(target_palette, color_mapping):
                        best_combination = i
                        best_score = combination_score
                        best_color_mapping = color_mapping
    
    if best_combination is not None:
        # Create the combined palette and update the existing one
        existing_palette = existing_palettes[best_combination]
        combined_palette = create_optimized_combined_palette(existing_palette, target_palette)
        combined_palette = ensure_black_at_index_zero(combined_palette)
        existing_palettes[best_combination] = combined_palette
        return combined_palette, False, "shared", best_color_mapping
    
    return None


def create_optimized_combined_palette(existing_palette, new_palette):
    """
    Create an optimized 16-color palette by intelligently combining two palettes.
    Prioritizes color overlap and optimal arrangement. Always ensures black is at index 0.
    """
    combined_palette = [(0, 0, 0)] * 16  # Initialize with black
    
    # Get all unique colors from both palettes
    all_colors = list(set(existing_palette + new_palette))
    
    # Remove black from the list (it will always be at index 0)
    if (0, 0, 0) in all_colors:
        all_colors.remove((0, 0, 0))
    
    # Sort colors by frequency in both palettes (prioritize commonly used colors)
    color_frequency = {}
    for color in existing_palette + new_palette:
        if color != (0, 0, 0):
            color_frequency[color] = color_frequency.get(color, 0) + 1
    
    # Sort by frequency (descending) then by color value for determinism
    all_colors.sort(key=lambda c: (-color_frequency.get(c, 0), c))
    
    # Fill the palette with the most important colors first
    for i, color in enumerate(all_colors[:15]):  # Leave room for black at index 0
        combined_palette[i + 1] = color
    
    return combined_palette


def ensure_black_at_index_zero(palette):
    """
    Ensure that black (0, 0, 0) is always at index 0 in the palette.
    If black is not at index 0, move it there and adjust other colors accordingly.
    """
    if not palette or palette[0] == (0, 0, 0):
        return palette  # Already correct
    
    # Find black in the palette
    black_index = None
    for i, color in enumerate(palette):
        if color == (0, 0, 0):
            black_index = i
            break
    
    if black_index is None:
        # No black found, add it at index 0 and shift other colors
        new_palette = [(0, 0, 0)] + palette[:15]
        return new_palette
    else:
        # Black found, move it to index 0
        new_palette = [(0, 0, 0)] + [color for i, color in enumerate(palette) if i != black_index]
        # Pad with black if needed
        while len(new_palette) < 16:
            new_palette.append((0, 0, 0))
        return new_palette[:16]


def create_color_mapping_for_palette_merge(old_palette, new_palette):
    """
    Create a color mapping when merging palettes.
    Maps old palette indices to new palette indices.
    """
    color_mapping = {}
    
    for old_idx, old_color in enumerate(old_palette):
        if old_color in new_palette:
            new_idx = new_palette.index(old_color)
            color_mapping[old_idx] = new_idx
        else:
            # Find closest color in new palette
            min_dist = float('inf')
            best_idx = 0
            for new_idx, new_color in enumerate(new_palette):
                r, g, b = old_color
                nr, ng, nb = new_color
                dist = (r-nr)**2 + (g-ng)**2 + (b-nb)**2
                if dist < min_dist:
                    min_dist = dist
                    best_idx = new_idx
            color_mapping[old_idx] = best_idx
    
    return color_mapping


def regenerate_graphics_data_with_mapping(graphics_data, color_mapping):
    """
    Regenerate graphics data by applying a color mapping to the pixel indices.
    """
    new_graphics_data = bytearray(len(graphics_data))
    
    for i, byte in enumerate(graphics_data):
        # Extract the two 4-bit color indices from the byte
        lower_index = byte & 0x0F
        upper_index = (byte >> 4) & 0x0F
        
        # Map the indices using the color mapping
        new_lower_index = color_mapping.get(lower_index, lower_index)
        new_upper_index = color_mapping.get(upper_index, upper_index)
        
        # Reconstruct the byte with the new indices
        new_byte = (new_upper_index << 4) | new_lower_index
        new_graphics_data[i] = new_byte
    
    return new_graphics_data


def create_optimal_color_mapping(source_palette, target_palette, offset=0):
    """
    Create an optimal color mapping from source to target palette.
    Returns a dictionary mapping source indices to target indices.
    """
    color_mapping = {}
    
    # Special handling: ensure black always maps to index 0
    for i, source_color in enumerate(source_palette):
        if source_color == (0, 0, 0):
            color_mapping[i] = 0
            break
    
    # First pass: exact matches (skip black since it's already handled)
    for i, source_color in enumerate(source_palette):
        if source_color != (0, 0, 0) and source_color in target_palette:
            target_idx = target_palette.index(source_color) + offset
            # Ensure the target index is within bounds
            if target_idx < len(target_palette):
                color_mapping[i] = target_idx
    
    # Second pass: closest matches for unmapped colors
    for i, source_color in enumerate(source_palette):
        if i not in color_mapping:
            if source_color == (0, 0, 0):
                color_mapping[i] = 0  # Black always maps to 0
            else:
                best_idx = find_closest_color_index(source_color, target_palette) + offset
                # Ensure the target index is within bounds
                if best_idx < len(target_palette):
                    # Check if the color difference is acceptable
                    target_color = target_palette[best_idx - offset]
                    r_diff = abs(source_color[0] - target_color[0])
                    g_diff = abs(source_color[1] - target_color[1])
                    b_diff = abs(source_color[2] - target_color[2])
                    total_diff = r_diff + g_diff + b_diff
                    
                    if total_diff > 100:  # If color difference is too large, map to black instead
                        color_mapping[i] = 0
                    else:
                        color_mapping[i] = best_idx
                else:
                    # Fallback to index 0 (black) if out of bounds
                    color_mapping[i] = 0
    
    return color_mapping


def find_closest_color_index(target_color, palette):
    """
    Find the index of the closest color in the palette using perceptual color distance.
    """
    if not palette:
        return 0
    
    min_dist = float('inf')
    best_idx = 0
    
    for i, color in enumerate(palette):
        # Use perceptual color distance (weighted RGB)
        r, g, b = target_color
        pr, pg, pb = color
        
        # Perceptual weights (human eye is more sensitive to green)
        r_diff = (r - pr) * 0.3
        g_diff = (g - pg) * 0.59
        b_diff = (b - pb) * 0.11
        
        dist = r_diff**2 + g_diff**2 + b_diff**2
        
        if dist < min_dist:
            min_dist = dist
            best_idx = i
    
    return best_idx


def evaluate_color_mapping_quality(original_palette, color_mapping):
    """
    Evaluate the quality of a color mapping.
    Returns True if the mapping is acceptable, False otherwise.
    """
    if not color_mapping:
        return False
    
    non_black_colors = [c for c in original_palette if c != (0, 0, 0)]
    if not non_black_colors:
        return True
    
    # Count how many non-black colors are mapped to black (bad)
    mapped_to_black = sum(1 for i, color in enumerate(original_palette) 
                         if color != (0, 0, 0) and color_mapping.get(i, 0) == 0)
    
    # Accept if less than 30% of non-black colors are mapped to black
    return mapped_to_black / len(non_black_colors) < 0.3


def optimize_palette_utilization(palettes):
    """
    Optimize palette utilization by combining underutilized palettes.
    Returns updated palettes and a mapping of old indices to new indices.
    """
    if len(palettes) <= 1:
        return palettes, {}
    
    # Find underutilized palettes (less than 8 colors used - very aggressive)
    underutilized = []
    for i, palette in enumerate(palettes):
        non_black_colors = [c for c in palette if c != (0, 0, 0)]
        if len(non_black_colors) < 8:
            underutilized.append((i, palette, non_black_colors))
    
    if len(underutilized) < 2:
        return palettes, {}
    
    # Sort by number of colors (ascending) to combine smaller palettes first
    underutilized.sort(key=lambda x: len(x[2]))
    
    new_palettes = []
    palette_mapping = {}  # Maps old palette index to new palette index
    
    # Keep well-utilized palettes as-is (ensuring black is at index 0)
    for i, palette in enumerate(palettes):
        non_black_colors = [c for c in palette if c != (0, 0, 0)]
        if len(non_black_colors) >= 8:
            new_palettes.append(ensure_black_at_index_zero(palette))
            palette_mapping[i] = len(new_palettes) - 1
    
    # Combine underutilized palettes more aggressively
    i = 0
    while i < len(underutilized):
        # Try to combine up to 4 palettes into one shared palette
        combined_palette = [(0, 0, 0)] * 16  # Start with all black
        color_mappings = {}  # Maps old palette index to color mapping
        
        # Collect all unique colors from the palettes to combine
        all_colors_to_combine = []
        palettes_to_combine = []
        
        # Add colors from up to 4 palettes
        for j in range(min(4, len(underutilized) - i)):
            old_idx, old_palette, old_colors = underutilized[i + j]
            palettes_to_combine.append((old_idx, old_palette, old_colors))
            all_colors_to_combine.extend(old_colors)
        
        # Remove duplicates while preserving order
        unique_colors = []
        seen = set()
        for color in all_colors_to_combine:
            if color not in seen:
                unique_colors.append(color)
                seen.add(color)
        
        # Fill the combined palette with unique colors
        for k, color in enumerate(unique_colors[:15]):  # Leave room for black at index 0
            combined_palette[k + 1] = color
        
        # Create color mappings for each original palette
        for old_idx, old_palette, old_colors in palettes_to_combine:
            color_mapping = {}
            for orig_idx, orig_color in enumerate(old_palette):
                if orig_color in combined_palette:
                    new_idx = combined_palette.index(orig_color)
                    color_mapping[orig_idx] = new_idx
                else:
                    # Find closest color in combined palette
                    min_dist = float('inf')
                    best_idx = 0
                    for new_idx, new_color in enumerate(combined_palette):
                        if new_color != (0, 0, 0):  # Don't map to black unless it's the original color
                            r, g, b = orig_color
                            nr, ng, nb = new_color
                            dist = (r-nr)**2 + (g-ng)**2 + (b-nb)**2
                            if dist < min_dist:
                                min_dist = dist
                                best_idx = new_idx
                    color_mapping[orig_idx] = best_idx
            
            color_mappings[old_idx] = color_mapping
            palette_mapping[old_idx] = len(new_palettes)
        
        # Ensure black is at index 0
        combined_palette = ensure_black_at_index_zero(combined_palette)
        new_palettes.append(combined_palette)
        i += min(4, len(underutilized) - i)
    
    return new_palettes, palette_mapping


def find_palette_index(palette, palette_list):
    """
    Find the index of a palette in the palette list.
    Returns the index or -1 if not found.
    """
    for i, existing_palette in enumerate(palette_list):
        if palette == existing_palette:
            return i
    return -1


def create_color_mapping_for_similar_palette(original_palette, target_palette, use_lower_half):
    """
    Create a color mapping when using a similar palette.
    Maps original palette indices to target palette indices.
    """
    color_mapping = {}
    
    if use_lower_half:
        # Use lower half of target palette (indices 8-15)
        target_colors = target_palette[8:]
        offset = 8
    else:
        # Use upper half of target palette (indices 0-7)
        target_colors = target_palette[:8]
        offset = 0
    
    # Map each color in original palette to its position in target palette
    for i, original_color in enumerate(original_palette):
        if original_color in target_colors:
            target_index = target_colors.index(original_color)
            color_mapping[i] = target_index + offset
        else:
            # Find closest color in target palette
            min_dist = float('inf')
            best_idx = 0
            for j, target_color in enumerate(target_colors):
                r, g, b = original_color
                tr, tg, tb = target_color
                dist = (r-tr)**2 + (g-tg)**2 + (b-tb)**2
                if dist < min_dist:
                    min_dist = dist
                    best_idx = j
            color_mapping[i] = best_idx + offset
    
    return color_mapping


def create_color_mapping_for_shared_palette(original_palette, shared_palette, use_lower_half):
    """
    Create a color mapping when using a shared palette.
    Maps original palette indices to shared palette indices.
    """
    color_mapping = {}
    
    if use_lower_half:
        # Use lower half of shared palette (indices 8-15)
        target_colors = shared_palette[8:]
        offset = 8
    else:
        # Use upper half of shared palette (indices 0-7)
        target_colors = shared_palette[:8]
        offset = 0
    
    # Map each color in original palette to its position in shared palette
    for i, original_color in enumerate(original_palette):
        if original_color in target_colors:
            target_index = target_colors.index(original_color)
            color_mapping[i] = target_index + offset
        else:
            # Find closest color in shared palette
            min_dist = float('inf')
            best_idx = 0
            for j, target_color in enumerate(target_colors):
                r, g, b = original_color
                tr, tg, tb = target_color
                dist = (r-tr)**2 + (g-tg)**2 + (b-tb)**2
                if dist < min_dist:
                    min_dist = dist
                    best_idx = j
            color_mapping[i] = best_idx + offset
    
    return color_mapping


def combine_underutilized_palettes(palettes, sprite_info_list, threshold=0.3):
    """
    Combine palettes that are underutilized (less than threshold% of colors used).
    Returns updated palettes and sprite info with new mappings.
    """
    if len(palettes) <= 1:
        return palettes, sprite_info_list, {}
    
    # Find underutilized palettes
    underutilized = []
    for i, palette in enumerate(palettes):
        non_black_colors = [c for c in palette if c != (0, 0, 0)]
        usage_percent = len(non_black_colors) / 16
        if usage_percent < threshold:
            underutilized.append((i, palette, non_black_colors))
    
    if len(underutilized) < 2:
        return palettes, sprite_info_list, {}
    
    # Sort by number of colors (ascending) to combine smaller palettes first
    underutilized.sort(key=lambda x: len(x[2]))
    
    new_palettes = []
    palette_mapping = {}  # Maps old palette index to new palette index
    new_sprite_info = []
    
    # Keep non-underutilized palettes as-is
    for i, palette in enumerate(palettes):
        non_black_colors = [c for c in palette if c != (0, 0, 0)]
        usage_percent = len(non_black_colors) / 16
        if usage_percent >= threshold:
            new_palettes.append(palette)
            palette_mapping[i] = len(new_palettes) - 1
    
    # Combine underutilized palettes
    i = 0
    while i < len(underutilized):
        # Try to combine up to 4 palettes into one shared palette
        combined_palette = [0] * 16  # Start with all black
        color_mappings = {}  # Maps old palette index to color mapping
        
        # Add colors from up to 4 palettes
        for j in range(min(4, len(underutilized) - i)):
            old_idx, old_palette, old_colors = underutilized[i + j]
            
            # Find available slots in the combined palette
            available_slots = []
            for k in range(16):
                if combined_palette[k] == (0, 0, 0):
                    available_slots.append(k)
            
            # Map colors to available slots
            color_mapping = {}
            for color in old_colors:
                if available_slots:
                    slot = available_slots.pop(0)
                    combined_palette[slot] = color
                    # Find the original index of this color in the old palette
                    for orig_idx, orig_color in enumerate(old_palette):
                        if orig_color == color:
                            color_mapping[orig_idx] = slot
                            break
            
            color_mappings[old_idx] = color_mapping
            palette_mapping[old_idx] = len(new_palettes)
        
        new_palettes.append(combined_palette)
        i += min(4, len(underutilized) - i)
    
    # Update sprite info with new palette indices and color mappings
    for sprite_info in sprite_info_list:
        old_pal_idx = sprite_info['pal_idx']
        if old_pal_idx in palette_mapping:
            new_sprite_info.append({
                'bpp': sprite_info['bpp'],
                'gfx_idx': sprite_info['gfx_idx'],
                'pal_idx': palette_mapping[old_pal_idx]
            })
        else:
            new_sprite_info.append(sprite_info)
    
    return new_palettes, new_sprite_info, {}


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
    
    # Palette optimization counters
    duplicate_palettes_saved = 0
    subset_palettes_saved = 0
    shared_palettes_created = 0

    # Process each 16x16 sprite (4 8x8 tiles) as a unit
    for y in range(0, num_rows):
        for x in range(0, tiles_per_row):
            box = (x * 8, y * 8, (x + 1) * 8, (y + 1) * 8)
            tile_8x8 = img.crop(box)
            tile = tile_8x8.resize((16, 16), Image.NEAREST)

            # --- Per-sprite palette generation and processing ---
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
                    # Ensure black is color 0, then add actual colors
                    final_palette = [black] + palette_colors[:15]
                    # Only pad with black if we have fewer than 16 colors
                    while len(final_palette) < 16:
                        final_palette.append(black)
                    palette = final_palette
                else:
                    palette = [black] * 16

                # Store original palette and tile for later optimization
                # We'll do all palette optimization in one pass after collecting all palettes
                pal_idx = len(row_4bpp_palettes)
                row_4bpp_palettes.append(palette)
                
                # Store tile data for later graphics generation
                gfx_idx = (len(all_4bpp_data) + len(row_top_4bpp_data)) // 32
                info = {"bpp": 4, "gfx_idx": gfx_idx, "pal_idx": pal_idx, "tile": tile, "original_palette": palette}
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

                gfx_idx = (len(all_2bpp_data) + len(row_top_2bpp_data)) // 16
                info = {"bpp": 2, "gfx_idx": gfx_idx, "pal_idx": len(all_2bpp_palettes) + len(row_2bpp_palettes)}
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

    # Note: 4bpp data will be generated after palette optimization

    all_2bpp_palettes.extend(row_2bpp_palettes)
    all_4bpp_palettes.extend(row_4bpp_palettes)
    sprite_info.extend(row_sprite_info)
    
    # === COMPREHENSIVE PALETTE OPTIMIZATION ===
    print("Applying comprehensive palette optimization...")
    
    # Step 1: Apply aggressive palette optimization to all 4bpp palettes
    optimized_4bpp_palettes = []
    palette_mapping = {}  # Maps old palette index to new palette index
    optimization_stats = {"duplicate": 0, "subset": 0, "shared": 0, "similar": 0, "new": 0}
    
    for i, original_palette in enumerate(all_4bpp_palettes):
        # Find the best optimization for this palette
        optimized_palette, use_lower_half, optimization_type, color_mapping = optimize_palette_for_sprite(
            original_palette, optimized_4bpp_palettes, None
        )
        
        # Find or add the optimized palette
        pal_idx = find_palette_index(optimized_palette, optimized_4bpp_palettes)
        if pal_idx == -1:
            pal_idx = len(optimized_4bpp_palettes)
            optimized_4bpp_palettes.append(optimized_palette)
        
        palette_mapping[i] = pal_idx
        optimization_stats[optimization_type] += 1
    
    # Step 2: Combine underutilized palettes
    print("Combining underutilized palettes...")
    final_4bpp_palettes, final_palette_mapping = optimize_palette_utilization(optimized_4bpp_palettes)
    
    # Update palette mapping to account for final optimization
    for old_idx, new_idx in palette_mapping.items():
        if new_idx in final_palette_mapping:
            palette_mapping[old_idx] = final_palette_mapping[new_idx]
        else:
            palette_mapping[old_idx] = new_idx
    
    # Step 3: Ensure black is at index 0 for all palettes
    for i, palette in enumerate(final_4bpp_palettes):
        final_4bpp_palettes[i] = ensure_black_at_index_zero(palette)
    
    all_4bpp_palettes = final_4bpp_palettes
    
    print(f"Palette optimization complete: {len(row_4bpp_palettes)} -> {len(all_4bpp_palettes)} palettes")
    print(f"Optimization breakdown: {optimization_stats}")
    
    # === UPDATE GRAPHICS DATA ===
    print("Updating graphics data with optimized palettes...")
    
    # Clear existing 4bpp data
    all_4bpp_data = bytearray()
    
    # Collect 4bpp graphics data in the same structure as before
    row_top_4bpp_data = bytearray()
    row_bottom_4bpp_data = bytearray()
    
    # Process 4bpp sprites with optimized palettes
    for info in sprite_info:
        if info['bpp'] == 4:
            tile = info['tile']
            original_palette = info['original_palette']
            # The pal_idx is already relative to 4bpp palettes only
            original_4bpp_pal_idx = info['pal_idx']
            
            if original_4bpp_pal_idx in palette_mapping:
                new_pal_idx = palette_mapping[original_4bpp_pal_idx]
                optimized_palette = all_4bpp_palettes[new_pal_idx]
                
                # Create color mapping from original to optimized palette
                color_mapping = create_optimal_color_mapping(original_palette, optimized_palette, 0)
                
                # Generate graphics data with the optimized palette
                top_data, bottom_data = convert_tile_to_4bpp_16x16(tile, optimized_palette, False, color_mapping, original_palette)
                
                # Update sprite info with new palette index (absolute index including 2bpp palettes)
                info['pal_idx'] = len(all_2bpp_palettes) + new_pal_idx
                
                # Add to row data (maintaining the same structure as before)
                row_top_4bpp_data.extend(top_data)
                row_bottom_4bpp_data.extend(bottom_data)
            else:
                print(f"Warning: Palette index {original_4bpp_pal_idx} not found in mapping")
    
    # Process 4bpp data in 16-tile segments (same as before)
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

    print("Graphics data update complete")


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
    print(f"Total sprite info entries: {len(sprite_info)}")
    print(f"arrMustBeObject tiles processed as 4bpp: {len(arrMustBeObject)}")
    print(f"Actual 4bpp sprites processed: {tile_count_4bpp}")
    print(f"Expected 4bpp sprites: {len(arrMustBeObject)}")
    
    # Calculate palette sharing statistics
    original_4bpp_count = tile_count_4bpp
    final_4bpp_count = len(all_4bpp_palettes)
    if original_4bpp_count > 0:
        palette_reduction = ((original_4bpp_count - final_4bpp_count) / original_4bpp_count) * 100
        print(f"4bpp palette optimization: {original_4bpp_count} original palettes reduced to {final_4bpp_count} ({palette_reduction:.1f}% reduction)")
        print(f"Optimization breakdown:")
        print(f"  - Duplicate palettes saved: {optimization_stats['duplicate']}")
        print(f"  - Subset palettes saved: {optimization_stats['subset']}")
        print(f"  - Shared palettes created: {optimization_stats['shared']}")
        print(f"  - New palettes created: {optimization_stats['new']}")
        
        # Analyze palette usage
        print(f"\nPalette usage analysis:")
        total_colors_used = 0
        for i, palette in enumerate(all_4bpp_palettes):
            non_black_colors = [c for c in palette if c != (0, 0, 0)]
            usage_percent = (len(non_black_colors) / 16) * 100
            total_colors_used += len(non_black_colors)
            print(f"  Palette {i}: {len(non_black_colors)}/16 colors used ({usage_percent:.1f}%)")
        
        # Calculate overall palette efficiency
        max_possible_colors = len(all_4bpp_palettes) * 16
        efficiency = (total_colors_used / max_possible_colors) * 100 if max_possible_colors > 0 else 0
        print(f"\nOverall palette efficiency: {total_colors_used}/{max_possible_colors} colors used ({efficiency:.1f}%)")
        
        # Verify graphics data integrity
        print(f"\nGraphics data verification:")
        total_4bpp_bytes = len(all_4bpp_data)
        non_zero_bytes = sum(1 for b in all_4bpp_data if b != 0)
        print(f"  4bpp graphics: {non_zero_bytes}/{total_4bpp_bytes} non-zero bytes ({non_zero_bytes/total_4bpp_bytes*100:.1f}%)")
        if non_zero_bytes == 0:
            print("  WARNING: All 4bpp graphics data is zero!")
        elif non_zero_bytes < total_4bpp_bytes * 0.1:
            print("  WARNING: Very few 4bpp graphics bytes are non-zero!")
        
        # Show some sample graphics data to verify color indices
        print(f"\nSample graphics data (first 32 bytes):")
        sample_data = all_4bpp_data[:32]
        hex_values = [f"0x{b:02x}" for b in sample_data]
        print(f"  {', '.join(hex_values[:16])}")
        print(f"  {', '.join(hex_values[16:32])}")
        
        # Count unique color indices used in graphics data
        unique_indices = set()
        for byte in all_4bpp_data:
            if byte != 0:
                # Extract 4-bit color indices from the byte
                unique_indices.add(byte & 0x0F)  # Lower 4 bits
                unique_indices.add((byte >> 4) & 0x0F)  # Upper 4 bits
        print(f"  Unique color indices used in graphics: {sorted(unique_indices)}")
        
        print(f"\nAdvanced optimization features:")
        print(f"  - Aggressive palette sharing with color rearrangement")
        print(f"  - Full 16-color palette utilization")
        print(f"  - Intelligent palette combination")
        print(f"  - Underutilized palette merging")
        print(f"  - Graphics data automatically adjusted for color remapping")

if __name__ == '__main__':
    main()
