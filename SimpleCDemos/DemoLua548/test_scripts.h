/*
** Embedded Lua test scripts for SNES demo
** These are hardcoded Lua scripts that demonstrate basic functionality
*/

#ifndef TEST_SCRIPTS_H
#define TEST_SCRIPTS_H

/* Test 1: Basic arithmetic and SNES functions */
static const char* test_arithmetic = 
    "result1 = snes.add(5, 3)\n"
    "result2 = snes.multiply(4, 6)\n"
    "sum = result1 + result2\n";

/* Test 2: String operations */
static const char* test_strings = 
    "message = 'Hello from Lua on SNES!'\n"
    "length = string.len(message)\n"
    "upper = string.upper(message)\n";

/* Test 3: Table operations */
static const char* test_tables = 
    "t = {x = 10, y = 20, name = 'SNES'}\n"
    "sum = t.x + t.y\n"
    "t.z = 30\n"
    "total = t.x + t.y + t.z\n";

/* Test 4: Function definition and call */
static const char* test_functions = 
    "function square(n)\n"
    "    return n * n\n"
    "end\n"
    "function fibonacci(n)\n"
    "    if n <= 1 then\n"
    "        return n\n"
    "    else\n"
    "        return fibonacci(n-1) + fibonacci(n-2)\n"
    "    end\n"
    "end\n"
    "result1 = square(4)\n"
    "result2 = fibonacci(6)\n";

/* Test 5: Control structures */
static const char* test_control = 
    "sum = 0\n"
    "for i = 1, 10 do\n"
    "    sum = sum + i\n"
    "end\n"
    "if sum > 50 then\n"
    "    message = 'Sum is greater than 50'\n"
    "else\n"
    "    message = 'Sum is 50 or less'\n"
    "end\n";

/* Test 6: Math library */
static const char* test_math = 
    "pi = math.pi\n"
    "sqrt_val = math.sqrt(16)\n"
    "sin_val = math.sin(math.pi / 2)\n"
    "floor_val = math.floor(3.7)\n"
    "ceil_val = math.ceil(3.2)\n";

#endif
