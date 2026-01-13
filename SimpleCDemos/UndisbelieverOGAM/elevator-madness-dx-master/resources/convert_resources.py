#!/usr/bin/env python3
"""
convert_resources.py - Convert PCX resources to SNES format

Replaces the resources/Makefile functionality for converting PCX files to SNES format.
Converts .pcx files to .4bpp (tiles) and .clr (palette) using pcx2snes.

This script is designed to be modular, similar to the original Makefile structure.
"""

import os
import sys
import argparse
import re
from pathlib import Path

# Add shared pythonUtils to path
# Script is at: SimpleCDemos/UndisbelieverOGAM/elevator-madness-dx-master/resources/convert_resources.py
# pythonUtils is at: SimpleCDemos/shared/pythonUtils
# So we need to go up 4 levels from convert_resources.py to get to SimpleCDemos/
project_root = Path(__file__).resolve().parent.parent.parent.parent
python_utils_path = project_root / "shared" / "pythonUtils"
if python_utils_path.exists():
    sys.path.insert(0, str(python_utils_path))
else:
    print(f"Warning: pythonUtils not found at {python_utils_path}", file=sys.stderr)

from file_utils import (
    find_files_by_pattern,
    get_file_base_name,
    delete_files,
    change_extension,
    get_file_directory,
    file_exists
)
from pcx_converter import convert_pcx_to_snes as convert_pcx_to_snes_python


# ============================================================================
# METASPRITE CONVERSION FUNCTIONS
# ============================================================================

def parse_metasprite_data(data_file):
    """Parse the metasprite data file and return a dict of frame name -> data bytes"""
    frames = {}
    current_frame = None
    current_labels = []
    current_data = []
    prev_was_label = False
    
    with open(data_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                prev_was_label = False
                continue
            
            # Check for frame label
            match = re.match(r'^(\w+):', line)
            if match:
                label = match.group(1)
                
                # Check if there are multiple labels on the same line (aliases)
                all_labels = re.findall(r'(\w+):', line)
                
                # If previous line was also a label, this is an alias (same data)
                if prev_was_label and current_frame:
                    # Add this label as an alias to current frame
                    current_labels.extend(all_labels)
                    prev_was_label = True
                    continue
                
                # Save previous frame if exists and has data
                if current_frame and current_data:
                    # Save for all previous labels (if any aliases)
                    for prev_label in current_labels:
                        frames[prev_label] = bytes(current_data)
                
                # Start new frame
                current_frame = label
                current_labels = all_labels  # Track all labels for this frame
                current_data = []
                prev_was_label = True
                continue
            
            # Not a label line
            prev_was_label = False
            
            # Skip comments
            if line.startswith(';'):
                continue
            
            # Parse byte data
            if line.startswith('.byte'):
                # Extract hex values
                hex_values = re.findall(r'\$([0-9a-fA-F]{2})', line)
                for hex_val in hex_values:
                    current_data.append(int(hex_val, 16))
    
    # Save last frame if has data
    if current_frame and current_data:
        for label in current_labels:
            frames[label] = bytes(current_data)
    
    return frames

def parse_metasprite_table(table_file):
    """Parse the metasprite table file and return frame table structures"""
    tables = {}
    current_table = None
    current_frames = []
    
    with open(table_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            
            # Check for table label - look for MetaSprite_ labels that are frame sets
            match = re.match(r'^(\w+):', line)
            if match:
                label = match.group(1)
                
                # Check if this is a frame set (MetaSprite_player, MetaSprite_businessMan, etc.)
                # These come after MetaSpriteFrameTable labels
                if label.startswith('MetaSprite_') and label not in ['MetaSpriteFrameTable', 'MetaSpriteFrameTable_elevator', 'MetaSpriteFrameTable_npcs', 'MetaSpriteFrameTable_fighting', 'MetaSpriteFrameTable_leftRight']:
                    # Save previous table if exists
                    if current_table and current_frames:
                        tables[current_table] = current_frames
                    
                    # Start new table
                    current_table = label
                    current_frames = []
                    continue
            
            # Parse .addr directive
            match = re.match(r'\.addr\s+(\w+)', line)
            if match:
                frame_name = match.group(1)
                if current_table:
                    current_frames.append(frame_name)
    
    # Save last table
    if current_table and current_frames:
        tables[current_table] = current_frames
    
    return tables

def generate_metasprite_header(output_file, frames, tables):
    """Generate C header file with metasprite data"""
    with open(output_file, 'w') as f:
        f.write("/* Generated by convert_resources.py */\n")
        f.write("/* Metasprite layout data converted from assembly format */\n\n")
        f.write("#ifndef METASPRITE_DATA_H\n")
        f.write("#define METASPRITE_DATA_H\n\n")
        f.write("#include <stdint.h>\n\n")
        
        # Generate frame data arrays
        f.write("/* Metasprite frame data arrays */\n")
        frame_arrays = {}
        for frame_name, data in sorted(frames.items()):
            # Convert to C array name (remove MetaSprite_ prefix for cleaner names)
            var_name = frame_name.replace('MetaSprite_', '')
            frame_arrays[frame_name] = var_name
            
            f.write(f"/* {frame_name} */\n")
            f.write(f"static const uint8_t {var_name}[] = {{\n")  # Keep static for frame data (only used internally)
            f.write(f"    {len(data)},\n")  # First byte is count
            for i in range(0, len(data), 16):
                f.write("    ")
                for j in range(16):
                    if i + j < len(data):
                        f.write(f"0x{data[i + j]:02X}")
                        if i + j < len(data) - 1:
                            f.write(", ")
                f.write("\n")
            f.write("};\n\n")
        
        # Generate frame table arrays (pointers to frame data)
        f.write("/* Metasprite frame table arrays (pointers to frame data) */\n")
        for table_name, frame_list in sorted(tables.items()):
            var_name = table_name.replace('MetaSpriteFrameTable_', '').replace('MetaSprite_', '')
            if not var_name:
                var_name = table_name
            
            f.write(f"/* {table_name} */\n")
            f.write(f"static const uint8_t *const {var_name}[] = {{\n")  # Static to avoid multiple definitions when included in multiple files
            for frame_name in frame_list:
                if frame_name in frame_arrays:
                    f.write(f"    {frame_arrays[frame_name]},\n")
                else:
                    # Frame not found, use NULL
                    f.write(f"    NULL,  // {frame_name} (not found)\n")
            f.write("};\n\n")
        
        # Generate frame index constants
        f.write("/* Frame index constants */\n")
        f.write("/* Player frames */\n")
        player_frames = [
            ("MetaSpriteFrame_player_standRight", 0),
            ("MetaSpriteFrame_player_walkRight", 1),
            ("MetaSpriteFrame_player_turnRight", 2),
            ("MetaSpriteFrame_player_jumpRight", 3),
            ("MetaSpriteFrame_player_fallRight", 4),
            ("MetaSpriteFrame_player_pushButtonRight", 5),
            ("MetaSpriteFrame_player_zappedRight", 6),
            ("MetaSpriteFrame_player_standLeft", 7),
            ("MetaSpriteFrame_player_walkLeft", 8),
            ("MetaSpriteFrame_player_turnLeft", 9),
            ("MetaSpriteFrame_player_jumpLeft", 10),
            ("MetaSpriteFrame_player_fallLeft", 11),
            ("MetaSpriteFrame_player_pushButtonLeft", 12),
            ("MetaSpriteFrame_player_zappedLeft", 13),
        ]
        for frame_name, index in player_frames:
            const_name = frame_name.replace('MetaSpriteFrame_', '').upper()
            f.write(f"#define {const_name} {index}\n")
        
        f.write("\n/* NPC frames */\n")
        npc_frames = [
            ("MetaSpriteFrame_npc_normalStandRight", 0),
            ("MetaSpriteFrame_npc_normalWalkRight", 1),
            ("MetaSpriteFrame_npc_normalWaitRight", 2),
            ("MetaSpriteFrame_npc_stressedStandRight", 3),
            ("MetaSpriteFrame_npc_stressedWalkRight", 4),
            ("MetaSpriteFrame_npc_stressedWaitRight", 5),
            ("MetaSpriteFrame_npc_angryStandRight", 6),
            ("MetaSpriteFrame_npc_angryWalkRight", 7),
            ("MetaSpriteFrame_npc_angryWaitRight", 8),
            ("MetaSpriteFrame_npc_fightingStandRight", 9),
            ("MetaSpriteFrame_npc_fightingWalkRight", 10),
            ("MetaSpriteFrame_npc_fightingWaitRight", 11),
            ("MetaSpriteFrame_npc_normalStandLeft", 12),
            ("MetaSpriteFrame_npc_normalWalkLeft", 13),
            ("MetaSpriteFrame_npc_normalWaitLeft", 14),
            ("MetaSpriteFrame_npc_stressedStandLeft", 15),
            ("MetaSpriteFrame_npc_stressedWalkLeft", 16),
            ("MetaSpriteFrame_npc_stressedWaitLeft", 17),
            ("MetaSpriteFrame_npc_angryStandLeft", 18),
            ("MetaSpriteFrame_npc_angryWalkLeft", 19),
            ("MetaSpriteFrame_npc_angryWaitLeft", 20),
            ("MetaSpriteFrame_npc_fightingStandLeft", 21),
            ("MetaSpriteFrame_npc_fightingWalkLeft", 22),
            ("MetaSpriteFrame_npc_fightingWaitLeft", 23),
        ]
        for frame_name, index in npc_frames:
            const_name = frame_name.replace('MetaSpriteFrame_', '').upper()
            f.write(f"#define {const_name} {index}\n")
        
        f.write("\n/* Fighting cloud frames */\n")
        fighting_frames = [
            ("MetaSpriteFrame_fightingCloud_frame0", 0),
            ("MetaSpriteFrame_fightingCloud_frame1", 1),
            ("MetaSpriteFrame_fightingCloud_frame2", 2),
            ("MetaSpriteFrame_fightingCloud_frame3", 3),
            ("MetaSpriteFrame_fightingCloud_frame4", 4),
        ]
        for frame_name, index in fighting_frames:
            const_name = frame_name.replace('MetaSpriteFrame_', '').upper()
            f.write(f"#define {const_name} {index}\n")
        
        f.write("\n/* Left/Right frames */\n")
        lr_frames = [
            ("MetaSpriteFrame_leftRight_left", 0),
            ("MetaSpriteFrame_leftRight_right", 1),
        ]
        for frame_name, index in lr_frames:
            const_name = frame_name.replace('MetaSpriteFrame_', '').upper()
            f.write(f"#define {const_name} {index}\n")
        
        # Note: Frame table arrays are static, so each translation unit gets its own copy
        # This is fine for const data and avoids multiple definition errors
        # No extern declarations needed since arrays are static
        
        f.write("\n#endif // METASPRITE_DATA_H\n")

def generate_resources_header(resources_dir: Path, verbose: bool = True) -> bool:
    """
    Auto-generate resources.h by scanning all generated resource header files.
    
    Args:
        resources_dir: Base resources directory
        verbose: If True, print progress messages
    
    Returns:
        True if generation successful, False otherwise
    """
    resources_h_path = resources_dir.parent / "resources.h"
    
    if verbose:
        print(f"Generating {resources_h_path.name}...")
    
    # Scan for all generated .h files in subdirectories
    resource_declarations = []
    
    # Scan images4bpp directory
    images_dir = resources_dir / "images4bpp"
    if images_dir.exists():
        for h_file in images_dir.glob("*.h"):
            if h_file.name != "resources.h":  # Skip if there's a resources.h in subdir
                base_name = h_file.stem
                # Convert to camelCase
                var_name_parts = base_name.replace('-', '_').split('_')
                var_base = var_name_parts[0] + ''.join(word.capitalize() for word in var_name_parts[1:])
                
                # Check if file has Map (background image)
                with open(h_file, 'r') as f:
                    content = f.read()
                    if f'const unsigned short {var_base}Map' in content:
                        resource_declarations.append(('Background', var_base, True))  # Has map
                    else:
                        resource_declarations.append(('Image', var_base, False))
    
    # Scan tiles4bpp directory
    tiles_dir = resources_dir / "tiles4bpp"
    if tiles_dir.exists():
        for h_file in tiles_dir.glob("*.h"):
            base_name = h_file.stem
            var_name_parts = base_name.replace('-', '_').split('_')
            var_base = var_name_parts[0] + ''.join(word.capitalize() for word in var_name_parts[1:])
            resource_declarations.append(('Tiles', var_base, False))
    
    # Scan metasprites directory (PCX files, not the metasprite_data.h)
    metasprites_dir = resources_dir / "metasprites"
    if metasprites_dir.exists():
        for h_file in metasprites_dir.glob("*.h"):
            if h_file.name != "metasprite_data.h" and h_file.name != "elevator.metasprite.h":
                base_name = h_file.stem
                var_name_parts = base_name.replace('-', '_').split('_')
                var_base = var_name_parts[0] + ''.join(word.capitalize() for word in var_name_parts[1:])
                resource_declarations.append(('Metasprite', var_base, False))
    
    # Generate resources.h
    with open(resources_h_path, 'w') as f:
        f.write("#ifndef RESOURCES_H\n")
        f.write("#define RESOURCES_H\n\n")
        f.write("// Auto-generated by convert_resources.py\n")
        f.write("// Forward declarations for resource data\n")
        f.write("// The actual definitions are in the individual .h files\n")
        f.write("// Include them directly in files that need them to avoid multiple definition errors\n\n")
        
        # Group by category
        background_resources = [r for r in resource_declarations if r[0] == 'Background']
        image_resources = [r for r in resource_declarations if r[0] == 'Image']
        tiles_resources = [r for r in resource_declarations if r[0] == 'Tiles']
        metasprite_resources = [r for r in resource_declarations if r[0] == 'Metasprite']
        
        if background_resources:
            f.write("// Background resources\n")
            for category, var_base, has_map in background_resources:
                f.write(f"extern const unsigned short {var_base}Palette[];\n")
                f.write(f"extern const unsigned char {var_base}Tiles[];\n")
                if has_map:
                    f.write(f"extern const unsigned short {var_base}Map[];\n")
            f.write("\n")
        
        if image_resources:
            f.write("// Image resources\n")
            for category, var_base, has_map in image_resources:
                f.write(f"extern const unsigned short {var_base}Palette[];\n")
                f.write(f"extern const unsigned char {var_base}Tiles[];\n")
                if has_map:
                    f.write(f"extern const unsigned short {var_base}Map[];\n")
            f.write("\n")
        
        if tiles_resources:
            f.write("// Tiles resources\n")
            for category, var_base, has_map in tiles_resources:
                f.write(f"extern const unsigned short {var_base}Palette[];\n")
                f.write(f"extern const unsigned char {var_base}Tiles[];\n")
            f.write("\n")
        
        if metasprite_resources:
            f.write("// Metasprite resources\n")
            for category, var_base, has_map in metasprite_resources:
                f.write(f"extern const unsigned short {var_base}Palette[];\n")
                f.write(f"extern const unsigned char {var_base}Tiles[];\n")
            f.write("\n")
        
        f.write("// Size constants are defined in the individual generated header files\n")
        f.write("// Include the specific resource headers to access their size constants:\n")
        f.write("// - resources/images4bpp/background.h defines BACKGROUNDTILES_SIZE, BACKGROUNDMAP_SIZE, etc.\n")
        f.write("// - resources/images4bpp/splash.h defines SPLASHTILES_SIZE, SPLASHMAP_SIZE, etc.\n")
        f.write("// - resources/images4bpp/controls.h defines CONTROLSTILES_SIZE, CONTROLSMAP_SIZE, etc.\n")
        f.write("// - etc.\n\n")
        
        f.write("// Metasprite frame constants (forward declarations - include metasprite_data.h in files that need frame tables)\n")
        f.write("// Frame index constants are defined in resources/metasprites/metasprite_data.h\n")
        f.write("// Frame tables: player[], businessMan[], fightingCloud[], etc.\n\n")
        
        f.write("#endif // RESOURCES_H\n")
    
    if verbose:
        print(f"  Generated: {resources_h_path}")
    
    return True


def convert_metasprites(resources_dir: Path, verbose: bool = True) -> bool:
    """
    Convert metasprite assembly files to C format.
    
    Args:
        resources_dir: Base resources directory
        verbose: If True, print progress messages
    
    Returns:
        True if conversion successful, False otherwise
    """
    metasprite_dir = resources_dir / "metasprites"
    data_file = metasprite_dir / "elevator.metasprite-data.inc"
    table_file = metasprite_dir / "elevator.metasprite-table.inc"
    output_file = metasprite_dir / "metasprite_data.h"
    
    if not data_file.exists():
        if verbose:
            print(f"Warning: {data_file} not found, skipping metasprite conversion")
        return True  # Not an error if file doesn't exist
    
    if not table_file.exists():
        if verbose:
            print(f"Warning: {table_file} not found, skipping metasprite conversion")
        return True  # Not an error if file doesn't exist
    
    if verbose:
        print(f"Converting metasprites...")
        print(f"  Parsing {data_file.name}...")
    
    frames = parse_metasprite_data(data_file)
    
    if verbose:
        print(f"  Found {len(frames)} metasprite frames")
        print(f"  Parsing {table_file.name}...")
    
    tables = parse_metasprite_table(table_file)
    
    if verbose:
        print(f"  Found {len(tables)} frame tables")
        print(f"  Generating {output_file.name}...")
    
    generate_metasprite_header(output_file, frames, tables)
    
    if verbose:
        print(f"  Generated: {output_file}")
    
    return True


# ============================================================================
# CONFIGURATION
# ============================================================================

# Base directory for resources (relative to script location)
RESOURCES_DIR = Path(__file__).resolve().parent

# Directories containing PCX files to convert
# Each entry is a tuple: (subdirectory, pattern)
PCX_DIRECTORIES = [
    ("metasprites", "*.pcx"),
    ("tiles4bpp", "*.pcx"),
    ("images4bpp", "*.pcx"),
]

# PCX conversion settings (equivalent to pcx2snes -n -s8 -c16 -o16)
# -n = no header
# -s8 = sprite mode (8x8 tiles)
# -c16 = 16 colors
# -o16 = output format (16-color palette)
# These are now handled by the Python converter in shared/pythonUtils/pcx_converter.py


# ============================================================================
# CONVERSION FUNCTIONS
# ============================================================================

def get_pcx_files(resources_dir: Path) -> list:
    """
    Find all PCX files in the configured directories.
    
    Args:
        resources_dir: Base resources directory
    
    Returns:
        List of PCX file paths
    """
    pcx_files = []
    
    for subdir, pattern in PCX_DIRECTORIES:
        directory = resources_dir / subdir
        if directory.exists():
            matches = find_files_by_pattern(str(directory), pattern, recursive=False)
            pcx_files.extend(matches)
    
    return sorted(pcx_files)


def get_output_files(pcx_file: str) -> tuple:
    """
    Get the output file path for a PCX file.
    
    Args:
        pcx_file: Path to the PCX file
    
    Returns:
        Tuple of (base_name, header_file)
    """
    base_name = get_file_base_name(pcx_file)
    directory = get_file_directory(pcx_file)
    
    # Python converter generates .h header file
    header_file = os.path.join(directory, base_name + ".h")
    
    return (base_name, header_file)


def convert_pcx_to_snes(pcx_file: str, verbose: bool = True) -> bool:
    """
    Convert a PCX file to SNES format and generate a C header file.
    
    Uses the Python converter from shared/pythonUtils/pcx_converter.py.
    
    Args:
        pcx_file: Path to the PCX file to convert
        verbose: If True, print progress messages
    
    Returns:
        True if conversion successful, False otherwise
    """
    base_name, header_file = get_output_files(pcx_file)
    
    if verbose:
        print(f"Converting: {pcx_file}")
    
    # Use Python converter
    success = convert_pcx_to_snes_python(
        pcx_file,
        output_header_path=header_file,
        verbose=verbose
    )
    
    if success and verbose:
        print(f"  Generated: {header_file}")
    
    return success


def convert_all_resources(resources_dir: Path, verbose: bool = True) -> bool:
    """
    Convert all PCX files found in the configured directories.
    
    Args:
        resources_dir: Base resources directory
        verbose: If True, print progress messages
    
    Returns:
        True if all conversions successful, False otherwise
    """
    pcx_files = get_pcx_files(resources_dir)
    
    if not pcx_files:
        if verbose:
            print("No PCX files found to convert.")
        return True
    
    if verbose:
        print(f"Found {len(pcx_files)} PCX file(s) to convert:")
        for pcx_file in pcx_files:
            print(f"  - {pcx_file}")
        print()
    
    success_count = 0
    for pcx_file in pcx_files:
        if convert_pcx_to_snes(pcx_file, verbose=verbose):
            success_count += 1
        if verbose:
            print()
    
    if verbose:
        print(f"Converted {success_count}/{len(pcx_files)} file(s) successfully.")
    
    return success_count == len(pcx_files)


def clean_resources(resources_dir: Path, verbose: bool = True) -> int:
    """
    Remove all generated resource files (.h header files).
    
    Args:
        resources_dir: Base resources directory
        verbose: If True, print progress messages
    
    Returns:
        Number of files deleted
    """
    pcx_files = get_pcx_files(resources_dir)
    files_to_delete = []
    
    for pcx_file in pcx_files:
        _, header_file = get_output_files(pcx_file)
        if file_exists(header_file):
            files_to_delete.append(header_file)
    
    # Also clean metasprite header
    metasprite_header = resources_dir / "metasprites" / "metasprite_data.h"
    if file_exists(str(metasprite_header)):
        files_to_delete.append(str(metasprite_header))
    
    # Also clean resources.h (auto-generated)
    resources_h = resources_dir.parent / "resources.h"
    if file_exists(str(resources_h)):
        files_to_delete.append(str(resources_h))
    
    if not files_to_delete:
        if verbose:
            print("No generated files found to clean.")
        return 0
    
    if verbose:
        print(f"Deleting {len(files_to_delete)} file(s):")
        for filepath in files_to_delete:
            print(f"  - {filepath}")
    
    deleted_count = delete_files(files_to_delete)
    
    if verbose:
        print(f"Deleted {deleted_count} file(s).")
    
    return deleted_count


# ============================================================================
# MAIN ENTRY POINT
# ============================================================================

def main():
    """Main entry point for the script."""
    parser = argparse.ArgumentParser(
        description="Convert PCX resources to SNES format (.4bpp and .clr)",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python convert_resources.py          # Clean and then convert all resources (default)
  python convert_resources.py --clean  # Only remove generated files, don't convert
  python convert_resources.py -v       # Verbose output
        """
    )
    
    parser.add_argument(
        "--clean",
        action="store_true",
        dest="clean_only",
        help="Only remove generated files, don't convert"
    )
    
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        default=True,
        help="Print verbose output (default: True)"
    )
    
    parser.add_argument(
        "-q", "--quiet",
        action="store_true",
        help="Suppress verbose output"
    )
    
    args = parser.parse_args()
    
    verbose = args.verbose and not args.quiet
    
    # Get resources directory
    resources_dir = RESOURCES_DIR
    
    if verbose:
        print(f"Resources directory: {resources_dir}")
        print()
    
    # Execute requested action
    if args.clean_only:
        # Only clean, don't convert
        deleted_count = clean_resources(resources_dir, verbose=verbose)
        sys.exit(0 if deleted_count >= 0 else 1)
    else:
        # Default behavior: clean first, then convert
        if verbose:
            print("Cleaning existing generated files...")
            print()
        
        clean_resources(resources_dir, verbose=verbose)
        
        if verbose:
            print()
            print("Converting resources...")
            print()
        
        success = convert_all_resources(resources_dir, verbose=verbose)
        
        if success:
            # Convert metasprites after PCX files
            if verbose:
                print()
                print("Converting metasprites...")
                print()
            
            convert_metasprites(resources_dir, verbose=verbose)
            
            # Generate resources.h after all conversions
            if verbose:
                print()
                print("Generating resources.h...")
                print()
            
            generate_resources_header(resources_dir, verbose=verbose)
        
        sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
