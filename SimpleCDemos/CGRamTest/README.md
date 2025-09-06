# snesCcompiler

SNES C Compiler Demo Project

## Building

This project now uses CMake instead of batch files for building.

### Prerequisites

- CMake 3.16 or later
- WDC 816 C Compiler tools in your PATH
- MinGW Makefiles (or your preferred CMake generator)

### Build Commands

#### Quick Build
```bash
# Windows
build.bat

# Or manually
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

#### Clean Build
```bash
# Windows
clean.bat

# Or manually
cmake --build . --target clean
```

### Project Structure

- `mainBankZero.c` - Main application code
- `kernel.c` - Kernel/heap configuration
- `vectors.asm` - SNES startup and interrupt vectors
- `lib/SNES.h` - SNES hardware register definitions
- `CMakeLists.txt` - CMake build configuration

### Output Files

The build process generates:
- `mainBankZero.obj` - Compiled main source
- `vectors.obj` - Assembled vectors
- `kernel.obj` - Compiled kernel
- `PROG.LINK` - Linker output file

### Include Paths

The project includes:
- `C:/wdc/Tools/include` - WDC compiler headers
- `../shared/src` - Shared SNES library headers
- `lib` - Local project headers
