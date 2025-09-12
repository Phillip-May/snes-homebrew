// Stub implementations for missing functions on SNES

#include <stdio.h>
#include <stdint.h>

// Standard library stubs
int fileno(FILE *stream) {
    // Not supported on SNES
    return -1;
}

// Device manager stubs
void* dm_getaddr(int fd) {
    // Not supported on SNES
    return NULL;
}

// eLua platform functions
int platform_cpu_set_global_interrupts(int status) {
    return 0; // Always return enabled state
}

int platform_cpu_get_global_interrupts(void) {
    return 1; // Always return enabled
}

// eLua interrupt functions
int elua_int_add(int inttype, int resnum) {
    return 0; // Not supported
}

void elua_int_cleanup(void) {
    // Nothing to clean up
}

void elua_int_enable(int inttype) {
    // Not supported
}

void elua_int_disable(int inttype) {
    // Not supported
}

int elua_int_is_enabled(int inttype) {
    return 0; // Not supported
}

void elua_int_disable_all(void) {
    // Not supported
}

// linenoise functions
void linenoise_cleanup(int id) {
    // Nothing to clean up
}

// Linker symbols for text segment boundaries
char stext[1] = {0};
char etext[1] = {0};
