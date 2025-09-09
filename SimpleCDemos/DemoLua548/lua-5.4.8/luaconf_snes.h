/*
** SNES-compatible configuration for Lua 5.4.8
** Based on luaconf.h but adapted for SNES/65816 platform
*/

#ifndef luaconf_h
#define luaconf_h

#include <limits.h>
#include <stddef.h>

/* SNES-specific configuration */
#define LUA_USE_C89
#define LUA_32BITS

/* Basic types for SNES */
#define LUA_INT_TYPE	LUA_INT_INT
#define LUA_FLOAT_TYPE	LUA_FLOAT_DOUBLE

/* Memory management - use simple malloc/free */
#define LUA_USE_MEMORY_MANAGER

/* Stack configuration - smaller for SNES */
#define LUAI_MAXSTACK	1000
#define LUAI_MAXCSTACK	2000

/* String configuration */
#define LUAI_MAXSHORTLEN	40
#define LUA_MAXUNSIGNED		UINT_MAX

/* Table configuration */
#define LUAI_MAXASIZE		(1 << (LUAI_BITSINT - 2))
#define LUAI_MAXHASHSIZE	(1 << (LUAI_BITSINT - 1))

/* Garbage collection - simplified for SNES */
#define LUA_GCSTEPMUL		200
#define LUA_GCSTEPSIZE		200

/* Disable features not needed on SNES */
#undef LUA_USE_DLOPEN
#undef LUA_USE_READLINE
#undef LUA_USE_WINDOWS
#undef LUA_USE_POSIX

/* Disable debug features to save memory */
#undef LUA_USE_APICHECK
#undef LUA_USE_ASSERT

/* Basic integer and float types */
typedef int LUA_INT32;
typedef unsigned int LUA_UINT32;
typedef double lua_Number;
typedef LUA_INT32 lua_Integer;

/* Platform-specific includes */
#include <stdarg.h>
#include <stddef.h>

/* Memory allocation functions */
#define luaM_malloc(L, s)	malloc(s)
#define luaM_free(L, p)	free(p)
#define luaM_realloc(L, p, os, s)	realloc(p, s)

/* Error handling */
#define LUAI_THROW(L,c)		longjmp((c)->b, 1)
#define LUAI_TRY(L,c,a)		if (setjmp((c)->b) == 0) { a }
#define luai_jmpbuf		jmp_buf

/* Basic macros */
#define lua_assert(c)		((void)0)
#define check_exp(c,e)		(e)
#define lua_longassert(c)	((void)0)

/* String operations */
#define luaS_new(L, s)		luaS_newlstr(L, s, strlen(s))
#define luaS_newliteral(L, s)	luaS_new(L, "" s, (sizeof(s)/sizeof(char))-1)

/* Table operations */
#define luaH_getint(t,i)	luaH_get(t, luaO_pushinteger(L, i))

/* VM operations */
#define luaV_fastget(L,t,k,v,slot) \
  (ttistable(t) && (slot = luaH_get(t, k)) != luaO_nilobject && \
   (v = luaV_gettable(L, t, k, slot), 1))

/* Basic constants */
#define LUA_MAXINTEGER		INT_MAX
#define LUA_MININTEGER		INT_MIN

/* Thread status */
#define LUA_OK		0
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRGCMM	5
#define LUA_ERRERR	6

/* Call info */
#define CIST_OAH	(1<<0)	/* original value of 'allowhook' */
#define CIST_C		(1<<1)	/* call is running a C function */
#define CIST_FRESH	(1<<2)	/* call is on a fresh "luaV_execute" frame */
#define CIST_HOOKYIELD	(1<<3)	/* last hook called yielded */
#define CIST_LEQ	(1<<4)	/* using __lt for __le */
#define CIST_FIN	(1<<5)	/* call is finishing (finishing variable) */
#define CIST_TAIL	(1<<6)	/* call was tail called */
#define CIST_HOOKED	(1<<7)	/* call is running a debug hook */
#define CIST_YPCALL	(1<<8)	/* call is a yieldable protected call */
#define CIST_TAIL	(1<<9)	/* call was tail called */

/* Basic function declarations */
void luaM_free (void *L, void *ptr, size_t osize);
void *luaM_realloc_ (void *L, void *ptr, size_t osize, size_t nsize);
void *luaM_growaux_ (void *L, void *block, int *size, size_t size_elem, int limit, const char *what);

/* Include the rest of the standard configuration */
#include "luaconf.h"

#endif
