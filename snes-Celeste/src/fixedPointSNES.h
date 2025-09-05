#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H

#include <inttypes.h>

#ifndef __WDC816CC__

#define inline
#include "libfixmath/fixmath.h"
#undef inline

// Define fixed_t as fix16_t from libfixmath
typedef fix16_t fixed_t;
#define FIXED_ABS(x) ((x) < 0 ? -(x) : (x))


// Basic arithmetic operations using libfixmath
#define FIXED_ADD(a, b) (fix16_add((a), (b)))
#define FIXED_SUB(a, b) (fix16_sub((a), (b)))
#define FIXED_MUL(a, b) (fix16_mul((a), (b)))
#define FIXED_DIV(a, b) (fix16_div((a), (b)))

// Conversion macros
#define FLOAT_TO_FIXED(x) (fix16_from_float((x)))
#define INT_TO_FIXED(x) (fix16_from_int((x)))
#define FIXED_TO_INT(x) (fix16_to_int((x)))
#define FIXED_TO_FLOAT(x) (fix16_to_float((x)))

// Utility macros
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define FIXED_CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))
#define FIXED_FLOOR(x) (fix16_floor((x)))
#define FIXED_CEIL(x) (fix16_ceil((x)))

// Trigonometric functions
#define FIXED_SIN(x) (fix16_sin((x)))
#define FIXED_COS(x) (fix16_cos((x)))
#define FIXED_TAN(x) (fix16_tan((x)))
#define FIXED_ASIN(x) (fix16_asin((x)))
#define FIXED_ACOS(x) (fix16_acos((x)))
#define FIXED_ATAN(x) (fix16_atan((x)))
#define FIXED_ATAN2(y, x) (fix16_atan2((y), (x)))

// Mathematical functions
#define FIXED_SQRT(x) fix16_sqrt(x)
#define FIXED_SQ(x) fix16_sq(x)
#define FIXED_EXP(x) fix16_exp(x)
#define FIXED_LOG(x) fix16_log(x)
#define FIXED_LOG2(x) fix16_log2(x)

// Angle conversion
#define FIXED_RAD_TO_DEG(x) fix16_rad_to_deg(x)
#define FIXED_DEG_TO_RAD(x) fix16_deg_to_rad(x)

// Linear interpolation
#define FIXED_LERP8(a, b, t) fix16_lerp8(a, b, t)
#define FIXED_LERP16(a, b, t) fix16_lerp16(a, b, t)
#define FIXED_LERP32(a, b, t) fix16_lerp32(a, b, t)

// Saturated arithmetic (overflow protection)
#define FIXED_SADD(a, b) fix16_sadd(a, b)
#define FIXED_SSUB(a, b) fix16_ssub(a, b)
#define FIXED_SMUL(a, b) fix16_smul(a, b)
#define FIXED_SDIV(a, b) fix16_sdiv(a, b)

// Modulo operation
#define FIXED_MOD(x, y) fix16_mod(x, y)

// String conversion (if needed)
#define FIXED_TO_STR(x, buf, decimals) fix16_to_str(x, buf, decimals)
#define FIXED_FROM_STR(buf) fix16_from_str(buf)

// Comparison macros
#define FIXED_EQ(a, b) ((a) == (b))
#define FIXED_NE(a, b) ((a) != (b))
#define FIXED_LT(a, b) ((a) < (b))
#define FIXED_LE(a, b) ((a) <= (b))
#define FIXED_GT(a, b) ((a) > (b))
#define FIXED_GE(a, b) ((a) >= (b))

// Sign function
#define FIXED_SIGN(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

// Power of 2 operations (optimized)
#define FIXED_MUL_2(x) ((x) << 1)
#define FIXED_DIV_2(x) ((x) >> 1)
#define FIXED_MUL_4(x) ((x) << 2)
#define FIXED_DIV_4(x) ((x) >> 2)
#define FIXED_MUL_8(x) ((x) << 3)
#define FIXED_DIV_8(x) ((x) >> 3)
#define FIXED_MUL_16(x) ((x) << 4)
#define FIXED_DIV_16(x) ((x) >> 4)

// Common fixed-point constants
#define FIXED_HALF (FIXED_ONE >> 1)
#define FIXED_QUARTER (FIXED_ONE >> 2)
#define FIXED_EIGHTH (FIXED_ONE >> 3)
#define FIXED_SIXTEENTH (FIXED_ONE >> 4)

// Zero and one
#define FIXED_ZERO 0
#define FIXED_NEG_ONE (-FIXED_ONE)

// Epsilon for comparisons
#define FIXED_EPSILON fix16_eps

// Overflow detection (if not using FIXMATH_NO_OVERFLOW)
#ifndef FIXMATH_NO_OVERFLOW
#define FIXED_IS_OVERFLOW(x) ((x) == fix16_overflow)
#else
#define FIXED_IS_OVERFLOW(x) (0) // Always false when overflow detection is disabled
#endif

#else
// Simple fabsf implementation
#define fabsf(x) ((x) < 0 ? -(x) : (x))


// WDC816cc implementation using float directly
typedef float fixed_t;
#define FIXED_ABS(x) fabsf(x)

// Basic arithmetic operations using float
#define FIXED_ADD(a, b) ((a) + (b))
#define FIXED_SUB(a, b) ((a) - (b))
#define FIXED_MUL(a, b) ((a) * (b))
#define FIXED_DIV(a, b) ((a) / (b))

// Conversion macros
#define FLOAT_TO_FIXED(x) (x)
#define INT_TO_FIXED(x) ((float)(x))
#define FIXED_TO_INT(x) ((int)(x))
#define FIXED_TO_FLOAT(x) (x)

// Utility macros
#define FIXED_MIN(a, b) fminf(a, b)
#define FIXED_MAX(a, b) fmaxf(a, b)
#define FIXED_CLAMP(x, lo, hi) fmaxf(lo, fminf(hi, x))
#define FIXED_FLOOR(x) floorf(x)
#define FIXED_CEIL(x) ceilf(x)

// Trigonometric functions
#define FIXED_SIN(x) sinf(x)
#define FIXED_COS(x) cosf(x)
#define FIXED_TAN(x) tanf(x)
#define FIXED_ASIN(x) asinf(x)
#define FIXED_ACOS(x) acosf(x)
#define FIXED_ATAN(x) atanf(x)
#define FIXED_ATAN2(y, x) atan2f(y, x)

// Mathematical functions
#define FIXED_SQRT(x) sqrtf(x)
#define FIXED_SQ(x) ((x) * (x))
#define FIXED_EXP(x) expf(x)
#define FIXED_LOG(x) logf(x)
#define FIXED_LOG2(x) log2f(x)

// Angle conversion
#define FIXED_RAD_TO_DEG(x) ((x) * 180.0f / 3.14159265359f)
#define FIXED_DEG_TO_RAD(x) ((x) * 3.14159265359f / 180.0f)

// Linear interpolation
#define FIXED_LERP8(a, b, t) ((a) + ((b) - (a)) * (t) / 255.0f)
#define FIXED_LERP16(a, b, t) ((a) + ((b) - (a)) * (t) / 65535.0f)
#define FIXED_LERP32(a, b, t) ((a) + ((b) - (a)) * (t) / 4294967295.0f)

// Saturated arithmetic (overflow protection)
#define FIXED_SADD(a, b) ((a) + (b))
#define FIXED_SSUB(a, b) ((a) - (b))
#define FIXED_SMUL(a, b) ((a) * (b))
#define FIXED_SDIV(a, b) ((a) / (b))

// Modulo operation
#define FIXED_MOD(x, y) fmodf(x, y)

// String conversion (if needed)
#define FIXED_TO_STR(x, buf, decimals) sprintf(buf, "%.*f", decimals, x)
#define FIXED_FROM_STR(buf) atof(buf)

// Comparison macros
#define FIXED_EQ(a, b) ((a) == (b))
#define FIXED_NE(a, b) ((a) != (b))
#define FIXED_LT(a, b) ((a) < (b))
//WDC816cc implementation, just use their float, it's good enough

#endif  // __WDC816CC__

#endif /* FIXEDPOINT_H */
