#!/usr/bin/env python3
"""
TCC816 (pvsneslib) compilation wrapper script
Copies source files to build directory and runs TCC816 from there
"""

import os
import sys
import subprocess
import argparse
import shutil

def main():
    print("TCC816 wrapper starting...")
    # Set up environment variables (similar to devEnv.bat)
    os.environ["DEV_KIT_SNES_PATH"] = r"C:\pvsneslib\devkitsnes\bin"
    os.environ["C_INC_PATH"] = r"C:\pvsneslib\devkitsnes\include"
    os.environ["TOOLS_PATH"] = r"C:\pvsneslib\devkitsnes\tools"
    os.environ["PVSNESLIB_HOME"] = r"C:\pvsneslib"
    
    # Update PATH
    os.environ["PATH"] = os.environ["DEV_KIT_SNES_PATH"] + os.pathsep + os.environ["PATH"]
    os.environ["PATH"] = os.environ["C_INC_PATH"] + os.pathsep + os.environ["PATH"]
    os.environ["PATH"] = os.environ["TOOLS_PATH"] + os.pathsep + os.environ["PATH"]
    
    # Get the current working directory (where make was called from)
    work_dir = os.getcwd()
    
    # Create build directory
    build_dir = os.path.join(work_dir, "build")
    os.makedirs(build_dir, exist_ok=True)
    
    # Parse arguments
    parser = argparse.ArgumentParser(description='TCC816 compilation wrapper')
    parser.add_argument('-c', '--compile', action='store_true', help='Compile only')
    parser.add_argument('-l', '--linker', help='Linker script (ignored for TCC816)')
    parser.add_argument('-O', '--optimize', help='Optimization level')
    parser.add_argument('-D', '--debug', help='Debug level')
    parser.add_argument('-V', '--verbose', help='Verbose level')
    parser.add_argument('-r', '--result', help='Result directory')
    parser.add_argument('-o', '--output', help='Output file')
    parser.add_argument('sources', nargs='*', help='Source files')
    
    # Parse known args (ignore unknown ones for now)
    args, unknown = parser.parse_known_args()
    
    # Copy source files to build directory - flatten all paths to just filenames
    copied_sources = []
    
    # Process both known sources and unknown arguments that might be source files
    all_sources = args.sources + [arg for arg in unknown if not arg.startswith('-') and arg.endswith('.c')]
    
    for source in all_sources:
        # Skip if this looks like an output file path
        if source.endswith('.o') and 'build/' in source:
            continue
            
        # Skip if this is a directory (like ../shared/src)
        if os.path.isdir(os.path.normpath(os.path.join(work_dir, source))):
            continue
            
        # Handle relative paths properly using os.path.normpath
        source_path = os.path.normpath(os.path.join(work_dir, source))
            
        if os.path.exists(source_path) and os.path.isfile(source_path):
            # Always flatten to just the filename in build directory
            dest_filename = os.path.basename(source)
            dest_path = os.path.join(build_dir, dest_filename)
            
            # Copy the file
            shutil.copy2(source_path, dest_path)
            copied_sources.append(dest_filename)  # Use just filename for TCC816
            print(f"Copied: {source} -> {dest_path}")
        else:
            print(f"Warning: Source file not found: {source_path}")
    
    # Copy header files from shared/src to build directory for TCC816
    header_files = ["snes_regs_xc.h", "int_snes_xc.h"]
    for header_file in header_files:
        header_path = os.path.normpath(os.path.join(work_dir, "..", "shared", "src", header_file))
        if os.path.exists(header_path):
            dest_path = os.path.join(build_dir, header_file)
            shutil.copy2(header_path, dest_path)
            print(f"Copied: {header_file} -> {dest_path}")
        else:
            print(f"Warning: {header_file} not found at {header_path}")
    
    # Build TCC816 command
    tcc816_path = os.path.join(os.environ["DEV_KIT_SNES_PATH"], "816-tcc.exe")
    cmd = [tcc816_path]
    
    # Add include paths for pvsneslib
    cmd.extend(["-I" + os.path.join(os.environ["PVSNESLIB_HOME"], "pvsneslib", "include")])
    cmd.extend(["-I" + os.environ["C_INC_PATH"]])
    
    # Process include path arguments from Makefile
    i = 0
    while i < len(unknown):
        if unknown[i] == "-I" and i + 1 < len(unknown):
            # Convert relative paths to absolute paths
            include_path = os.path.normpath(os.path.join(work_dir, unknown[i + 1]))
            cmd.extend(["-I", include_path])
            i += 2
        elif unknown[i].startswith("-I"):
            # Handle -Ipath format (no space)
            include_path = os.path.normpath(os.path.join(work_dir, unknown[i][2:]))
            cmd.extend(["-I", include_path])
            i += 1
        else:
            i += 1
    
    # Define TCC816 macro
    cmd.extend(["-D__TCC816__"])
    
    # Add arguments
    if args.optimize:
        # TCC816 doesn't have standard optimization flags, skip for now
        pass
    
    if args.debug:
        # TCC816 doesn't have standard debug flags, skip for now
        pass
    
    if args.verbose:
        cmd.extend(["-v"])
    
    # Add compile-only flag
    cmd.extend(["-c"])
    
    # Add output file if specified
    if args.output:
        cmd.extend(["-o", os.path.basename(args.output)])
    
    # Add source files with just filenames (all in build directory root)
    for source in copied_sources:
        cmd.append(source)
    
    # Print the command for debugging
    print("Running:", " ".join(cmd))
    print(f"Working directory: {build_dir}")
    
    # Run TCC816 from the build directory
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, cwd=build_dir)
        
        if result.returncode != 0:
            print("STDOUT:", result.stdout)
            print("STDERR:", result.stderr)
            sys.exit(result.returncode)
        
        print("TCC816 compilation completed successfully")
        
        # Convert assembly files to object files and link
        asm_files = [f for f in os.listdir(build_dir) if f.endswith('.o') and not f.endswith('.obj')]
        if asm_files and not args.output:
            print("Converting assembly to object files...")
            convert_and_link(build_dir, asm_files)
        
    except Exception as e:
        print("Error running TCC816:", e)
        sys.exit(1)

def convert_and_link(build_dir, asm_files):
    """Convert assembly files to object files and link into a SNES ROM"""
    try:
        # Copy header file from pvsneslib examples
        port_dir = os.path.dirname(os.path.abspath(__file__))
        hdr_source = os.path.join(port_dir, "hdr.asm")
        hdr_dest = os.path.join(build_dir, "hdr.asm")
        
        if os.path.exists(hdr_source):
            shutil.copy2(hdr_source, hdr_dest)
            print(f"Copied header: {hdr_source} -> {hdr_dest}")
        else:
            # Fallback to generated header if pvsneslib header not found
            hdr_content = ''';==LoRom==                      ; We'll get to HiRom some other time.

.MEMORYMAP                      ; Begin describing the system architecture.
  SLOTSIZE $8000                ; The slot is $8000 bytes in size. More details on slots later.
  DEFAULTSLOT 0                 ; There's only 1 slot in SNES, there are more in other consoles.
  SLOT 0 $8000                  ; Defines Slot 0's starting address.
  SLOT 1 $0 $2000
  SLOT 2 $2000 $E000
  SLOT 3 $0 $10000
.ENDME          ; End MemoryMap definition

.ROMBANKSIZE $8000              ; Every ROM bank is 32 KBytes in size
.ROMBANKS 8                     ; 2 Mbits - Tell WLA we want to use 8 ROM Banks

.SNESHEADER
  ID "SNES"                     ; 1-4 letter string, just leave it as "SNES"

  NAME "TCC816 TEST ROM      "  ; Program Title - can't be over 21 bytes,
  ;    "123456789012345678901"  ; use spaces for unused bytes of the name.

  SLOWROM
  LOROM

  CARTRIDGETYPE $00             ; $00=ROM, $01=ROM+RAM, $02=ROM+SRAM, $03=ROM+DSP1, $04=ROM+RAM+DSP1, $05=ROM+SRAM+DSP1, $13=ROM+Super FX
  ROMSIZE $08                   ; $08=2 Megabits, $09=4 Megabits,$0A=8 Megabits,$0B=16 Megabits,$0C=32 Megabits
  SRAMSIZE $00                  ; $00=0 kilobits, $01=16 kilobits, $02=32 kilobits, $03=64 kilobits
  COUNTRY $01                   ; $01= U.S., $00=Japan, $02=Europe, $03=Sweden/Scandinavia, $04=Finland, $05=Denmark, $06=France, $07=Netherlands, $08=Spain, $09=Germany, $0A=Italy, $0B=China, $0C=Indonesia, $0D=Korea
  LICENSEECODE $00              ; Just use $00
  VERSION $00                   ; $00 = 1.00, $01 = 1.01, etc.
.ENDSNES

.SNESNATIVEVECTOR               ; Define Native Mode interrupt vector table
  COP EmptyHandler
  BRK EmptyHandler
  ABORT EmptyHandler
  NMI VBlank
  IRQ EmptyHandler
.ENDNATIVEVECTOR

.SNESEMUVECTOR                  ; Define Emulation Mode interrupt vector table
  COP EmptyHandler
  ABORT EmptyHandler
  NMI EmptyHandler
  RESET tcc__start                   ; where execution starts
  IRQBRK EmptyHandler
.ENDEMUVECTOR

EmptyHandler:
  rti

VBlank:
  rti

.BANK 0 SLOT 0
.ORG $0000
'''
            hdr_path = os.path.join(build_dir, "hdr.asm")
            with open(hdr_path, 'w') as f:
                f.write(hdr_content)
        
        # Convert each assembly file to object file
        wla_path = os.path.join(os.environ["DEV_KIT_SNES_PATH"], "wla-65816.exe")
        obj_files = []
        
        # First assemble hdr.asm
        hdr_obj = "hdr.obj"
        hdr_cmd = [wla_path, "-d", "-s", "-x", "-o", hdr_obj, "hdr.asm"]
        print(f"Assembling hdr.asm...")
        result = subprocess.run(hdr_cmd, capture_output=True, text=True, cwd=build_dir)
        if result.returncode == 0:
            obj_files.append(hdr_obj)
            print("hdr.asm assembled successfully")
        else:
            print("hdr.asm assembly failed:", result.stderr)
        
        
        for asm_file in asm_files:
            obj_file = asm_file.replace('.o', '.obj')
            asm_cmd = [wla_path, "-d", "-s", "-x", "-o", obj_file, asm_file]
            
            print(f"Assembling {asm_file}...")
            result = subprocess.run(asm_cmd, capture_output=True, text=True, cwd=build_dir)
            
            if result.returncode != 0:
                print(f"Assembly STDOUT: {result.stdout}")
                print(f"Assembly STDERR: {result.stderr}")
                print(f"Failed to assemble {asm_file}")
                continue
                
            obj_files.append(obj_file)
        
        if obj_files:
            print("Linking object files to create ROM...")
            link_rom(build_dir, obj_files)
        else:
            print("No object files created successfully")
            
    except Exception as e:
        print(f"Error during conversion and linking: {e}")

def link_rom(build_dir, obj_files):
    """Link object files into a SNES ROM using WLA-65816"""
    try:
        # Create linkfile
        linkfile_path = os.path.join(build_dir, "linkfile")
        with open(linkfile_path, 'w') as f:
            f.write("[objects]\n")
            for obj_file in obj_files:
                f.write(f"{obj_file}\n")
            
            
            # Add pvsneslib library objects
            lib_dir = os.path.join(os.environ["PVSNESLIB_HOME"], "pvsneslib", "lib", "LoROM_SlowROM")
            if os.path.exists(lib_dir):
                for lib_file in os.listdir(lib_dir):
                    if lib_file.endswith('.obj'):
                        f.write(f"{lib_dir}/{lib_file}\n")
        
        # Link using wlalink
        wlalink_path = os.path.join(os.environ["DEV_KIT_SNES_PATH"], "wlalink.exe")
        rom_filename = "mainBankZero_tcc816.sfc"
        link_cmd = [wlalink_path, "-d", "-s", "-v", "-A", "-c", "-L", lib_dir, "linkfile", rom_filename]
        
        print("Linking command:", " ".join(link_cmd))
        result = subprocess.run(link_cmd, capture_output=True, text=True, cwd=build_dir)
        
        if result.returncode != 0:
            print("Link STDOUT:", result.stdout)
            print("Link STDERR:", result.stderr)
            print("Linking failed, but object files were created successfully")
        else:
            print(f"ROM created successfully: {rom_filename}")
            
    except Exception as e:
        print(f"Error during linking: {e}")
        print("Object files were created successfully")

if __name__ == "__main__":
    main()
