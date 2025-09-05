//For some reason wdc816cc's stdint.h uses cursed defines instead of typedefs
//however inttypes.h uses typedefs
#ifndef __CUSTOM_LIBFIXMATH_DEFINES_H__
#define __CUSTOM_LIBFIXMATH_DEFINES_H__
#include <inttypes.h>


#ifdef __WDC816CC__
#pragma section CODE=FIXEDMATH
#endif


#define FIXMATH_NO_64BIT
#define FIXMATH_NO_CACHE
#define FIXMATH_NO_OVERFLOW
//Not technically 16 bit, but also the regular version uses uint64_t
//#define FIXMATH_OPTIMIZE_8BIT
#define inline

#endif
