"""
Subprocess utility functions for SNES homebrew development.

Provides functions for running external commands safely with error handling.
"""

import subprocess
import sys
from typing import List, Optional, Tuple


def run_command(
    command: List[str],
    cwd: Optional[str] = None,
    check: bool = True,
    capture_output: bool = False,
    verbose: bool = True
) -> Tuple[bool, Optional[str], Optional[str]]:
    """
    Run a command and handle errors.
    
    Args:
        command: Command and arguments as a list (e.g., ['pcx2snes', '-n', '-s8'])
        cwd: Working directory for the command (None = current directory)
        check: If True, raise exception on non-zero return code
        capture_output: If True, capture stdout and stderr
        verbose: If True, print command and output
    
    Returns:
        Tuple of (success, stdout, stderr)
        - success: True if return code was 0, False otherwise
        - stdout: Captured stdout if capture_output=True, None otherwise
        - stderr: Captured stderr if capture_output=True, None otherwise
    """
    if verbose:
        cmd_str = ' '.join(command)
        print(f"Running: {cmd_str}")
        if cwd:
            print(f"  in directory: {cwd}")
    
    try:
        result = subprocess.run(
            command,
            cwd=cwd,
            check=check,
            capture_output=capture_output,
            text=True
        )
        
        if capture_output:
            if verbose and result.stdout:
                print(result.stdout)
            if verbose and result.stderr:
                print(result.stderr, file=sys.stderr)
            return (True, result.stdout, result.stderr)
        else:
            return (True, None, None)
    
    except subprocess.CalledProcessError as e:
        error_msg = f"Command failed with return code {e.returncode}"
        if e.stderr:
            error_msg += f": {e.stderr}"
        if verbose:
            print(error_msg, file=sys.stderr)
        return (False, e.stdout if capture_output else None, e.stderr if capture_output else None)
    
    except FileNotFoundError:
        error_msg = f"Command not found: {command[0]}"
        if verbose:
            print(error_msg, file=sys.stderr)
        return (False, None, error_msg)
    
    except Exception as e:
        error_msg = f"Error running command: {e}"
        if verbose:
            print(error_msg, file=sys.stderr)
        return (False, None, error_msg)


def find_executable(name: str, path: Optional[str] = None) -> Optional[str]:
    """
    Find an executable in the system PATH.
    
    Args:
        name: Name of the executable to find
        path: Optional PATH string (uses os.environ['PATH'] if None)
    
    Returns:
        Full path to executable if found, None otherwise
    """
    import shutil
    return shutil.which(name, path=path)


def check_command_available(command: str) -> bool:
    """
    Check if a command is available in the system PATH.
    
    Args:
        command: Name of the command to check
    
    Returns:
        True if command is available, False otherwise
    """
    return find_executable(command) is not None
