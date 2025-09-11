#ifndef SNES_MEMORY_MANAGER_H
#define SNES_MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>

/* Memory pool sizes */
#define POOL1_SIZE (65535ul)
#define POOL2_SIZE (32768ul)

/* Memory pool declarations with section attributes */
extern uint8_t banked_heap1[POOL1_SIZE] __attribute__((section("BANKED_HEAP1")));
extern uint8_t banked_heap2[POOL2_SIZE];

/* Memory manager state */
typedef struct {
    uint8_t *pool1_start;
    uint8_t *pool1_end;
    uint8_t *pool1_current;
    
    uint8_t *pool2_start;
    uint8_t *pool2_end;
    uint8_t *pool2_current;
    
    uint32_t total_allocated;
    uint32_t total_freed;
} snes_memory_manager_t;

/* Function declarations */
void snes_memory_init(void);
void* snes_malloc(size_t size);
void snes_free(void* ptr);
void* snes_realloc(void* ptr, size_t old_size, size_t new_size);
void snes_memory_stats(void);

/* Global memory manager instance */
extern snes_memory_manager_t g_mem_manager;

#endif /* SNES_MEMORY_MANAGER_H */
