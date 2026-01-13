
Register Passing
================
    * 32 bit variables passed via XY, With Y being loword and X being hiword
    * 16 bit variables passed via Y

It is recommended to append the routine's name with the variable being referenced. For example, the following are used within `Text.s` to specify the variable used:

_U8A
: unsigned 8 bit A

_U16Y
: unsigned 16 bit Y

_S16Y
: signed 16 bit Y

_U32XY
: unsigned 32 bit X/Y, Index = 16 bit, Y is the lo-word, X is the high-word.

_16X
: 16 Bit X register


Naming
======
The following naming conventons are used.

    * Routines are CapitalizedCamelCase
        * Labels starting with `_` are private
        * Labels within a routine should start with routine name, then `_`, then name
          ie, `_ClearWindow_skip_php`
    * Variables are lowerCamerCase
    * Defines are in ALL_CAPS
        * Internal macros start with `_`
    * Constants are in ALL_CAPS


Coding Conventions
==================
    * opcodes should be in ALLCAPS
    * use tabs top to separate opcode from parameters
    * use tabbed indenting to show scope/structure
    * Have at least 3 empty lines between routines
    * Use `structure.inc` and `import_export.inc` macros to define variables and routines.
    * Routines should be prepending with `.A#` and `.I#` control statements to define register size for routine (if required).


Assertions
==========
    * Use `.assert` to ensure correct flow if routine falls into next routine.

       ~~~~ asm
       ROUTINE PrintDecimal_U32XY
           LDA	#0
           .assert * = PrintDecimalPadded_U32XY, lderror, "Bad Flow Control"

        ROUTINE PrintDecimalPadded_U32XY
        ~~~~

     * Use `.assert` when multiplying an Index by the size of a variable.

        ~~~~ asm
            .assert .sizeof(TextWindow) = 15, error, "Incorrect TextWindow Size"

            ; code ...
        ~~~~ 
 
