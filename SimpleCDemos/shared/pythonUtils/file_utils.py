"""
File utility functions for SNES homebrew development.

Provides functions for file operations like finding files, renaming, deleting,
and path manipulation.
"""

import os
import glob
from pathlib import Path
from typing import List, Optional


def find_files_by_pattern(directory: str, pattern: str, recursive: bool = True) -> List[str]:
    """
    Find files matching a pattern in a directory.
    
    Args:
        directory: Base directory to search in
        pattern: Glob pattern (e.g., '*.pcx', 'metasprites/*.pcx')
        recursive: If True, search recursively in subdirectories
    
    Returns:
        List of matching file paths (relative to directory or absolute)
    """
    if not os.path.isdir(directory):
        return []
    
    search_pattern = os.path.join(directory, '**', pattern) if recursive else os.path.join(directory, pattern)
    matches = glob.glob(search_pattern, recursive=recursive)
    return sorted(matches)


def get_file_base_name(filepath: str) -> str:
    """
    Get the base name of a file without extension.
    
    Args:
        filepath: Full path to the file
    
    Returns:
        Base name without extension (e.g., '/path/to/file.pcx' -> 'file')
    """
    return os.path.splitext(os.path.basename(filepath))[0]


def rename_file(old_path: str, new_path: str) -> bool:
    """
    Rename or move a file.
    
    Args:
        old_path: Current file path
        new_path: New file path
    
    Returns:
        True if successful, False otherwise
    """
    try:
        if os.path.exists(old_path):
            os.rename(old_path, new_path)
            return True
        return False
    except OSError as e:
        print(f"Error renaming {old_path} to {new_path}: {e}")
        return False


def delete_file(filepath: str) -> bool:
    """
    Delete a file.
    
    Args:
        filepath: Path to the file to delete
    
    Returns:
        True if successful, False otherwise
    """
    try:
        if os.path.exists(filepath):
            os.remove(filepath)
            return True
        return False
    except OSError as e:
        print(f"Error deleting {filepath}: {e}")
        return False


def delete_files(filepaths: List[str]) -> int:
    """
    Delete multiple files.
    
    Args:
        filepaths: List of file paths to delete
    
    Returns:
        Number of files successfully deleted
    """
    count = 0
    for filepath in filepaths:
        if delete_file(filepath):
            count += 1
    return count


def ensure_directory(directory: str) -> bool:
    """
    Ensure a directory exists, creating it if necessary.
    
    Args:
        directory: Path to the directory
    
    Returns:
        True if directory exists or was created, False otherwise
    """
    try:
        os.makedirs(directory, exist_ok=True)
        return True
    except OSError as e:
        print(f"Error creating directory {directory}: {e}")
        return False


def get_file_directory(filepath: str) -> str:
    """
    Get the directory containing a file.
    
    Args:
        filepath: Path to a file
    
    Returns:
        Directory path
    """
    return os.path.dirname(os.path.abspath(filepath))


def file_exists(filepath: str) -> bool:
    """
    Check if a file exists.
    
    Args:
        filepath: Path to the file
    
    Returns:
        True if file exists, False otherwise
    """
    return os.path.isfile(filepath)


def change_extension(filepath: str, new_extension: str) -> str:
    """
    Change the extension of a file path.
    
    Args:
        filepath: Original file path
        new_extension: New extension (with or without leading dot)
    
    Returns:
        File path with new extension
    """
    if not new_extension.startswith('.'):
        new_extension = '.' + new_extension
    
    base = os.path.splitext(filepath)[0]
    return base + new_extension
