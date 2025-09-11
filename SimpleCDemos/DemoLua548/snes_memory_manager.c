#include "snes_memory_manager.h"
#include <string.h>

/* Memory pools - these will be placed in specific memory sections */
uint8_t banked_heap1[POOL1_SIZE] __attribute__((section("BANKED_HEAP1")));
uint8_t banked_heap2[POOL2_SIZE];

/* Global memory manager instance */
snes_memory_manager_t g_mem_manager;

/* Simple memory block header for tracking allocations */
typedef struct {
    uint32_t size;    /* Use uint32_t instead of size_t for consistency */
    uint8_t pool_id;  /* 1 for pool1, 2 for pool2 */
    uint8_t is_free;
    uint8_t padding[2]; /* Ensure 8-byte alignment */
} mem_block_header_t;

#define HEADER_SIZE sizeof(mem_block_header_t)
#define ALIGN_SIZE 8  /* Align to 8-byte boundaries for better performance */

/* Align size to 4-byte boundary */
static size_t align_size(size_t size) {
    return (size + ALIGN_SIZE - 1) & ~(ALIGN_SIZE - 1);
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
    
    /* Try pool1 first (larger pool) - use 32-bit arithmetic for boundary check */
    uint32_t pool1_current_addr = (uint32_t)(uintptr_t)g_mem_manager.pool1_current;
    uint32_t pool1_end_addr = (uint32_t)(uintptr_t)g_mem_manager.pool1_end;
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
    
    /* Try pool2 if pool1 is full - use 32-bit arithmetic for boundary check */
    uint32_t pool2_current_addr = (uint32_t)(uintptr_t)g_mem_manager.pool2_current;
    uint32_t pool2_end_addr = (uint32_t)(uintptr_t)g_mem_manager.pool2_end;
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
