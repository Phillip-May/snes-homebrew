// Minimal putchar stub for CC65
// This provides the __putchar function that sprintf requires internally
#include <stdio.h>

int __putchar(int c) {
    // Do nothing - this is just a stub to satisfy the linker
    // Potentially write to no$snes debug port and use a lua script for mesen
    return c;
}

// Alternative putchar implementation for CC65
int putchar(int c) {
    return __putchar(c);
}
