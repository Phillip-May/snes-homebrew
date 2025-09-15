#include "snes_memory_manager.h"
#include <string.h>
#include <stdio.h>

/* SA-1 SRAM address range macros - internal to this file only */
#define SA1_SRAM_START ((uint8_t*)0x400000)
#define SA1_SRAM_END   ((uint8_t*)0x420000)  /* 128KB SA-1 SRAM: 0x400000-0x41FFFF */
#define SA1_SRAM_MAX_TEST ((uint8_t*)0x420000)  /* Maximum address to test for SRAM detection */
#define SA1_SRAM_FALLBACK_END ((uint8_t*)0xABCDEF00)  /* Fallback end if all tests pass */

/* WRAM range to check as fallback */
#define WRAM_START ((uint8_t*)0x7f0000)
#define WRAM_END   ((uint8_t*)0x800000)
#define WRAM_MAX_TEST ((uint8_t*)0x800000)  /* Maximum address to test for WRAM */

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

/* Test if a memory range is writable */
int snes_test_memory_range(volatile uint8_t *start, volatile uint8_t *end) {
    /* Test pattern for write/read verification */
    uint8_t test_pattern = 0xAA;
    uint8_t test_pattern2 = 0x55;
    
    /* Test a few locations in the memory range */
    for (volatile uint8_t *ptr = start; ptr < start + SA1_SRAM_INITIAL_TEST_SIZE && ptr < end; ptr += SA1_SRAM_INITIAL_TEST_STEP) {
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
    
    return 1;  /* Memory range is writable */
}

/* Test if SA-1 SRAM range is writable (legacy function for compatibility) */
int snes_test_sa1_sram(void) {
    return snes_test_memory_range((volatile uint8_t*)SA1_SRAM_START, (volatile uint8_t*)SA1_SRAM_END);
}

/* Find the actual end of a memory range by testing writability and detecting mirrors */
uint8_t* snes_find_memory_end(volatile uint8_t *start, volatile uint8_t *max_test) {
    volatile uint8_t *test_ptr = start;
    uint8_t test_pattern = 0xAA;
    uint8_t test_pattern2 = 0x55;
    uint8_t mirror_test_pattern = 0x33;
    
    /* Test every 4KB boundary to find where memory ends */
    while (test_ptr < max_test) {  /* Don't go beyond reasonable range */
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
        if (((test_ptr - start) % SA1_SRAM_MIRROR_TEST_SIZE == 0) && (test_ptr != start)) {
            *test_ptr = mirror_test_pattern;
            
            /* Check if this write affected the start of memory (indicating mirroring) */
            if (*start == mirror_test_pattern) {
                /* This is a mirror of the start - we found the actual end */
                *test_ptr = original;  /* Restore original value */
                *start = test_pattern;  /* Restore start to original pattern */
                return (uint8_t*)test_ptr;
            }
        }
        
        /* Restore original value */
        *test_ptr = original;
        
        /* Move to next 4KB boundary */
        test_ptr += SA1_SRAM_TEST_STEP_SIZE;
    }
    
    /* If we get here, we tested up to the maximum test range and it was all writable */
    /* For SA-1 SRAM, if we tested the full range and it was all writable, return the expected end */
    if (start == (volatile uint8_t*)SA1_SRAM_START) {
        return (uint8_t*)0x420000;  /* Return expected end of SA-1 SRAM */
    }
    
    /* For WRAM, if we tested the full range and it was all writable, return the expected end */
    if (start == (volatile uint8_t*)WRAM_START) {
        return (uint8_t*)0x800000;  /* Return expected end of WRAM (0x7FFFFF + 1) */
    }
    
    /* For other ranges, return the fallback end address */
    return SA1_SRAM_FALLBACK_END;
}

/* Find the actual end of SA-1 SRAM by testing writability and detecting mirrors (legacy function) */
uint8_t* snes_find_sa1_sram_end(void) {
    return snes_find_memory_end((volatile uint8_t*)SA1_SRAM_START, (volatile uint8_t*)SA1_SRAM_MAX_TEST);
}

/* Initialize memory manager */
void snes_memory_init(void) {
    /* Test and initialize memory pool - try multiple ranges */
    g_mem_manager.pool3_available = 0;
    
    /* First try: SA-1 SRAM 40:0000-41:FFFF range */
    int sa1_test_result = snes_test_memory_range((volatile uint8_t*)SA1_SRAM_START, (volatile uint8_t*)SA1_SRAM_END);
    if (sa1_test_result) {
        g_mem_manager.pool3_start = (uint8_t*)SA1_SRAM_START;  /* Use 0x400000 as start */
        
        /* Automatically detect the actual end of SA-1 SRAM */
        g_mem_manager.pool3_end = snes_find_memory_end((volatile uint8_t*)SA1_SRAM_START, (volatile uint8_t*)SA1_SRAM_MAX_TEST);
        g_mem_manager.pool3_current = g_mem_manager.pool3_start;
        g_mem_manager.pool3_available = 1;
    } else {
        /* Second try: WRAM 7f:0000-7f:ffff range */
        int wram_test_result = snes_test_memory_range((volatile uint8_t*)WRAM_START, (volatile uint8_t*)WRAM_END);
        if (wram_test_result) {
            g_mem_manager.pool3_start = (uint8_t*)WRAM_START;  /* Use 0x7f0000 as start */
            
            /* Automatically detect the actual end of WRAM */
            g_mem_manager.pool3_end = snes_find_memory_end((volatile uint8_t*)WRAM_START, (volatile uint8_t*)WRAM_MAX_TEST);
            g_mem_manager.pool3_current = g_mem_manager.pool3_start;
            g_mem_manager.pool3_available = 1;
        } else {
            /* Neither range is writable - use fallback */
            g_mem_manager.pool3_start = (uint8_t*)SA1_SRAM_START;  /* Use 0x400000 as start */
            g_mem_manager.pool3_end = (uint8_t*)SA1_SRAM_FALLBACK_END;  /* Use fallback end */
            g_mem_manager.pool3_current = g_mem_manager.pool3_start;
            g_mem_manager.pool3_available = 1;  /* Still mark as available for debugging */
        }
    }
    
    g_mem_manager.total_allocated = 0;
    g_mem_manager.total_freed = 0;
}

/* Allocate memory from pool3 (SA-1 SRAM) only */
void* snes_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    /* Only use pool3 (SA-1 SRAM) */
    if (!g_mem_manager.pool3_available) {
        return NULL;
    }
    
    size_t total_size = size + HEADER_SIZE;
    
    unsigned long pool3_current_addr = (unsigned long)(uintptr_t)g_mem_manager.pool3_current;
    unsigned long pool3_end_addr = (unsigned long)(uintptr_t)g_mem_manager.pool3_end;
    
    /* Check if we have enough space in pool3 */
    if (pool3_current_addr + total_size > pool3_end_addr) {
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
    if (new_size == 0) {
        snes_free(ptr);
        return NULL;
    }
    
    if (ptr == NULL) {
        return snes_malloc(new_size);
    }
    
    mem_block_header_t *header = (mem_block_header_t*)((uint8_t*)ptr - HEADER_SIZE);
    
    /* If new size is smaller or equal, just update the header */
    if (new_size <= header->size) {
        header->size = new_size;
        return ptr;
    }
    
    /* Need to allocate new block and copy data */
    void *new_ptr = snes_malloc(new_size);
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
