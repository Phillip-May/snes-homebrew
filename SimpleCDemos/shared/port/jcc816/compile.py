#!/usr/bin/env python3
"""
JCC816 compilation wrapper script
Copies source files to build directory and runs JCC816 from there
"""

import os
import sys
import subprocess
import argparse
import shutil

def main():
    # Set up environment variables (similar to devEnvWin.bat)
    os.environ["JAVA_PATH"] = r"C:\Users\Admin\Downloads\JCC816-main\openjdk-24.0.1_windows-x64_bin\jdk-24.0.1\bin"
    os.environ["CC65_PATH"] = r"C:\Users\Admin\Downloads\cc65-snapshot-win32\bin"
    os.environ["JCC816_PATH"] = r"C:\Users\Admin\Documents\JCC816"
    os.environ["C_INC_PATH"] = r"C:\Users\Admin\Documents\JCC816\src\stdlib"
    
    # Update PATH
    os.environ["PATH"] = os.environ["JAVA_PATH"] + os.pathsep + os.environ["PATH"]
    os.environ["PATH"] = os.environ["CC65_PATH"] + os.pathsep + os.environ["PATH"]
    os.environ["PATH"] = os.environ["JCC816_PATH"] + os.pathsep + os.environ["PATH"]
    
    # Get the current working directory (where make was called from)
    work_dir = os.getcwd()
    
    # Create build directory
    build_dir = os.path.join(work_dir, "build")
    os.makedirs(build_dir, exist_ok=True)
    
    # Parse arguments
    parser = argparse.ArgumentParser(description='JCC816 compilation wrapper')
    parser.add_argument('-l', '--linker', help='Linker script')
    parser.add_argument('-O', '--optimize', help='Optimization level')
    parser.add_argument('-D', '--debug', help='Debug level')
    parser.add_argument('-V', '--verbose', help='Verbose level')
    parser.add_argument('-r', '--result', help='Result directory')
    parser.add_argument('sources', nargs='*', help='Source files')
    
    # Parse known args (ignore unknown ones for now)
    args, unknown = parser.parse_known_args()
    
    # Copy source files to build directory - flatten all paths to just filenames
    copied_sources = []
    for source in args.sources:
        source_path = os.path.join(work_dir, source)
        if os.path.exists(source_path):
            # Always flatten to just the filename in build directory
            dest_filename = os.path.basename(source)
            dest_path = os.path.join(build_dir, dest_filename)
            
            # Copy the file
            shutil.copy2(source_path, dest_path)
            copied_sources.append(dest_filename)  # Use just filename for JCC816
            print(f"Copied: {source} -> {dest_path}")
        else:
            print(f"Warning: Source file not found: {source_path}")
    
    # Copy header files that the source code needs
    header_files = [
        ("SNES.h", [os.path.join(work_dir, "SNES.h")]),
        ("int.h", [os.path.join(work_dir, "..", "shared", "src", "int.h")]),
        ("initsnes.h", [os.path.join(work_dir, "..", "shared", "src", "initsnes.h")])
    ]
    
    for header_name, header_paths in header_files:
        copied = False
        for header_path in header_paths:
            if os.path.exists(header_path):
                dest_header = os.path.join(build_dir, header_name)
                shutil.copy2(header_path, dest_header)
                print(f"Copied: {header_name} -> {dest_header}")
                copied = True
                break
        
        if not copied:
            print(f"Warning: {header_name} not found")
    
    # Copy the linker script to build directory
    linker_script_path = None
    if args.linker and "example=" in args.linker:
        header_path = args.linker.split("example=")[1]
        source_header = os.path.join(work_dir, header_path)
        if os.path.exists(source_header):
            dest_header = os.path.join(build_dir, "exampleHeader.xml")
            shutil.copy2(source_header, dest_header)
            linker_script_path = "exampleHeader.xml"  # Relative to build directory
            print(f"Copied linker script: {header_path} -> {dest_header}")
        else:
            print(f"Warning: Linker script not found: {source_header}")
    
    # Build JCC816 Java command
    java_path = os.path.join(os.environ["JAVA_PATH"], "java.exe")
    jcc816_jar = os.path.join(os.environ["JCC816_PATH"], "JCC816.jar")
    cmd = [java_path, "-jar", jcc816_jar]
    
    # Add arguments - all relative to build directory now
    if linker_script_path:
        cmd.extend(["-l=example=" + linker_script_path])
    
    if args.optimize:
        cmd.extend(["-O=" + args.optimize])
    
    if args.debug:
        cmd.extend(["-D=" + args.debug])
    
    if args.verbose:
        cmd.extend(["-V=" + args.verbose])
    
    # Result directory is just "." since we're running from build directory
    cmd.extend(["-r=."])
    
    # Add source files with just filenames (all in build directory root)
    for source in copied_sources:
        cmd.append(source)
    
    # Print the command for debugging
    print("Running:", " ".join(cmd))
    print(f"Working directory: {build_dir}")
    
    # Run JCC816 from the build directory
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, cwd=build_dir)
        
        if result.returncode != 0:
            print("STDOUT:", result.stdout)
            print("STDERR:", result.stderr)
            sys.exit(result.returncode)
        
        # Handle output file - delete existing file first to avoid rename error
        example_sfc = os.path.join(build_dir, "example.sfc")
        output_sfc = os.path.join(build_dir, "mainBankZero.sfc")
        
        if os.path.exists(example_sfc):
            # Delete existing output file if it exists
            if os.path.exists(output_sfc):
                os.remove(output_sfc)
                print(f"Removed existing output file: {output_sfc}")
            
            # Rename the new output file
            os.rename(example_sfc, output_sfc)
            print("Output:", output_sfc)
        
    except Exception as e:
        print("Error running JCC816:", e)
        sys.exit(1)

if __name__ == "__main__":
    main()