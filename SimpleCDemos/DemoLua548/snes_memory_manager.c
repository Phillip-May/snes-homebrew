#include "snes_memory_manager.h"
#include <string.h>
#include <stdio.h>

/* SA-1 SRAM address range macros - internal to this file only */
#define SA1_SRAM_START ((uint8_t*)0x400000)
#define SA1_SRAM_END   ((uint8_t*)0x500000)
#define SA1_SRAM_MAX_TEST ((uint8_t*)0x500000)  /* Maximum address to test for SRAM detection */
#define SA1_SRAM_FALLBACK_END ((uint8_t*)0xABCDEF00)  /* Fallback end if all tests pass */

/* SA-1 SRAM test configuration macros - internal to this file only */
#define SA1_SRAM_TEST_STEP_SIZE 0x1000      /* 4KB step size for boundary testing */
#define SA1_SRAM_MIRROR_TEST_SIZE 0x8000    /* 32KB step size for mirror detection */
#define SA1_SRAM_INITIAL_TEST_SIZE 0x1000   /* Initial test range size */
#define SA1_SRAM_INITIAL_TEST_STEP 0x100    /* Step size for initial test */

/* Memory pools - only pool3 (SA-1 SRAM) is used */

/* Global memory manager instance */
snes_memory_manager_t g_mem_manager;

/* Simple memory block header for tracking allocations */
typedef struct {
    uint32_t size;    /* Use uint32_t instead of size_t for consistency */
    uint8_t pool_id;  /* 3 for pool3 (SA-1 SRAM) */
    uint8_t is_free;
} mem_block_header_t;

#define HEADER_SIZE sizeof(mem_block_header_t)

/* Test if SA-1 SRAM range is writable and find the actual end address */
int snes_test_sa1_sram(void) {
    volatile uint8_t *sram_start = (volatile uint8_t*)SA1_SRAM_START;
    volatile uint8_t *sram_end = (volatile uint8_t*)SA1_SRAM_END;
    
    /* Test pattern for write/read verification */
    uint8_t test_pattern = 0xAA;
    uint8_t test_pattern2 = 0x55;
    
    /* Test a few locations in the SRAM range */
    for (volatile uint8_t *ptr = sram_start; ptr < sram_start + SA1_SRAM_INITIAL_TEST_SIZE; ptr += SA1_SRAM_INITIAL_TEST_STEP) {
        /* Save original value */
        uint8_t original = *ptr;
        
        /* Test write/read with first pattern */
        *ptr = test_pattern;
        if (*ptr != test_pattern) {
            return 0;  /* Not writable */
        }
        
        /* Test write/read with second pattern */
        *ptr = test_pattern2;
        if (*ptr != test_pattern2) {
            return 0;  /* Not writable */
        }
        
        /* Restore original value */
        *ptr = original;
    }
    
    return 1;  /* SRAM is writable */
}

/* Find the actual end of SA-1 SRAM by testing writability and detecting mirrors */
uint8_t* snes_find_sa1_sram_end(void) {
    volatile uint8_t *sram_start = (volatile uint8_t*)SA1_SRAM_START;
    volatile uint8_t *test_ptr = sram_start;
    uint8_t test_pattern = 0xAA;
    uint8_t test_pattern2 = 0x55;
    uint8_t mirror_test_pattern = 0x33;
    
    /* Test every 4KB boundary to find where SRAM ends */
    while (test_ptr < (volatile uint8_t*)SA1_SRAM_MAX_TEST) {  /* Don't go beyond reasonable range */
        /* Save original value */
        uint8_t original = *test_ptr;
        
        /* Test write/read with first pattern */
        *test_ptr = test_pattern;
        if (*test_ptr != test_pattern) {
            /* Not writable - we found the end */
            *test_ptr = original;  /* Restore original value */
            return (uint8_t*)test_ptr;
        }
        
        /* Test write/read with second pattern */
        *test_ptr = test_pattern2;
        if (*test_ptr != test_pattern2) {
            /* Not writable - we found the end */
            *test_ptr = original;  /* Restore original value */
            return (uint8_t*)test_ptr;
        }
        
        /* Test for mirroring: write a unique pattern and check if it affects the start */
        /* Only test for mirroring every 32KB to avoid false positives, but skip the very first iteration */
        if (((test_ptr - sram_start) % SA1_SRAM_MIRROR_TEST_SIZE == 0) && (test_ptr != sram_start)) {
            *test_ptr = mirror_test_pattern;
            
            /* Check if this write affected the start of SRAM (indicating mirroring) */
            if (*sram_start == mirror_test_pattern) {
                /* This is a mirror of the start - we found the actual end */
                *test_ptr = original;  /* Restore original value */
                *sram_start = test_pattern;  /* Restore start to original pattern */
                return (uint8_t*)test_ptr;
            }
        }
        
        /* Restore original value */
        *test_ptr = original;
        
        /* Move to next 4KB boundary */
        test_ptr += SA1_SRAM_TEST_STEP_SIZE;
    }
    
    /* If we get here, we tested up to the maximum test range and it was all writable */
    /* Return the fallback end address */
    return SA1_SRAM_FALLBACK_END;
}

/* Initialize memory manager */
void snes_memory_init(void) {
    extern void termM0PrintStringXY_scroll(const char* str, int x, int y);
    static int debug_line = 90;
    char ptr_str[32];
    
    /* Test and initialize SA-1 SRAM pool */
    g_mem_manager.pool3_available = 0;
    int sa1_test_result = snes_test_sa1_sram();
    if (sa1_test_result) {
        /* Try using local variable first */
        uint8_t *local_ptr = (uint8_t*)0x400000;
        unsigned long local_val = (unsigned long)(uintptr_t)local_ptr;
        uint32_t local_high = (local_val >> 16) & 0xFFFF;
        uint32_t local_low = local_val & 0xFFFF;
        sprintf(ptr_str, "LOCAL: 0x%04X%04X", (unsigned int)local_high, (unsigned int)local_low);
        termM0PrintStringXY_scroll(ptr_str, 0, debug_line++);
        
        g_mem_manager.pool3_start = local_ptr;
        termM0PrintStringXY_scroll("SET: local var", 0, debug_line++);
        
        /* Debug: Check what was actually set */
        unsigned long set_val = (unsigned long)(uintptr_t)g_mem_manager.pool3_start;
        uint32_t set_high = (set_val >> 16) & 0xFFFF;
        uint32_t set_low = set_val & 0xFFFF;
        sprintf(ptr_str, "SET_RESULT: 0x%04X%04X", (unsigned int)set_high, (unsigned int)set_low);
        termM0PrintStringXY_scroll(ptr_str, 0, debug_line++);
        
        /* Automatically detect the actual end of SA-1 SRAM */
        g_mem_manager.pool3_end = snes_find_sa1_sram_end();
        g_mem_manager.pool3_current = g_mem_manager.pool3_start;
        
        /* Debug: Show what pool3_current was set to immediately after assignment */
        unsigned long pool3_current_val = (unsigned long)(uintptr_t)g_mem_manager.pool3_current;
        uint32_t pool3_current_high = (pool3_current_val >> 16) & 0xFFFF;
        uint32_t pool3_current_low = pool3_current_val & 0xFFFF;
        char ptr_str[32];
        sprintf(ptr_str, "POOL3_CURR: 0x%04X%04X", pool3_current_high, pool3_current_low);
        termM0PrintStringXY_scroll(ptr_str, 0, debug_line++);
        g_mem_manager.pool3_available = 1;
        
        /* Debug: Show what pool3_start was set to */
        unsigned long pool3_start_val = (unsigned long)(uintptr_t)g_mem_manager.pool3_start;
        uint32_t pool3_start_high = (pool3_start_val >> 16) & 0xFFFF;
        uint32_t pool3_start_low = pool3_start_val & 0xFFFF;
        sprintf(ptr_str, "POOL3_START: 0x%04X%04X", pool3_start_high, pool3_start_low);
        termM0PrintStringXY_scroll(ptr_str, 0, debug_line++);
        termM0PrintStringXY_scroll("SA1: start OK", 0, debug_line++);
        termM0PrintStringXY_scroll("SA1: end OK", 0, debug_line++);
        termM0PrintStringXY_scroll("SA1: current OK", 0, debug_line++);
    } else {
        g_mem_manager.pool3_start = NULL;
        g_mem_manager.pool3_end = NULL;
        g_mem_manager.pool3_current = NULL;
        
        /* Debug: Show SA-1 SRAM failure */
        extern void termM0PrintStringXY_scroll(const char* str, int x, int y);
        extern int current_line;
        static int debug_line = 90;
        termM0PrintStringXY_scroll("SA1: TEST FAILED!", 0, debug_line++);
    }
    
    g_mem_manager.total_allocated = 0;
    g_mem_manager.total_freed = 0;
}

/* Allocate memory from pool3 (SA-1 SRAM) only */
void* snes_malloc(size_t size) {
    /* Debug: Add external debug function declaration */
    extern void termM0PrintStringXY_scroll(const char* str, int x, int y);
    extern int current_line;
    static int debug_line = 70;
    
    if (size == 0) {
        termM0PrintStringXY_scroll("MALLOC: size=0", 0, debug_line++);
        return NULL;
    }
    
    /* Only use pool3 (SA-1 SRAM) */
    if (!g_mem_manager.pool3_available) {
        termM0PrintStringXY_scroll("MALLOC: pool3 not avail", 0, debug_line++);
        return NULL;
    }
    
    /* Check for heap corruption - verify pool3_current is valid */
    if (g_mem_manager.pool3_current < (uint8_t*)0x400000 || g_mem_manager.pool3_current > (uint8_t*)0x500000) {
        termM0PrintStringXY_scroll("HEAP CORRUPT: pool3_current!", 0, debug_line++);
        return NULL;
    }
    
    /* Check for heap corruption - verify pool3_end is valid */
    if (g_mem_manager.pool3_end < (uint8_t*)0x400000 || g_mem_manager.pool3_end > (uint8_t*)0x500000) {
        termM0PrintStringXY_scroll("HEAP CORRUPT: pool3_end!", 0, debug_line++);
        return NULL;
    }
    
    size_t total_size = size + HEADER_SIZE;
    
    unsigned long pool3_current_addr = (unsigned long)(uintptr_t)g_mem_manager.pool3_current;
    unsigned long pool3_end_addr = (unsigned long)(uintptr_t)g_mem_manager.pool3_end;
    
    /* Check if we have enough space in pool3 */
    if (pool3_current_addr + total_size > pool3_end_addr) {
        termM0PrintStringXY_scroll("MALLOC: no space", 0, debug_line++);
        return NULL;  /* Not enough space in pool3 */
    }
    
    /* Allocate from pool3 */
    /* Print pool3_current before allocation */
    unsigned long pool3_val = (unsigned long)(uintptr_t)g_mem_manager.pool3_current;
    uint32_t pool3_high = (pool3_val >> 16) & 0xFFFF;
    uint32_t pool3_low = pool3_val & 0xFFFF;
    char ptr_str[32];
    sprintf(ptr_str, "POOL3_CURR: 0x%04X%04X", (unsigned int)pool3_high, (unsigned int)pool3_low);
    termM0PrintStringXY_scroll(ptr_str, 0, debug_line++);
    
    /* Also print pool3_start to see if it changed */
    unsigned long start_val = (unsigned long)(uintptr_t)g_mem_manager.pool3_start;
    uint32_t start_high = (start_val >> 16) & 0xFFFF;
    uint32_t start_low = start_val & 0xFFFF;
    sprintf(ptr_str, "POOL3_START: 0x%04X%04X", (unsigned int)start_high, (unsigned int)start_low);
    termM0PrintStringXY_scroll(ptr_str, 0, debug_line++);
    
    mem_block_header_t *header = (mem_block_header_t*)g_mem_manager.pool3_current;
    header->size = size;
    header->pool_id = 3;  /* Mark as pool3 allocation */
    header->is_free = 0;
    
    void *ptr = (void*)(g_mem_manager.pool3_current + HEADER_SIZE);
    g_mem_manager.pool3_current += total_size;
    g_mem_manager.total_allocated += size;
    
    /* Print the actual pointer value */
    unsigned long ptr_val = (unsigned long)(uintptr_t)ptr;
    uint32_t ptr_high = (ptr_val >> 16) & 0xFFFF;
    uint32_t ptr_low = ptr_val & 0xFFFF;
    sprintf(ptr_str, "MALLOC PTR: 0x%04X%04X", (unsigned int)ptr_high, (unsigned int)ptr_low);
    termM0PrintStringXY_scroll(ptr_str, 0, debug_line++);
    
    /* Check for heap corruption - verify allocated pointer is valid */
    if (ptr < (void*)0x400000 || ptr > (void*)0x500000) {
        termM0PrintStringXY_scroll("HEAP CORRUPT: bad ptr!", 0, debug_line++);
        /* Show where the pointer actually is */
        unsigned long ptr_val = (unsigned long)(uintptr_t)ptr;
        if (ptr_val < 0x400000) {
            termM0PrintStringXY_scroll("MALLOC: LOW MEM", 0, debug_line++);
        } else if (ptr_val < 0x7E0000) {
            termM0PrintStringXY_scroll("MALLOC: MID MEM", 0, debug_line++);
        } else if (ptr_val < 0x800000) {
            termM0PrintStringXY_scroll("MALLOC: WRAM", 0, debug_line++);
        } else {
            termM0PrintStringXY_scroll("MALLOC: HIGH MEM", 0, debug_line++);
        }
        return NULL;
    }
    
    return ptr;
}

/* Free memory (mark as free for future reuse) */
void snes_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    
    mem_block_header_t *header = (mem_block_header_t*)((uint8_t*)ptr - HEADER_SIZE);
    
    if (header->is_free) {
        /* Already freed - double free */
        return;
    }
    
    g_mem_manager.total_freed += header->size;
    header->is_free = 1;
    
    /* For a simple bump allocator, reset current pointer if this was the last allocation */
    if (header->pool_id == 3) {
        /* Check if this was the last allocation in pool3 (SA-1 SRAM) */
        if ((uint8_t*)ptr + header->size == g_mem_manager.pool3_current) {
            g_mem_manager.pool3_current = g_mem_manager.pool3_start;
        }
    }
}

/* Reallocate memory */
void* snes_realloc(void* ptr, size_t old_size, size_t new_size) {
    /* Debug: Add external debug function declaration */
    extern void termM0PrintStringXY_scroll(const char* str, int x, int y);
    extern int current_line;
    static int debug_line = 80;
    
    if (new_size == 0) {
        snes_free(ptr);
        return NULL;
    }
    
    if (ptr == NULL) {
        termM0PrintStringXY_scroll("REALLOC: ptr=NULL", 0, debug_line++);
        return snes_malloc(new_size);
    }
    
    /* Check if original pointer is in SA-1 SRAM range */
    if (ptr < (void*)0x400000 || ptr > (void*)0x500000) {
        termM0PrintStringXY_scroll("REALLOC: old ptr bad!", 0, debug_line++);
        /* Show where the old pointer actually is */
        unsigned long ptr_val = (unsigned long)(uintptr_t)ptr;
        if (ptr_val < 0x400000) {
            termM0PrintStringXY_scroll("REALLOC: LOW MEM", 0, debug_line++);
        } else if (ptr_val < 0x7E0000) {
            termM0PrintStringXY_scroll("REALLOC: MID MEM", 0, debug_line++);
        } else if (ptr_val < 0x800000) {
            termM0PrintStringXY_scroll("REALLOC: WRAM", 0, debug_line++);
        } else {
            termM0PrintStringXY_scroll("REALLOC: HIGH MEM", 0, debug_line++);
        }
        return NULL;
    }
    
    mem_block_header_t *header = (mem_block_header_t*)((uint8_t*)ptr - HEADER_SIZE);
    
    /* If new size is smaller or equal, just update the header */
    if (new_size <= header->size) {
        header->size = new_size;
        return ptr;
    }
    
    /* Need to allocate new block and copy data */
    termM0PrintStringXY_scroll("REALLOC: calling malloc", 0, debug_line++);
    void *new_ptr = snes_malloc(new_size);
    if (new_ptr == NULL) {
        termM0PrintStringXY_scroll("REALLOC: malloc failed", 0, debug_line++);
        return NULL;
    }
    
    /* Check if new pointer is in SA-1 SRAM range */
    if (new_ptr < (void*)0x400000 || new_ptr > (void*)0x500000) {
        termM0PrintStringXY_scroll("REALLOC: new ptr bad!", 0, debug_line++);
        return NULL;
    }
    
    /* Copy old data to new location */
    size_t copy_size = (old_size < new_size) ? old_size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    
    /* Free old block */
    snes_free(ptr);
    
    termM0PrintStringXY_scroll("REALLOC: success", 0, debug_line++);
    return new_ptr;
}

/* Print memory statistics */
void snes_memory_stats(void) {
    /* This would be called from the main program to display stats */
    /* For now, just a placeholder - actual implementation would depend on display system */
}

/* Pool3-specific allocation functions for Lua */
void* snes_malloc_pool3(size_t size) {
    if (!g_mem_manager.pool3_available) {
        return NULL;
    }
    
    size_t total_size = HEADER_SIZE + size;
    
    /* Check if we have enough space in pool3 */
    if ((uint8_t*)g_mem_manager.pool3_current + total_size > g_mem_manager.pool3_end) {
        return NULL;  /* Not enough space in pool3 */
    }
    
    /* Allocate from pool3 */
    mem_block_header_t *header = (mem_block_header_t*)g_mem_manager.pool3_current;
    header->size = size;
    header->pool_id = 3;  /* Mark as pool3 allocation */
    header->is_free = 0;
    
    void *ptr = (void*)(g_mem_manager.pool3_current + HEADER_SIZE);
    g_mem_manager.pool3_current += total_size;
    g_mem_manager.total_allocated += size;
    
    return ptr;
}

void* snes_realloc_pool3(void* ptr, size_t old_size, size_t new_size) {
    if (new_size == 0) {
        snes_free(ptr);
        return NULL;
    }
    
    if (ptr == NULL) {
        return snes_malloc_pool3(new_size);
    }
    
    mem_block_header_t *header = (mem_block_header_t*)((uint8_t*)ptr - HEADER_SIZE);
    
    /* Verify this is a pool3 allocation */
    if (header->pool_id != 3) {
        return NULL;  /* Not a pool3 allocation */
    }
    
    /* If new size is smaller or equal, just update the header */
    if (new_size <= header->size) {
        header->size = new_size;
        return ptr;
    }
    
    /* Need to allocate new block and copy data */
    void *new_ptr = snes_malloc_pool3(new_size);
    if (new_ptr == NULL) {
        return NULL;
    }
    
    /* Copy old data to new location */
    size_t copy_size = (old_size < new_size) ? old_size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    
    /* Free old block */
    snes_free(ptr);
    
    return new_ptr;
}
