/*
** $Id: lstate.c,v 2.36.1.2 2008/01/03 15:20:39 roberto Exp $
** Global State
** See Copyright Notice in lua.h
*/


#include <stddef.h>

#define lstate_c
#define LUA_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "llex.h"
#include "lmem.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lcode.h"
#include "platform_conf.h"
// BogdanM: modified for Lua interrupt support
#ifndef LUA_CROSS_COMPILER
#include "elua_int.h"
#include "platform.h"
#endif

/* Debug function declaration */
extern void termM0PrintStringXY_scroll(const char* str, int x, int y);
// BogdanM: linenoise clenaup
#include "linenoise.h"

#define state_size(x)	(sizeof(x) + LUAI_EXTRASPACE)
#define fromstate(l)	(cast(lu_byte *, (l)) - LUAI_EXTRASPACE)
#define tostate(l)   (cast(lua_State *, cast(lu_byte *, l) + LUAI_EXTRASPACE))


/*
** Main thread combines a thread state and the global state
*/
typedef struct LG {
  lua_State l;
  global_State g;
} LG;
  


static void stack_init (lua_State *L1, lua_State *L) {
  /* Debug: Add external debug function declaration */
  extern void termM0PrintStringXY_scroll(const char* str, int x, int y);
  static int debug_line = 10;
  
  termM0PrintStringXY_scroll("stack_init: start", 0, debug_line++);
  
  /* Check for stack corruption - verify L1 pointer is valid */
  if (L1 == NULL) {
    termM0PrintStringXY_scroll("STACK CORRUPT: L1 NULL!", 0, debug_line++);
    return;
  }
  
  /* Check for stack corruption - verify L pointer is valid */
  if (L == NULL) {
    termM0PrintStringXY_scroll("STACK CORRUPT: L NULL!", 0, debug_line++);
    return;
  }
  
  /* Check for stack corruption - verify L1 is in valid memory range (SA-1 SRAM) */
  if (L1 < (lua_State*)0x400000 || L1 > (lua_State*)0x500000) {
    termM0PrintStringXY_scroll("STACK CORRUPT: L1 BAD!", 0, debug_line++);
    return;
  }
  
  /* initialize CallInfo array */
  termM0PrintStringXY_scroll("stack_init: luaM_newvector ci", 0, debug_line++);
  L1->base_ci = luaM_newvector(L, BASIC_CI_SIZE, CallInfo);
  
  /* Check if allocation succeeded */
  if (L1->base_ci == NULL) {
    termM0PrintStringXY_scroll("STACK CORRUPT: CI ALLOC FAIL!", 0, debug_line++);
    return;
  }
  
  /* Check if allocated pointer is valid (SA-1 SRAM range) */
  if (L1->base_ci < (CallInfo*)0x400000 || L1->base_ci > (CallInfo*)0x500000) {
    termM0PrintStringXY_scroll("STACK CORRUPT: CI BAD PTR!", 0, debug_line++);
    return;
  }
  
  termM0PrintStringXY_scroll("stack_init: ci alloc done", 0, debug_line++);
  
  L1->ci = L1->base_ci;
  L1->size_ci = BASIC_CI_SIZE;
  L1->end_ci = L1->base_ci + L1->size_ci - 1;
  
  /* Check for stack corruption - verify ci pointer is valid */
  if (L1->ci == NULL) {
    termM0PrintStringXY_scroll("STACK CORRUPT: CI NULL!", 0, debug_line++);
    return;
  }
  
  /* initialize stack array */
  termM0PrintStringXY_scroll("stack_init: luaM_newvector stack", 0, debug_line++);
  L1->stack = luaM_newvector(L, BASIC_STACK_SIZE + EXTRA_STACK, TValue);
  
  /* Check if allocation succeeded */
  if (L1->stack == NULL) {
    termM0PrintStringXY_scroll("STACK CORRUPT: STACK ALLOC FAIL!", 0, debug_line++);
    return;
  }
  
  /* Check if allocated pointer is valid (SA-1 SRAM range) */
  if (L1->stack < (TValue*)0x400000 || L1->stack > (TValue*)0x500000) {
    termM0PrintStringXY_scroll("STACK CORRUPT: STACK BAD PTR!", 0, debug_line++);
    return;
  }
  
  termM0PrintStringXY_scroll("stack_init: stack alloc done", 0, debug_line++);
  
  L1->stacksize = BASIC_STACK_SIZE + EXTRA_STACK;
  L1->top = L1->stack;
  L1->stack_last = L1->stack+(L1->stacksize - EXTRA_STACK)-1;
  
  /* Check for stack corruption - verify stack pointers are valid */
  if (L1->top == NULL || L1->stack_last == NULL) {
    termM0PrintStringXY_scroll("STACK CORRUPT: STACK PTRS NULL!", 0, debug_line++);
    return;
  }
  
  /* initialize first ci */
  L1->ci->func = L1->top;
  setnilvalue(L1->top++);  /* `function' entry for this `ci' */
  L1->base = L1->ci->base = L1->top;
  L1->ci->top = L1->top + LUA_MINSTACK;
  
  termM0PrintStringXY_scroll("stack_init: complete", 0, debug_line++);
}


static void freestack (lua_State *L, lua_State *L1) {
  luaM_freearray(L, L1->base_ci, L1->size_ci, CallInfo);
  luaM_freearray(L, L1->stack, L1->stacksize, TValue);
}


/*
** open parts that may cause memory-allocation errors
*/
static void f_luaopen (lua_State *L, void *ud) {
  global_State *g = G(L);
  UNUSED(ud);
  
  /* Debug: Add external debug function declaration */
  extern void termM0PrintStringXY_scroll(const char* str, int x, int y);
  static int debug_line = 0;
  
  termM0PrintStringXY_scroll("f_luaopen: start", 0, debug_line++);
  
  termM0PrintStringXY_scroll("f_luaopen: stack_init", 0, debug_line++);
  stack_init(L, L);  /* init stack */
  termM0PrintStringXY_scroll("f_luaopen: stack_init OK", 0, debug_line++);
  
  termM0PrintStringXY_scroll("f_luaopen: luaH_new globals", 0, debug_line++);
  sethvalue(L, gt(L), luaH_new(L, 0, 2));  /* table of globals */
  termM0PrintStringXY_scroll("f_luaopen: luaH_new globals OK", 0, debug_line++);
  
  termM0PrintStringXY_scroll("f_luaopen: luaH_new registry", 0, debug_line++);
  sethvalue(L, registry(L), luaH_new(L, 0, 2));  /* registry */
  termM0PrintStringXY_scroll("f_luaopen: luaH_new registry OK", 0, debug_line++);
  
  termM0PrintStringXY_scroll("f_luaopen: luaS_resize", 0, debug_line++);
  luaS_resize(L, MINSTRTABSIZE);  /* initial size of string table */
  termM0PrintStringXY_scroll("f_luaopen: luaS_resize OK", 0, debug_line++);
  
  termM0PrintStringXY_scroll("f_luaopen: luaT_init", 0, debug_line++);
  luaT_init(L);
  termM0PrintStringXY_scroll("f_luaopen: luaT_init OK", 0, debug_line++);
  
  termM0PrintStringXY_scroll("f_luaopen: luaX_init", 0, debug_line++);
  luaX_init(L);
  termM0PrintStringXY_scroll("f_luaopen: luaX_init OK", 0, debug_line++);
  
  termM0PrintStringXY_scroll("f_luaopen: luaS_fix", 0, debug_line++);
  luaS_fix(luaS_newliteral(L, MEMERRMSG));
  termM0PrintStringXY_scroll("f_luaopen: luaS_fix OK", 0, debug_line++);
  
  termM0PrintStringXY_scroll("f_luaopen: GCthreshold", 0, debug_line++);
  g->GCthreshold = 4*g->totalbytes;
  termM0PrintStringXY_scroll("f_luaopen: GCthreshold OK", 0, debug_line++);
  
  termM0PrintStringXY_scroll("f_luaopen: complete", 0, debug_line++);
}


static void preinit_state (lua_State *L, global_State *g) {
  G(L) = g;
  L->stack = NULL;
  L->stacksize = 0;
  L->errorJmp = NULL;
  L->hook = NULL;
  L->hookmask = 0;
  L->basehookcount = 0;
  L->allowhook = 1;
  resethookcount(L);
  L->openupval = NULL;
  L->size_ci = 0;
  L->nCcalls = L->baseCcalls = 0;
  L->status = 0;
  L->base_ci = L->ci = NULL;
  L->savedpc = NULL;
  L->errfunc = 0;
  setnilvalue(gt(L));
}


static void close_state (lua_State *L) {
  global_State *g = G(L);
  luaF_close(L, L->stack);  /* close all upvalues for this thread */
  luaC_freeall(L);  /* collect all objects */
  lua_assert(g->rootgc == obj2gco(L));
  lua_assert(g->strt.nuse == 0);
  luaM_freearray(L, G(L)->strt.hash, G(L)->strt.size, TString *);
  luaZ_freebuffer(L, &g->buff);
  freestack(L, L);
  lua_assert(g->totalbytes == sizeof(LG));
  (*g->frealloc)(g->ud, fromstate(L), state_size(LG), 0);
}


lua_State *luaE_newthread (lua_State *L) {
  lua_State *L1 = tostate(luaM_malloc(L, state_size(lua_State)));
  luaC_link(L, obj2gco(L1), LUA_TTHREAD);
  setthvalue(L, L->top, L1); /* put thread on stack */
  incr_top(L);
  preinit_state(L1, G(L));
  stack_init(L1, L);  /* init stack */
  setobj2n(L, gt(L1), gt(L));  /* share table of globals */
  L1->hookmask = L->hookmask;
  L1->basehookcount = L->basehookcount;
  L1->hook = L->hook;
  resethookcount(L1);
  lua_assert(!isdead(G(L), obj2gco(L1)));
  L->top--; /* remove thread from stack */
  return L1;
}


void luaE_freethread (lua_State *L, lua_State *L1) {
  luaF_close(L1, L1->stack);  /* close all upvalues for this thread */
  lua_assert(L1->openupval == NULL);
  luai_userstatefree(L1);
  freestack(L, L1);
  luaM_freemem(L, fromstate(L1), state_size(lua_State));
}


LUA_API lua_State *lua_newstate (lua_Alloc f, void *ud) {
  static int debug_line = 30;
  
  termM0PrintStringXY_scroll("lua_newstate: start", 0, debug_line++);
  
  int i;
  lua_State *L;
  global_State *g;
  
  termM0PrintStringXY_scroll("lua_newstate: alloc LG", 0, debug_line++);
  void *l = (*f)(ud, NULL, 0, state_size(LG));
  if (l == NULL) {
    termM0PrintStringXY_scroll("lua_newstate: alloc failed", 0, debug_line++);
    return NULL;
  }
  termM0PrintStringXY_scroll("lua_newstate: alloc OK", 0, debug_line++);
  
  /* Check for corruption - verify allocated pointer is valid (SA-1 SRAM range) */
  if (l < (void*)0x400000 || l > (void*)0x500000) {
    termM0PrintStringXY_scroll("CORRUPT: l bad ptr!", 0, debug_line++);
    return NULL;
  }
  
  L = tostate(l);
  g = &((LG *)L)->g;
  
  /* Check for corruption - verify L pointer is valid */
  if (L == NULL) {
    termM0PrintStringXY_scroll("CORRUPT: L NULL!", 0, debug_line++);
    return NULL;
  }
  
  /* Check for corruption - verify g pointer is valid */
  if (g == NULL) {
    termM0PrintStringXY_scroll("CORRUPT: g NULL!", 0, debug_line++);
    return NULL;
  }
  
  L->next = NULL;
  L->tt = LUA_TTHREAD;
  
  g->currentwhite = bit2mask(WHITE0BIT, FIXEDBIT);
  L->marked = luaC_white(g);
  set2bits(L->marked, FIXEDBIT, SFIXEDBIT);
  
  termM0PrintStringXY_scroll("lua_newstate: preinit_state", 0, debug_line++);
  preinit_state(L, g);
  termM0PrintStringXY_scroll("lua_newstate: preinit_state OK", 0, debug_line++);
  
  g->frealloc = f;
  g->ud = ud;
  g->mainthread = L;
  
  g->uvhead.u.l.prev = &g->uvhead;
  g->uvhead.u.l.next = &g->uvhead;
  
  g->GCthreshold = 0;  /* mark it as unfinished state */
  g->estimate = 0;
  g->strt.size = 0;
  g->strt.nuse = 0;
  g->strt.hash = NULL;
  
  setnilvalue(registry(L));
  luaZ_initbuffer(L, &g->buff);
  
  g->panic = NULL;
  
  g->gcstate = GCSpause;
  g->gcflags = GCFlagsNone;
  g->rootgc = obj2gco(L);
  g->sweepstrgc = 0;
  g->sweepgc = &g->rootgc;
  g->gray = NULL;
  g->grayagain = NULL;
  g->weak = NULL;
  g->tmudata = NULL;
  
  g->totalbytes = sizeof(LG);
  g->memlimit = 0;
  g->gcpause = LUAI_GCPAUSE;
  g->gcstepmul = LUAI_GCMUL;
  g->gcdept = 0;
  
#ifdef EGC_INITIAL_MODE
  g->egcmode = EGC_INITIAL_MODE;
#else
  g->egcmode = 0;
#endif
  
#ifdef EGC_INITIAL_MEMLIMIT
  g->memlimit = EGC_INITIAL_MEMLIMIT;
#else
  g->memlimit = 0;
#endif
  
  for (i=0; i<NUM_TAGS; i++) g->mt[i] = NULL;
  
  termM0PrintStringXY_scroll("lua_newstate: call f_luaopen", 0, debug_line++);
  if (luaD_rawrunprotected(L, f_luaopen, NULL) != 0) {
    termM0PrintStringXY_scroll("lua_newstate: f_luaopen failed", 0, debug_line++);
    /* memory allocation error: free partial state */
    close_state(L);
    L = NULL;
  }
  else {
    termM0PrintStringXY_scroll("lua_newstate: f_luaopen OK", 0, debug_line++);
    luai_userstateopen(L);
  }
  
  termM0PrintStringXY_scroll("lua_newstate: return", 0, debug_line++);
  return L;
}


static void callallgcTM (lua_State *L, void *ud) {
  UNUSED(ud);
  luaC_callGCTM(L);  /* call GC metamethods for all udata */
}

// BogdanM: modified for eLua interrupt support
extern lua_State *luaL_newstate (void);
static lua_State *lua_crtstate;

lua_State *lua_open(void) {
  lua_crtstate = luaL_newstate(); 
  return lua_crtstate;
}

lua_State *lua_getstate(void) {
  return lua_crtstate;
}
LUA_API void lua_close (lua_State *L) {
#ifndef LUA_CROSS_COMPILER  
  int oldstate = platform_cpu_set_global_interrupts( PLATFORM_CPU_DISABLE );
  lua_sethook( L, NULL, 0, 0 );
  lua_crtstate = NULL;
  lua_pushnil( L );
  lua_rawseti( L, LUA_REGISTRYINDEX, LUA_INT_HANDLER_KEY );
  elua_int_cleanup();
  platform_cpu_set_global_interrupts( oldstate );
  linenoise_cleanup( LINENOISE_ID_LUA );
#endif  
  L = G(L)->mainthread;  /* only the main thread can be closed */
  lua_lock(L);
  luaF_close(L, L->stack);  /* close all upvalues for this thread */
  luaC_separateudata(L, 1);  /* separate udata that have GC metamethods */
  L->errfunc = 0;  /* no error function during GC metamethods */
  do {  /* repeat until no more errors */
    L->ci = L->base_ci;
    L->base = L->top = L->ci->base;
    L->nCcalls = L->baseCcalls = 0;
  } while (luaD_rawrunprotected(L, callallgcTM, NULL) != 0);
  lua_assert(G(L)->tmudata == NULL);
  luai_userstateclose(L);
  close_state(L);
}

