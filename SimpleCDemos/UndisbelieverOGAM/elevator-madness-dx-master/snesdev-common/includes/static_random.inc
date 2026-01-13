;; static-random.inc
;;
;; Builds random numbers using the Multiply with Carry method.
;;

.ifndef ::_STATIC_RANDOM_INC_
::_STATIC_RANDOM_INC_ = 1

__STATIC_RANDOM_W .set .time
__STATIC_RANDOM_Z .set .time

.if __STATIC_RANDOM_W = $464FFFFF
	__STATIC_RANDOM_W .set __STATIC_RANDOM_W + 1
.endif
.if __STATIC_RANDOM_Z = $9068FFFF
	__STATIC_RANDOM_Z .set __STATIC_RANDOM_Z + 1
.endif

;; Increments the random number generator.
;; if `value` is set then the assembler variable will be set to the random seed.
.macro STATIC_RANDOM value
	; ::SOURCE wikipedia::
	; ::: https://en.wikipedia.org/wiki/Random_number_generation ::

	__STATIC_RANDOM_Z .set 36969 * (__STATIC_RANDOM_Z & 65535) + (__STATIC_RANDOM_Z >> 16);
	__STATIC_RANDOM_W .set 18000 * (__STATIC_RANDOM_W & 65535) + (__STATIC_RANDOM_W >> 16);

	.ifnblank value
		value .set (__STATIC_RANDOM_Z << 16) + __STATIC_RANDOM_W
	.endif
.endmacro

;; Sets the assembler variable to a random number between min and max.
;;
;; Due to the way the ca65 handles arithmetic using sint32 values {abs(min - max)} MUST BE < (2^31-1).
.macro STATIC_RANDOM_MIN_MAX value, min, max
	.local r
	STATIC_RANDOM r
	value .set (r & $7FFFFFFF) .mod (min - max + 1) + min
.endmacro

.endif

; vim: ft=asm:

