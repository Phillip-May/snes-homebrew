/*
* This is where the actual celeste code sits.
* It is mostly a line by line port of the original lua code.
* Due to C limitations, modifications have to be made, mostly relating to static typing.
* The PICO-8 functions such as music() are used here preceded by Celeste_P8,
* so _init becomes Celeste_P8_init && music becomes P8music, etc
*/

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include <inttypes.h>


#include "celesteSimple.h"


void function_0000(void) {
}
static int glovalVar;