#ifndef SNES_MEMORY_MANAGER_H
#define SNES_MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>

/* Memory pool sizes */
#define POOL3_SIZE (131072ul)  /* 128KB SA-1 SRAM */

/* Memory manager state */
typedef struct {
    uint8_t *pool3_start;  /* SA-1 SRAM pool */
    uint8_t *pool3_end;
    uint8_t *pool3_current;
    uint8_t pool3_available;  /* 1 if SA-1 SRAM is available, 0 if not */
    
    uint32_t total_allocated;
    uint32_t total_freed;
} snes_memory_manager_t;

/* Function declarations */
void snes_memory_init(void);
int snes_test_sa1_sram(void);
uint8_t* snes_find_sa1_sram_end(void);
void* snes_malloc(size_t size);
void snes_free(void* ptr);
void* snes_realloc(void* ptr, size_t old_size, size_t new_size);
void snes_memory_stats(void);

/* Pool3-specific allocation functions for Lua */
void* snes_malloc_pool3(size_t size);
void* snes_realloc_pool3(void* ptr, size_t old_size, size_t new_size);

/* Global memory manager instance */
extern snes_memory_manager_t g_mem_manager;

#endif /* SNES_MEMORY_MANAGER_H */
