#include "snes_memory_manager.h"
#include <string.h>
#include <stdio.h>

/* SA-1 SRAM address range macros - internal to this file only */
#define SA1_SRAM_START ((uint8_t*)0x400000)
#define SA1_SRAM_END   ((uint8_t*)0x500000)  /* 128KB SA-1 SRAM: 0x400000-0x41FFFF */
#define SA1_SRAM_MAX_TEST ((uint8_t*)0x500000)  /* Maximum address to test for SRAM detection */
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

/* Enhanced memory block header for heap allocator */
typedef struct mem_block {
    uint32_t size;           /* Size of the data area (not including header) */
    uint8_t pool_id;         /* 3 for pool3 (SA-1 SRAM) */
    uint8_t is_free;         /* 1 if free, 0 if allocated */
    struct mem_block *next;  /* Next block in free list (only used when free) */
    struct mem_block *prev;  /* Previous block in free list (only used when free) */
} mem_block_t;

#define HEADER_SIZE sizeof(mem_block_t)
#define MIN_BLOCK_SIZE (HEADER_SIZE + 16)  /* Minimum useful block size */

/* Free list management */
mem_block_t *free_list_head = NULL;

/* Add block to free list */
static void add_to_free_list(mem_block_t *block) {
    if (free_list_head == NULL) {
        free_list_head = block;
        block->next = block;
        block->prev = block;
    } else {
        block->next = free_list_head;
        block->prev = free_list_head->prev;
        free_list_head->prev->next = block;
        free_list_head->prev = block;
    }
}

/* Remove block from free list */
static void remove_from_free_list(mem_block_t *block) {
    if (block->next == block) {
        /* Only block in list */
        free_list_head = NULL;
    } else {
        block->prev->next = block->next;
        block->next->prev = block->prev;
        if (free_list_head == block) {
            free_list_head = block->next;
        }
    }
}

/* Find best fit block in free list with debugging */
static mem_block_t *find_best_fit(uint32_t size) {
    if (free_list_head == NULL) {
        return NULL;
    }
    
    mem_block_t *best = NULL;
    mem_block_t *current = free_list_head;
    
    /* Debug: Log search for large allocations */
    if (size > 32768) {
        char search_debug[128];
        snprintf(search_debug, sizeof(search_debug), "SEARCH: Looking for %lu bytes", (unsigned long)size);
        auto_print_string(search_debug);
        
        /* CRITICAL: Show total free memory vs largest blocks to diagnose fragmentation */
        uint32_t total_free = snes_get_free_memory();
        char fragmentation_debug[128];
        snprintf(fragmentation_debug, sizeof(fragmentation_debug), "FRAGMENTATION: %lu total free, need %lu contiguous", 
                 (unsigned long)total_free, (unsigned long)size);
        auto_print_string(fragmentation_debug);
    }
    
    int block_count = 0;
    do {
        block_count++;
        
        /* Debug: Log ALL blocks for large allocation requests to see fragmentation */
        if (size > 32768) {
            char block_debug[128];
            if (current->size > 1000) {  /* Show blocks > 1KB */
                snprintf(block_debug, sizeof(block_debug), "BLOCK %d: %lu bytes at %p", 
                         block_count, (unsigned long)current->size, (void*)current);
                auto_print_string(block_debug);
            } else if (block_count <= 10) {  /* Show first 10 small blocks */
                snprintf(block_debug, sizeof(block_debug), "SMALL %d: %lu bytes at %p", 
                         block_count, (unsigned long)current->size, (void*)current);
                auto_print_string(block_debug);
            }
        }
        
        if (current->size >= size) {
            if (best == NULL || current->size < best->size) {
                best = current;
            }
        }
        current = current->next;
    } while (current != free_list_head);
    
    /* Debug: Log result for large allocations */
    if (size > 32768) {
        if (best) {
            char result_debug[128];
            snprintf(result_debug, sizeof(result_debug), "FOUND: Block with %lu bytes for %lu byte request", 
                     (unsigned long)best->size, (unsigned long)size);
            auto_print_string(result_debug);
        } else {
            char fail_debug[128];
            snprintf(fail_debug, sizeof(fail_debug), "NOT FOUND: No block large enough for %lu bytes (checked %d blocks)", 
                     (unsigned long)size, block_count);
            auto_print_string(fail_debug);
        }
    }
    
    return best;
}

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
    int test_count = 0;
    
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
        test_count++;
    }
    
    /* If we get here, we tested up to the maximum test range and it was all writable */
    /* For SA-1 SRAM, if we tested the full range and it was all writable, return the expected end */
    if (start == (volatile uint8_t*)SA1_SRAM_START) {
        /* Debug: Check if we actually tested the full range */
        if (test_count < 32) {  /* Should be 32 steps for 128KB (32 * 4KB = 128KB) */
            /* We didn't test the full range - something went wrong */
            return (uint8_t*)(start + (test_count * SA1_SRAM_TEST_STEP_SIZE));
        }
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
    
    /* Initialize free list with one large block covering the entire pool */
    if (g_mem_manager.pool3_available) {
        mem_block_t *initial_block = (mem_block_t*)g_mem_manager.pool3_start;
        uint32_t pool_size = (uint32_t)((uintptr_t)g_mem_manager.pool3_end - (uintptr_t)g_mem_manager.pool3_start);
        
        /* Debug: Print pool information */
        /* Note: This will be called before display system is ready, so we'll add it to main */
        
        initial_block->size = pool_size - HEADER_SIZE;
        initial_block->pool_id = 3;
        initial_block->is_free = 1;
        initial_block->next = NULL;
        initial_block->prev = NULL;
        
        add_to_free_list(initial_block);
        
        /* Update current pointer to end of pool */
        g_mem_manager.pool3_current = g_mem_manager.pool3_end;
        
        /* Debug: Verify free list was created correctly */
        /* This will be checked in main program */
    }
}

/* Allocate memory from pool3 (SA-1 SRAM) using heap allocator */
void* snes_malloc(uint32_t size) {
    if (size == 0) {
        return NULL;
    }
    
    /* Only use pool3 (SA-1 SRAM) */
    if (!g_mem_manager.pool3_available) {
        return NULL;
    }
    
    /* Align size to 4-byte boundary for better performance */
    size = (size + 3) & ~3;
    
    /* Find best fit block */
    mem_block_t *block = find_best_fit(size);
    if (block == NULL) {
        return NULL;  /* No suitable block found */
    }
    
    /* Remove from free list */
    remove_from_free_list(block);
    
    /* Check if we can split the block */
    if (block->size >= size + MIN_BLOCK_SIZE) {
        /* Split the block */
        mem_block_t *new_block = (mem_block_t*)((uint8_t*)block + HEADER_SIZE + size);
        new_block->size = block->size - size - HEADER_SIZE;
        new_block->pool_id = 3;
        new_block->is_free = 1;
        
        /* Add the remainder to free list */
        add_to_free_list(new_block);
        
        /* Update original block size */
        block->size = size;
    }
    
    /* Mark as allocated */
    block->is_free = 0;
    block->pool_id = 3;
    block->next = NULL;
    block->prev = NULL;
    
    g_mem_manager.total_allocated += block->size;
    
    return (void*)((uint8_t*)block + HEADER_SIZE);
}

/* Free memory using heap allocator with coalescing */
void snes_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    
    mem_block_t *block = (mem_block_t*)((uint8_t*)ptr - HEADER_SIZE);
    
    if (block->is_free) {
        /* Already freed - double free */
        return;
    }
    
    g_mem_manager.total_freed += block->size;
    block->is_free = 1;
    
    /* Coalesce with adjacent free blocks */
    mem_block_t *current = free_list_head;
    if (current != NULL) {
        do {
            /* Check if this block is adjacent to the one we're freeing */
            if ((uint8_t*)current + HEADER_SIZE + current->size == (uint8_t*)block) {
                /* Current block is immediately before our block - merge */
                current->size += HEADER_SIZE + block->size;
                return;  /* Block is now part of the existing free block */
            } else if ((uint8_t*)block + HEADER_SIZE + block->size == (uint8_t*)current) {
                /* Our block is immediately before current block - merge */
                block->size += HEADER_SIZE + current->size;
                remove_from_free_list(current);
                add_to_free_list(block);
                return;
            }
            current = current->next;
        } while (current != free_list_head);
    }
    
    /* No adjacent blocks found - just add to free list */
    add_to_free_list(block);
}

/* Reallocate memory using heap allocator */
void* snes_realloc(void* ptr, uint32_t old_size, uint32_t new_size) {
    if (new_size == 0) {
        snes_free(ptr);
        return NULL;
    }
    
    if (ptr == NULL) {
        return snes_malloc(new_size);
    }
    
    /* Align new size to 4-byte boundary */
    new_size = (new_size + 3) & ~3;
    
    mem_block_t *block = (mem_block_t*)((uint8_t*)ptr - HEADER_SIZE);
    
    /* If new size is smaller or equal, just update the header */
    if (new_size <= block->size) {
        /* Check if we can split the block */
        if (block->size >= new_size + MIN_BLOCK_SIZE) {
            /* Split the block */
            mem_block_t *new_block = (mem_block_t*)((uint8_t*)block + HEADER_SIZE + new_size);
            new_block->size = block->size - new_size - HEADER_SIZE;
            new_block->pool_id = 3;
            new_block->is_free = 1;
            
            /* Add the remainder to free list */
            add_to_free_list(new_block);
            
            /* Update original block size */
            block->size = new_size;
            g_mem_manager.total_freed += (block->size - new_size);
        }
        return ptr;
    }
    
    /* Need to allocate new block and copy data */
    void *new_ptr = snes_malloc(new_size);
    if (new_ptr == NULL) {
        return NULL;
    }
    
    /* Copy old data to new location */
    uint32_t copy_size = (old_size < new_size) ? old_size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    
    /* Free old block */
    snes_free(ptr);
    
    return new_ptr;
}

/* Calculate total free memory from free list */
uint32_t snes_get_free_memory(void) {
    uint32_t total_free = 0;
    mem_block_t *current = free_list_head;
    
    if (current != NULL) {
        do {
            total_free += current->size;
            current = current->next;
        } while (current != free_list_head);
    }
    
    return total_free;
}

/* Force coalescing of all adjacent free blocks with detailed debugging */
void snes_force_coalesce(void) {
    if (free_list_head == NULL) return;
    
    int coalesced_count = 0;
    int max_iterations = 10;  /* Multiple passes to catch all adjacencies */
    
    for (int pass = 0; pass < max_iterations; pass++) {
        mem_block_t *current = free_list_head;
        int found_merge = 0;
        
        do {
            mem_block_t *next_in_list = current->next;
            mem_block_t *check = free_list_head;
            
            /* Check all other blocks to see if any are adjacent */
            do {
                if (check != current) {
                    /* Check if blocks are adjacent */
                    uint8_t *current_end = (uint8_t*)current + HEADER_SIZE + current->size;
                    uint8_t *check_start = (uint8_t*)check;
                    
                    if (current_end == check_start) {
                        /* Current block is immediately before check block - merge */
                        char merge_debug[128];
                        snprintf(merge_debug, sizeof(merge_debug), "MERGE: %lu + %lu = %lu bytes", 
                                 (unsigned long)current->size, (unsigned long)check->size, 
                                 (unsigned long)(current->size + HEADER_SIZE + check->size));
                        auto_print_string(merge_debug);
                        
                        current->size += HEADER_SIZE + check->size;
                        remove_from_free_list(check);
                        coalesced_count++;
                        found_merge = 1;
                        break;  /* Restart the search */
                    }
                    
                    uint8_t *check_end = (uint8_t*)check + HEADER_SIZE + check->size;
                    uint8_t *current_start = (uint8_t*)current;
                    
                    if (check_end == current_start) {
                        /* Check block is immediately before current block - merge */
                        char merge_debug[128];
                        snprintf(merge_debug, sizeof(merge_debug), "MERGE: %lu + %lu = %lu bytes", 
                                 (unsigned long)check->size, (unsigned long)current->size, 
                                 (unsigned long)(check->size + HEADER_SIZE + current->size));
                        auto_print_string(merge_debug);
                        
                        check->size += HEADER_SIZE + current->size;
                        remove_from_free_list(current);
                        coalesced_count++;
                        found_merge = 1;
                        break;  /* Restart the search */
                    }
                }
                check = check->next;
            } while (check != free_list_head);
            
            if (found_merge) break;  /* Restart from beginning after merge */
            current = next_in_list;
        } while (current != free_list_head);
        
        if (!found_merge) break;  /* No more merges possible */
    }
    
    if (coalesced_count > 0) {
        char coalesce_summary[64];
        snprintf(coalesce_summary, sizeof(coalesce_summary), "COALESCED: %d blocks merged", coalesced_count);
        auto_print_string(coalesce_summary);
    } else {
        auto_print_string("COALESCE: No adjacent blocks found to merge");
    }
}

/* Get the size of the largest free block */
uint32_t snes_get_largest_free_block(void) {
    uint32_t largest = 0;
    mem_block_t *current = free_list_head;
    
    if (current != NULL) {
        do {
            if (current->size > largest) {
                largest = current->size;
            }
            current = current->next;
        } while (current != free_list_head);
    }
    
    return largest;
}

/* Debug function to show free list contents */
void snes_debug_free_list(void) {
    mem_block_t *current = free_list_head;
    int block_count = 0;
    uint32_t total_size = 0;
    uint32_t largest_block = 0;
    
    if (current == NULL) {
        return; /* No free blocks */
    }
    
    do {
        block_count++;
        total_size += current->size;
        if (current->size > largest_block) {
            largest_block = current->size;
        }
        current = current->next;
    } while (current != free_list_head);
    
    /* This would need to be called from main program to display */
    /* For now, just a placeholder - actual implementation would depend on display system */
}

/* Print memory statistics */
void snes_memory_stats(void) {
    /* This would be called from the main program to display stats */
    /* For now, just a placeholder - actual implementation would depend on display system */
}

/* Note: Pool3-specific functions removed - now using unified heap allocator */
