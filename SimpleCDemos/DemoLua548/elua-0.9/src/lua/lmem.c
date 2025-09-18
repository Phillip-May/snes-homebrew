/*
** $Id: lmem.c,v 1.70.1.1 2007/12/27 13:02:25 roberto Exp $
** Interface to Memory Manager
** See Copyright Notice in lua.h
*/


#include <stddef.h>
#include <stdio.h>
#include <stdint.h>  /* For uint32_t */

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
** void * frealloc (void *ud, void *ptr, uint32_t osize, uint32_t nsize);
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


void *luaM_growaux_ (lua_State *L, void *block, int *size, uint32_t size_elems,
                     int limit, const char *errormsg) {
  void *newblock;
  uint32_t newsize;  /* Use 32-bit to prevent overflow */
  
  if (*size >= limit/2) {  /* cannot double it? */
    if (*size >= limit)  /* cannot grow even a little? */
      luaG_runerror(L, errormsg);
    newsize = (uint32_t)limit;  /* still have at least one free place */
  }
  else {
    newsize = (uint32_t)(*size) * 2;  /* Explicit 32-bit arithmetic */
    if (newsize < MINSIZEARRAY)
      newsize = MINSIZEARRAY;  /* minimum size */
    
    /* CRITICAL: Cap buffer growth to prevent 65KB allocations on SNES */
    if (newsize > 32768) {  /* Cap at 32KB - fits in largest blocks */
      newsize = 32768;
    }
  }
  
  newblock = luaM_reallocv(L, block, *size, (int)newsize, size_elems);
  *size = (int)newsize;  /* update only when everything else is OK - cast back to int */
  return newblock;
}


void *luaM_toobig (lua_State *L) {
  luaG_runerror(L, "memory allocation error: block too big");
  return NULL;  /* to avoid warnings */
}

void *luaM_reallocv_impl (lua_State *L, void *block, int oldn, int n, uint32_t size_elem) {
  void* result = luaM_realloc_(L, block, (uint32_t)oldn * size_elem, (uint32_t)n * size_elem);
  return result;
}



/*
** generic allocation routine.
*/
void *luaM_realloc_ (lua_State *L, void *block, uint32_t osize, uint32_t nsize) {
  global_State *g = G(L);

  lua_assert((osize == 0) == (block == NULL));

  /* Call our allocator directly to bypass calling convention issues */
  extern void* snes_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize);
  block = snes_lua_alloc(g->ud, block, (size_t)osize, (size_t)nsize);

  if (block == NULL && nsize > 0) {
    luaD_throw(L, LUA_ERRMEM);
  }

  lua_assert((nsize == 0) == (block == NULL));
  g->totalbytes = (g->totalbytes - osize) + nsize;

  return block;
}

