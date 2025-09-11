/*
** SNES-specific Lua configuration
** Minimal configuration for embedded Lua on SNES
*/

#ifndef luaconf_h
#define luaconf_h

#include <limits.h>
#include <stddef.h>

/* Basic system configuration */
#define LUA_USE_C89
#define LUA_32BITS

/* Memory configuration - very conservative for SNES */
#define LUAI_MAXSTACK		100
#define LUAI_MAXCSTACK		200
#define LUAI_MAXCCALLS		50
#define LUAI_MAXVARS		50
#define LUAI_MAXUPVALUES	50

/* String configuration */
#define LUA_MAXLENGTH		(INT_MAX - 2)
#define LUA_MAXSHORTLEN		40

/* Table configuration */
#define LUA_MAXINDEX		(INT_MAX - 2)
#define LUA_MAXUPVAL		255

/* Garbage collection configuration - let the main luaconf.h define these */

/* Disable features we don't need on SNES */
#define LUA_USE_APICHECK	0
#define LUA_USE_ASSERT		0
#define LUA_USE_CTYPE		0

/* Disable libraries we don't need */
#define LUA_USE_LINENOISE	0
#define LUA_USE_READLINE	0

/* Math configuration */
#define LUA_NUMBER		double
#define LUA_INTEGER		long
#define LUA_MAXINTEGER		LONG_MAX
#define LUA_MININTEGER		LONG_MIN

/* String operations */
#define lua_strlen(s)		strlen(s)
#define lua_strcmp(a,b)		strcmp(a,b)

/* Memory allocation - use simple malloc/free */
#define luaM_new(L,t)		((t *)malloc(sizeof(t)))
#define luaM_newvector(L,n,t)	((t *)malloc((n)*sizeof(t)))
#define luaM_reallocvector(L,v,oldn,n,t) \
   ((v)=(t *)realloc(v,(n)*sizeof(t)))
#define luaM_free(L, b)		(free(b))
#define luaM_freearray(L, b, n)	(free(b))

/* Error handling */
#define lua_assert(c)		((void)0)

/* Override MAX_INT to fix opcode limit issue on SNES */
/* The default MAX_INT = INT_MAX (32767) is too small for Lua opcodes */
/* We need a larger value that fits in 32-bit but is much larger than 16-bit */
#undef MAX_INT
#define MAX_INT 0x7FFFFFFF  /* 2^31 - 1, maximum 32-bit signed integer */

/* Define missing types that Lua needs */
#define LUA_UNSIGNED unsigned long
#define LUA_KCONTEXT ptrdiff_t
#define LUA_API extern
#define LUALIB_API extern

/* Define missing constants */
#define LUA_IDSIZE 60
#define LUAL_BUFFERSIZE 8192
#define LUAI_MAXALIGN long double

/* Include the original luaconf.h for everything else */
#include "lua-5.4.8/luaconf.h"

#endif
