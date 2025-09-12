/*
** $Id: lmem.c,v 1.70.1.1 2007/12/27 13:02:25 roberto Exp $
** Interface to Memory Manager
** See Copyright Notice in lua.h
*/


#include <stddef.h>
#include <stdio.h>

#define lmem_c
#define LUA_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"



/*
** About the realloc function:
** void * frealloc (void *ud, void *ptr, size_t osize, size_t nsize);
** (`osize' is the old size, `nsize' is the new size)
**
** Lua ensures that (ptr == NULL) iff (osize == 0).
**
** * frealloc(ud, NULL, 0, x) creates a new block of size `x'
**
** * frealloc(ud, p, x, 0) frees the block `p'
** (in this specific case, frealloc must return NULL).
** particularly, frealloc(ud, NULL, 0, 0) does nothing
** (which is equivalent to free(NULL) in ANSI C)
**
** frealloc returns NULL if it cannot create or reallocate the area
** (any reallocation to an equal or smaller size cannot fail!)
*/



#define MINSIZEARRAY	4


void *luaM_growaux_ (lua_State *L, void *block, int *size, size_t size_elems,
                     int limit, const char *errormsg) {
  void *newblock;
  int newsize;
  
  /* Debug: Add external debug function declaration */
  extern void termM0PrintStringXY_scroll(const char* str, int x, int y);
  static int debug_line = 50;
  
  termM0PrintStringXY_scroll("growaux: start", 0, debug_line++);
  
  /* Debug: Show current size and limit */
  if (*size == 0) {
    termM0PrintStringXY_scroll("growaux: size=0", 0, debug_line++);
  } else if (*size == 1) {
    termM0PrintStringXY_scroll("growaux: size=1", 0, debug_line++);
  } else if (*size == 2) {
    termM0PrintStringXY_scroll("growaux: size=2", 0, debug_line++);
  } else {
    termM0PrintStringXY_scroll("growaux: size>2", 0, debug_line++);
  }
  
  if (*size >= limit/2) {  /* cannot double it? */
    if (*size >= limit)  /* cannot grow even a little? */
      luaG_runerror(L, errormsg);
    newsize = limit;  /* still have at least one free place */
  }
  else {
    newsize = (*size)*2;
    if (newsize < MINSIZEARRAY)
      newsize = MINSIZEARRAY;  /* minimum size */
  }
  
  /* Debug: Show new size */
  if (newsize == 2) {
    termM0PrintStringXY_scroll("growaux: newsize=2", 0, debug_line++);
  } else if (newsize == 4) {
    termM0PrintStringXY_scroll("growaux: newsize=4", 0, debug_line++);
  } else {
    termM0PrintStringXY_scroll("growaux: newsize>4", 0, debug_line++);
  }
  
  termM0PrintStringXY_scroll("growaux: before reallocv", 0, debug_line++);
  newblock = luaM_reallocv(L, block, *size, newsize, size_elems);
  *size = newsize;  /* update only when everything else is OK */
  termM0PrintStringXY_scroll("growaux: done", 0, debug_line++);
  return newblock;
}


void *luaM_toobig (lua_State *L) {
  /* Debug: Add external debug function declaration */
  extern void termM0PrintStringXY_scroll(const char* str, int x, int y);
  static int debug_line = 50;
  
  termM0PrintStringXY_scroll("luaM_toobig: called!", 0, debug_line++);
  
  /* Debug: Print size values */
  char size_str[64];
  sprintf(size_str, "MAX_SIZET: %u", (unsigned int)MAX_SIZET);
  termM0PrintStringXY_scroll(size_str, 0, debug_line++);
  
  sprintf(size_str, "BASIC_CI_SIZE: %d", BASIC_CI_SIZE);
  termM0PrintStringXY_scroll(size_str, 0, debug_line++);
  
  sprintf(size_str, "sizeof(CallInfo): %u", (unsigned int)sizeof(CallInfo));
  termM0PrintStringXY_scroll(size_str, 0, debug_line++);
  
  size_t total_size = (size_t)(BASIC_CI_SIZE + 1) * sizeof(CallInfo);
  sprintf(size_str, "total_size: %u", (unsigned int)total_size);
  termM0PrintStringXY_scroll(size_str, 0, debug_line++);
  
  /* Debug the actual condition values */
  size_t n_plus_one = (size_t)(BASIC_CI_SIZE + 1);
  size_t max_div_elem = MAX_SIZET / sizeof(CallInfo);
  sprintf(size_str, "n_plus_one: %u", (unsigned int)n_plus_one);
  termM0PrintStringXY_scroll(size_str, 0, debug_line++);
  
  sprintf(size_str, "max_div_elem: %u", (unsigned int)max_div_elem);
  termM0PrintStringXY_scroll(size_str, 0, debug_line++);
  
  int condition_result = (n_plus_one <= max_div_elem);
  sprintf(size_str, "condition: %s", condition_result ? "TRUE" : "FALSE");
  termM0PrintStringXY_scroll(size_str, 0, debug_line++);
  
  luaG_runerror(L, "memory allocation error: block too big");
  termM0PrintStringXY_scroll("luaM_toobig: after runerror", 0, debug_line++);
  return NULL;  /* to avoid warnings */
}

void *luaM_reallocv_impl (lua_State *L, void *block, int oldn, int n, size_t size_elem) {
  /* Debug: Add external debug function declaration */
  extern void termM0PrintStringXY_scroll(const char* str, int x, int y);
  static int debug_line = 25;
  
  termM0PrintStringXY_scroll("luaM_reallocv: before realloc", 0, debug_line++);
  termM0PrintStringXY_scroll("luaM_reallocv: calling luaM_realloc_", 0, debug_line++);
  
  /* Print stack variable pointers to check for corruption */
  unsigned long L_ptr = (unsigned long)L;
  unsigned long block_ptr = (unsigned long)block;
  unsigned long L_high = (L_ptr >> 16) & 0xFFFF;
  unsigned long L_low = L_ptr & 0xFFFF;
  unsigned long block_high = (block_ptr >> 16) & 0xFFFF;
  unsigned long block_low = block_ptr & 0xFFFF;
  
  char ptr_str[64];
  sprintf(ptr_str, "STACK L: 0x%04X%04X", (unsigned int)L_high, (unsigned int)L_low);
  termM0PrintStringXY_scroll(ptr_str, 0, debug_line++);
  sprintf(ptr_str, "STACK BLK: 0x%04X%04X", (unsigned int)block_high, (unsigned int)block_low);
  termM0PrintStringXY_scroll(ptr_str, 0, debug_line++);
  
  void* result = luaM_realloc_(L, block, oldn * size_elem, n * size_elem);
  termM0PrintStringXY_scroll("luaM_reallocv: luaM_realloc_ returned", 0, debug_line++);
  
  /* Print result pointer */
  unsigned long result_ptr = (unsigned long)result;
  unsigned long result_high = (result_ptr >> 16) & 0xFFFF;
  unsigned long result_low = result_ptr & 0xFFFF;
  sprintf(ptr_str, "STACK RESULT: 0x%04X%04X", (unsigned int)result_high, (unsigned int)result_low);
  termM0PrintStringXY_scroll(ptr_str, 0, debug_line++);
  
  termM0PrintStringXY_scroll("luaM_reallocv: about to return result", 0, debug_line++);
  termM0PrintStringXY_scroll("luaM_reallocv: after realloc", 0, debug_line++);
  return result;
}



/*
** generic allocation routine.
*/
void *luaM_realloc_ (lua_State *L, void *block, size_t osize, size_t nsize) {
  global_State *g = G(L);

  lua_assert((osize == 0) == (block == NULL));

  /* Debug: Add external debug function declaration */
  extern void termM0PrintStringXY_scroll(const char* str, int x, int y);
  static int debug_line = 40;
  
  termM0PrintStringXY_scroll("luaM_realloc_: start", 0, debug_line++);

  /* Check for stack overflow by printing stack variable address */
  int stack_var = 0x1234;
  unsigned long stack_addr = (unsigned long)&stack_var;
  unsigned long stack_high = (stack_addr >> 16) & 0xFFFF;
  unsigned long stack_low = stack_addr & 0xFFFF;
  char stack_str[64];
  sprintf(stack_str, "STACK ADDR: 0x%04X%04X", (unsigned int)stack_high, (unsigned int)stack_low);
  termM0PrintStringXY_scroll(stack_str, 0, debug_line++);

  /* Call our allocator directly to bypass calling convention issues */
  extern void* snes_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize);
  termM0PrintStringXY_scroll("luaM_realloc_: calling snes_lua_alloc", 0, debug_line++);
  block = snes_lua_alloc(g->ud, block, osize, nsize);
  termM0PrintStringXY_scroll("luaM_realloc_: snes_lua_alloc returned", 0, debug_line++);

  if (block == NULL && nsize > 0) {
    termM0PrintStringXY_scroll("luaM_realloc_: NULL block, throwing", 0, debug_line++);
    luaD_throw(L, LUA_ERRMEM);
  }

  termM0PrintStringXY_scroll("luaM_realloc_: checking assert", 0, debug_line++);
  lua_assert((nsize == 0) == (block == NULL));
  termM0PrintStringXY_scroll("luaM_realloc_: assert OK", 0, debug_line++);

  termM0PrintStringXY_scroll("luaM_realloc_: about to update totalbytes", 0, debug_line++);
  g->totalbytes = (g->totalbytes - osize) + nsize;
  termM0PrintStringXY_scroll("luaM_realloc_: totalbytes updated", 0, debug_line++);

  termM0PrintStringXY_scroll("luaM_realloc_: about to return", 0, debug_line++);
  return block;
}

