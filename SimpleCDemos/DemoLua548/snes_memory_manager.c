#include "snes_memory_manager.h"
#include <string.h>

/* SA-1 SRAM address range macros - internal to this file only */
#define SA1_SRAM_START ((uint8_t*)0xD00000)
#define SA1_SRAM_END   ((uint8_t*)0xFFFFFF)
#define SA1_SRAM_MAX_TEST ((uint8_t*)0xFFFFFF)  /* Maximum address to test for SRAM detection */
#define SA1_SRAM_FALLBACK_END ((uint8_t*)0xABCDEF00)  /* Fallback end if all tests pass */

/* SA-1 SRAM test configuration macros - internal to this file only */
#define SA1_SRAM_TEST_STEP_SIZE 0x1000      /* 4KB step size for boundary testing */
#define SA1_SRAM_MIRROR_TEST_SIZE 0x8000    /* 32KB step size for mirror detection */
#define SA1_SRAM_INITIAL_TEST_SIZE 0x1000   /* Initial test range size */
#define SA1_SRAM_INITIAL_TEST_STEP 0x100    /* Step size for initial test */

/* Memory pools - these will be placed in specific memory sections */
uint8_t banked_heap1[POOL1_SIZE] __attribute__((section("BANKED_HEAP1")));
uint8_t banked_heap2[POOL2_SIZE];

/* Global memory manager instance */
snes_memory_manager_t g_mem_manager;

/* Simple memory block header for tracking allocations */
typedef struct {
    uint32_t size;    /* Use uint32_t instead of size_t for consistency */
    uint8_t pool_id;  /* 1 for pool1, 2 for pool2, 3 for pool3 (SA-1 SRAM) */
    uint8_t is_free;
    uint8_t padding[2]; /* Ensure 8-byte alignment */
} mem_block_header_t;

#define HEADER_SIZE sizeof(mem_block_header_t)
#define ALIGN_SIZE 8  /* Align to 8-byte boundaries for better performance */

/* Align size to 4-byte boundary */
static size_t align_size(size_t size) {
    return (size + ALIGN_SIZE - 1) & ~(ALIGN_SIZE - 1);
}

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
    g_mem_manager.pool1_start = banked_heap1;
    g_mem_manager.pool1_current = banked_heap1;
    
    g_mem_manager.pool2_start = banked_heap2;
    g_mem_manager.pool2_current = banked_heap2;
    
    /* Calculate end addresses using explicit 24-bit arithmetic */
    uint32_t pool1_start_addr = (uint32_t)(uintptr_t)banked_heap1;
    uint32_t pool2_start_addr = (uint32_t)(uintptr_t)banked_heap2;
    g_mem_manager.pool1_end = (uint8_t*)(uintptr_t)(pool1_start_addr + POOL1_SIZE);
    g_mem_manager.pool2_end = (uint8_t*)(uintptr_t)(pool2_start_addr + POOL2_SIZE);
    
    /* Test and initialize SA-1 SRAM pool */
    g_mem_manager.pool3_available = 0;
    int sa1_test_result = snes_test_sa1_sram();
    if (sa1_test_result) {
        g_mem_manager.pool3_start = SA1_SRAM_START;
        /* Automatically detect the actual end of SA-1 SRAM */
        g_mem_manager.pool3_end = snes_find_sa1_sram_end();
        g_mem_manager.pool3_current = g_mem_manager.pool3_start;
        g_mem_manager.pool3_available = 1;
    } else {
        g_mem_manager.pool3_start = NULL;
        g_mem_manager.pool3_end = NULL;
        g_mem_manager.pool3_current = NULL;
    }
    
    g_mem_manager.total_allocated = 0;
    g_mem_manager.total_freed = 0;
}

/* Allocate memory from the appropriate pool */
void* snes_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    size_t aligned_size = align_size(size);
    size_t total_size = aligned_size + HEADER_SIZE;
    
    /* Debug: Check if manager is initialized */
    if (g_mem_manager.pool1_start == NULL || g_mem_manager.pool2_start == NULL) {
        return NULL;
    }
    
    /* Route allocations based on size:
     * - Small allocations (< 50K): try pool2 first, then pool1, then pool3
     * - Large allocations (>= 50K): try pool3 first, then pool1, then pool2
     */
    uint32_t pool1_current_addr = (uint32_t)(uintptr_t)g_mem_manager.pool1_current;
    uint32_t pool1_end_addr = (uint32_t)(uintptr_t)g_mem_manager.pool1_end;
    uint32_t pool2_current_addr = (uint32_t)(uintptr_t)g_mem_manager.pool2_current;
    uint32_t pool2_end_addr = (uint32_t)(uintptr_t)g_mem_manager.pool2_end;
    uint32_t pool3_current_addr = 0;
    uint32_t pool3_end_addr = 0;
    
    if (g_mem_manager.pool3_available) {
        pool3_current_addr = (uint32_t)(uintptr_t)g_mem_manager.pool3_current;
        pool3_end_addr = (uint32_t)(uintptr_t)g_mem_manager.pool3_end;
    }
    
    if (aligned_size < 50000) {
        /* Small allocation: try pool2 first */
        if (pool2_current_addr + total_size <= pool2_end_addr) {
            mem_block_header_t *header = (mem_block_header_t*)g_mem_manager.pool2_current;
            header->size = aligned_size;
            header->pool_id = 2;
            header->is_free = 0;
            
            void *ptr = (void*)(g_mem_manager.pool2_current + HEADER_SIZE);
            g_mem_manager.pool2_current += total_size;
            g_mem_manager.total_allocated += aligned_size;
            
            return ptr;
        }
        
        /* Try pool1 if pool2 is full */
        if (pool1_current_addr + total_size <= pool1_end_addr) {
            mem_block_header_t *header = (mem_block_header_t*)g_mem_manager.pool1_current;
            header->size = aligned_size;
            header->pool_id = 1;
            header->is_free = 0;
            
            void *ptr = (void*)(g_mem_manager.pool1_current + HEADER_SIZE);
            g_mem_manager.pool1_current += total_size;
            g_mem_manager.total_allocated += aligned_size;
            
            return ptr;
        }
        
        /* Try pool3 (SA-1 SRAM) if available and pools 1&2 are full */
        if (g_mem_manager.pool3_available && pool3_current_addr + total_size <= pool3_end_addr) {
            mem_block_header_t *header = (mem_block_header_t*)g_mem_manager.pool3_current;
            header->size = aligned_size;
            header->pool_id = 3;
            header->is_free = 0;
            
            void *ptr = (void*)(g_mem_manager.pool3_current + HEADER_SIZE);
            g_mem_manager.pool3_current += total_size;
            g_mem_manager.total_allocated += aligned_size;
            
            return ptr;
        }
    } else {
        /* Large allocation: try pool3 (SA-1 SRAM) first, then pool1, then pool2 */
        if (g_mem_manager.pool3_available && pool3_current_addr + total_size <= pool3_end_addr) {
            mem_block_header_t *header = (mem_block_header_t*)g_mem_manager.pool3_current;
            header->size = aligned_size;
            header->pool_id = 3;
            header->is_free = 0;
            
            void *ptr = (void*)(g_mem_manager.pool3_current + HEADER_SIZE);
            g_mem_manager.pool3_current += total_size;
            g_mem_manager.total_allocated += aligned_size;
            
            return ptr;
        }
        
        /* Try pool1 if pool3 is not available or full */
        if (pool1_current_addr + total_size <= pool1_end_addr) {
            mem_block_header_t *header = (mem_block_header_t*)g_mem_manager.pool1_current;
            header->size = aligned_size;
            header->pool_id = 1;
            header->is_free = 0;
            
            void *ptr = (void*)(g_mem_manager.pool1_current + HEADER_SIZE);
            g_mem_manager.pool1_current += total_size;
            g_mem_manager.total_allocated += aligned_size;
            
            return ptr;
        }
        
        /* Try pool2 if pool1 is full */
        if (pool2_current_addr + total_size <= pool2_end_addr) {
            mem_block_header_t *header = (mem_block_header_t*)g_mem_manager.pool2_current;
            header->size = aligned_size;
            header->pool_id = 2;
            header->is_free = 0;
            
            void *ptr = (void*)(g_mem_manager.pool2_current + HEADER_SIZE);
            g_mem_manager.pool2_current += total_size;
            g_mem_manager.total_allocated += aligned_size;
            
            return ptr;
        }
    }
    
    /* No memory available */
    return NULL;
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
    if (header->pool_id == 1) {
        /* Check if this was the last allocation in pool1 */
        if ((uint8_t*)ptr + header->size == g_mem_manager.pool1_current) {
            g_mem_manager.pool1_current = g_mem_manager.pool1_start;
        }
    } else if (header->pool_id == 2) {
        /* Check if this was the last allocation in pool2 */
        if ((uint8_t*)ptr + header->size == g_mem_manager.pool2_current) {
            g_mem_manager.pool2_current = g_mem_manager.pool2_start;
        }
    } else if (header->pool_id == 3) {
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
    
    size_t aligned_new_size = align_size(new_size);
    mem_block_header_t *header = (mem_block_header_t*)((uint8_t*)ptr - HEADER_SIZE);
    
    /* If new size is smaller or equal, just update the header */
    if (aligned_new_size <= header->size) {
        header->size = aligned_new_size;
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
    
    size_t aligned_size = align_size(size);
    size_t total_size = HEADER_SIZE + aligned_size;
    
    /* Check if we have enough space in pool3 */
    if ((uint8_t*)g_mem_manager.pool3_current + total_size > g_mem_manager.pool3_end) {
        return NULL;  /* Not enough space in pool3 */
    }
    
    /* Allocate from pool3 */
    mem_block_header_t *header = (mem_block_header_t*)g_mem_manager.pool3_current;
    header->size = aligned_size;
    header->pool_id = 3;  /* Mark as pool3 allocation */
    header->is_free = 0;
    
    void *ptr = (void*)(g_mem_manager.pool3_current + HEADER_SIZE);
    g_mem_manager.pool3_current += total_size;
    g_mem_manager.total_allocated += total_size;
    
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
    
    size_t aligned_new_size = align_size(new_size);
    mem_block_header_t *header = (mem_block_header_t*)((uint8_t*)ptr - HEADER_SIZE);
    
    /* Verify this is a pool3 allocation */
    if (header->pool_id != 3) {
        return NULL;  /* Not a pool3 allocation */
    }
    
    /* If new size is smaller or equal, just update the header */
    if (aligned_new_size <= header->size) {
        header->size = aligned_new_size;
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
