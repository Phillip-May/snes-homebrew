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
# CONFIGURATION
# ============================================================================

# Base directory for resources (relative to script location)
RESOURCES_DIR = Path(__file__).resolve().parent

# Directories containing PCX files to convert
# Each entry is a tuple: (subdirectory, pattern)
PCX_DIRECTORIES = [
    ("metasprites", "*.pcx"),
    # Add more directories here as needed, e.g.:
    # ("tiles4bpp", "*.pcx"),
    # ("images4bpp", "*.pcx"),
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
        sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
