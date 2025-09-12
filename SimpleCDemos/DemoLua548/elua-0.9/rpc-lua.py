import os, sys, platform

output = 'luarpc'
cdefs = "-DLUA_CROSS_COMPILER -DLUA_RPC"

# Lua source files and include path
lua_files = """lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c
   lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c lbaselib.c
   ldblib.c liolib.c lmathlib.c loslib.c ltablib.c lstrlib.c loadlib.c linit.c lua.c print.c lrotable.c"""
lua_full_files = " " + " ".join( [ "src/lua/%s" % name for name in lua_files.split() ] )
lua_full_files += " src/modules/luarpc.c src/modules/lpack.c src/modules/bitarray.c src/modules/bit.c src/luarpc_desktop_serial.c "

external_libs = '-lm'

if platform.system() == "Windows":
  lua_full_files += " src/serial/serial_win32.c"
  cdefs += " -DWIN32_BUILD"
else:
  lua_full_files += " src/serial/serial_posix.c src/linenoise_posix.c"
  cdefs += " -DLUA_USE_LINENOISE "

local_include = "-Isrc/lua -Iinc -Isrc/modules -Iinc/desktop"

# Compiler/linker options
cccom = "gcc -O2 -g %s -Wall %s -c $SOURCE -o $TARGET" % ( local_include, cdefs )
linkcom = "gcc -o $TARGET $SOURCES %s" % external_libs

# Env for building the program
comp = Environment( CCCOM = cccom,
                    LINKCOM = linkcom,
                    ENV = os.environ )
# Debug
Decider( 'MD5-timestamp' )                  
Default( comp.Program( output, Split( lua_full_files ) ) )
