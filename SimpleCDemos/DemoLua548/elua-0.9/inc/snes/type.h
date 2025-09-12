// Type definitions for SNES platform

#ifndef __TYPE_H__
#define __TYPE_H__

// Basic integer types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef signed char s8;
typedef signed short s16;
typedef signed long s32;

// 64-bit types
typedef unsigned long long u64;
typedef signed long long s64;

// Size types - use system definitions to avoid conflicts
#ifndef __SIZE_TYPE__
typedef unsigned long size_t;
#endif
#ifndef __SSIZE_TYPE__
typedef signed long ssize_t;
#endif

// Boolean type
typedef int bool;
#define true 1
#define false 0

// NULL definition
#ifndef NULL
#define NULL ((void*)0)
#endif

// Maximum values
#define U8_MAX 0xFF
#define U16_MAX 0xFFFF
#define U32_MAX 0xFFFFFFFFUL
#define S8_MAX 0x7F
#define S16_MAX 0x7FFF
#define S32_MAX 0x7FFFFFFFL

#define S8_MIN (-S8_MAX - 1)
#define S16_MIN (-S16_MAX - 1)
#define S32_MIN (-S32_MAX - 1)

#endif // #ifndef __TYPE_H__
