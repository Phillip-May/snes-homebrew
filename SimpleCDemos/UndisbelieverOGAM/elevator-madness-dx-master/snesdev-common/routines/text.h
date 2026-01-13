;;
;; Generic Padded width Text
;; ========================
;;
;;

; Layout idea from: http://forums.nesdev.com/viewtopic.php?p=49694#p49694

.ifndef ::_TEXT_H_
::_TEXT_H_ = 1

.include "includes/import_export.inc"
.include "includes/config.inc"
.include "routines/block.h"

;; Number of text windows to allocate in memory.
CONFIG TEXT_N_WINDOWS, 4

.struct TextWindow 
	;; This word is appended to each character to convert it into a tileset
	;; It represents the ' ' character in a fixed tileset
	;; Bits 10-12 are also used to set the text color (palette).
	tilemapOffset		.word

	;; Current cursor position in the buffer
	bufferPos		.word

	;; Index of Text window starting byte
	;; (Ypos * 64 + XPos * 2)
	windowStart		.word

	;; Index of Text window ending byte
	;; (Ypos * 64 + XPos * 2)
	windowEnd		.word

	;; The number of tiles in a line
	lineTilesWidth		.byte

	;; Number of tiles left in the line
	tilesLeftInLine		.byte

	;; Window Flags
	flags			.byte

	;; Current Text Interface Address
	;; @see TextInterface
	textInterfaceAddr	.addr

	;; Current PrintString routine Address
	;;
	;; REQUIRE: 8 bit A, 16 bit Index
	;; INPUT: A = the character to print
	printStringAddr		.addr
.endstruct


.struct TextInterface
	;; Go to the next line
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; Preforms the following:
	;;    * Sets `Text__window::bufferPos`
	;;    * Resets `Text__window::charsLeftInLine`
	NewLine			.addr

	;; Prints a single character to the screen
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; Preforms the following:
	;;   * Displays the character to the buffer`
	;;   * Increments `Text__window::bufferPos`
	;;   * Decrements Text__window::tilesLeftInLine`
	;;   * Calls `Text__NewLine` if `Text__window::tilesLeftInLine` is 0
	;;   * Calls `Text__NewLine` if character if EOL
	;;
	;; This routine does not:
	;;   * Set `Text__updateBufferIfZero`
	;;   * Call `NewLine`
	PrintChar		.addr

	;; Called when the cursor has moved, either by `Text__SetCursor` or `Text__NewLine`
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	CursorMoved		.addr

	;; Returns the width of the word in `Text__stringPtr` and the number of spaces
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; RETURNS: A = the length of the word in `Text__stringPtr`.
	;;              0 if there is no word.
	;;          Y = the length of the word after spaces
	;;
	;; This routine does not modify `Text__stringPtr`
	;;
	;; You should modify this routine to implement special characters.
	GetWordLength		.addr

	;; Called when the character is unknown in `Text__PrintString`
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; INPUT: A = character to print
	;; RETURN: C set if character is special
	;;         Y = the pointer to a string to print
	;;             (bank = `TEXT::SPECIAL_STRING_BANK`)
	;;             0 if nothing to print.
	SpecialCharacter	.addr
.endstruct


IMPORT_MODULE Text
	ZEROPAGE
		;; The position of the string
		FARADDR	stringPtr
	ENDZEROPAGE

	;; The text buffer
	;; In $7E so shadow RAM can also be accessed.
	WORD	buffer, 32*32

	;; Word address of the tilemap in VRAM
	WORD vramMapAddr

	;; If zero, then update buffer to VRAM on VBlank
	BYTE updateBufferIfZero

	;; The Window settings
	STRUCT window, TextWindow

	;; The character to use when padding a string.
	BYTE paddingCharacter

	;; Constants
	;; =========

	;; Window Flags
	;; -------------
		;;; Has a border
		CONST WINDOW_BORDER, $01

		;;; Has no border
		CONST WINDOW_NO_BORDER, $00

	;; The last character to print
	CONST LAST_CHARACTER, 144

	;; conversion between ASCII and the tileset
	CONST ASCII_DELTA, ' '



	;; Printing Routines
	;; =================

	;; Prints a string to the buffer
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; MODIFIES: A, X, Y
	;;
	;; INPUT: A:X the location of the string
	ROUTINE PrintString

	;; Prints a single character onto the buffer
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; MODIFIES: A, X
	;;
	;; INPUT: A = the character to print
	ROUTINE PrintChar

	;; Numbers
	;; -------

	;; Prints 8 bit A as a Hex string
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: A = the character to print
	;; MODIFIES: A, X
	ROUTINE PrintHex_8A

	;; Prints 16 bit Y as a hex string
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: Y = the character to print
	;; MODIFIES: A, X
	ROUTINE PrintHex_16Y

	;; Prints an unsigned 8 bit A
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: A = the unsigned 8 bit value to print
	;; MODIFIES: A, X, Y
	;; CAVEATS: Uses SNES Division Register
	ROUTINE PrintDecimal_U8A

	;; Prints an signed 8 bit A
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: A = the signed 8 bit value to print
	;; MODIFIES: A, X, Y
	;; CAVEATS: Uses SNES Division Register
	ROUTINE PrintDecimal_S8A

	;; Prints an unsigned 16 bit Y
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: Y = the number to print
	;; MODIFIES: A, X, Y
	;; CAVEATS: Uses SNES Division Register
	ROUTINE PrintDecimal_U16Y

	;; Prints a signed 16 bit Y
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: Y = the number to print (signed)
	;; MODIFIES: A, X, Y
	;; CAVEATS: Uses SNES Division Register
	ROUTINE PrintDecimal_S16Y

	;; Prints an unsigned 32 bit XY
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: XY = the number to print (Y = low byte)
	;; MODIFIES: A, X, Y
	;;
	;; CAVEATS: Uses Math__DIVIDE
	ROUTINE PrintDecimal_U32XY

	;; Prints a signed 32 bit XY
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: XY = the number to print (Y = low byte)
	;; MODIFIES: A, X, Y
	;;
	;; CAVEATS: Uses Math__DIVIDE
	ROUTINE PrintDecimal_S32XY

	;; Padded Numbers
	;; --------------

	;; Prints an unsigned 8 bit A with a minimum of 1 digit
	;;
	;; `Text__paddingCharacter` will be used as the padding character.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: A = the number to print
	;; MODIFIES: A, X, Y
	;;
	;; CAVEATS: Uses SNES Division Registers
	ROUTINE PrintDecimalPadded_U8A_1

	;; Prints an unsigned 8 bit A with a minimum of 2 digits
	;;
	;; `Text__paddingCharacter` will be used as the padding character.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: A = the number to print
	;; MODIFIES: A, X, Y
	;;
	;; CAVEATS: Uses SNES Division Registers
	ROUTINE PrintDecimalPadded_U8A_2

	;; Prints an unsigned 8 bit A with a minimum of 3 digits
	;;
	;; `Text__paddingCharacter` will be used as the padding character.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: A = the number to print
	;; MODIFIES: A, X, Y
	;;
	;; CAVEATS: Uses SNES Division Registers
	ROUTINE PrintDecimalPadded_U8A_3

	;; Prints an unsigned 16 bit Y with a padding
	;;
	;; `Text__paddingCharacter` will be used as the padding character.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: Y = the number to print
	;;        A = minimum number of digits to display (must be > 8)
	;; MODIFIES: A, X, Y
	;;
	;; CAVEATS: Uses SNES Division Registers
	ROUTINE PrintDecimalPadded_U16Y

	;; Prints an signed 16 bit Y with a padding
	;;
	;; `Text__paddingCharacter` will be used as the padding character.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: Y = the number to print (signed)
	;;        A = minimum number of digits to display (must be > 8)
	;; MODIFIES: A, X, Y
	;;
	;; CAVEATS: Uses SNES Division Registers
	ROUTINE PrintDecimalPadded_S16Y

	;; Prints an unsigned 32 bit XY (with padding)
	;;
	;; `Text__paddingCharacter` will be used as the padding character.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: XY = the number to print (Y = low byte)
	;;         A = the minimum number of digits to print
	;; MODIFIES: A, X, Y
	;;
	;; CAVEATS: Uses Math__DIVIDE
	ROUTINE PrintDecimalPadded_U32XY

	;; Prints a signed 32 bit XY (with padding)
	;;
	;; `Text__paddingCharacter` will be used as the padding character.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;;
	;; INPUT: XY = the number to print (Y = low byte)
	;;         A = the minimum number of digits to print
	;; MODIFIES: A, X, Y
	;;
	;; CAVEATS: Uses Math__DIVIDE
	ROUTINE PrintDecimalPadded_S32XY

	;; Cursor Routines
	;; ===============

	;; Moves the cursor to the next line.
	;;
	;; If new line is outside the text boundary, Text__OutOfBounds is called.
	;;
	;; REQUIRES: DB in Shadow 
	;; MODIFIES: A, X
	ROUTINE NewLine

	;; Sets the cursor position
	;;
	;; RETURNS: 8 bit A, 16 bit X
	;; INPUT:
	;;	X = The window X Position
	;;	Y = The window Y Position
	;;
	;; MODIFIES: A, X, Y
	ROUTINE SetCursor

	;; Sets the color of the text
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; MODIFIES: A
	;;
	;; INPUT: A = the color of the text
	ROUTINE SetColor

	;; Windows
	;; =======

.if TEXT_N_WINDOWS > 1
	;; Selects a given TextWindow
	;;
	;; The current window settings will be saved to WRAM
	;; and the new window will loaded from WRAM into `Text__window`
	;;
	;; The save/load method was chosen because it simplifies the TextInterface.
	;; And I believe that most window switching would occur in Text Scenes
	;; and menus which will require a lot of processor time.
	;;
	;; I also thought about using DP to select Window, but it wastes precious
	;; Shadow RAM and would actually waste CPU time if more than 250 characters
	;; were printed.
	;;
	;; INPUT: A = Window Number. If > TEXT_N_WINDOWS, window 0 will be used.
	ROUTINE SelectWindow
.endif

	;; Sets up a window at a given location.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index.
	;;
	;; Your better off calling the Text_SetupWindow Macro
	;;
	;; INPUT:
	;;	A = Window Flags (see the constants above)
	;;	X = startYPos * 64 + startXPos * 2
	;;	Y = endYPos * 64 + endXPos * 2
	;;
	;; WARNING: If using a border X must be > 32*2+2 and Y must be < 32*32*2-66
	;;
	;; MODIFIES: A, X, Y
	ROUTINE SetupWindow

	;; Draws a border *outside* the Window area.
	;;
	;; Also clears the inside of the Window.
	;;
	;; The window area is defined by `windowStart` and `windowEnd`
	;;
	;; REQUIRES 8 bit A, 16 bit Index
	;; MODIFIES: A, X, Y
	ROUTINE DrawBorder

	;; Clears the inside of the Window.
	;;
	;; The window area is defined by `windowStart` and `windowEnd`
	;;
	;; REQUIRES 8 bit A, 16 bit Index
	;; MODIFIES: A, X, Y
	ROUTINE ClearWindow

	;; Removes the window (resets tiles to 0)
	;;
	;; REQUIRES 8 bit A, 16 bit Index
	;; MODIFIES: A, X, Y
	;; CAVEATS: may modify the window area, causing issues if called again
	ROUTINE RemoveWindow

	;; Resets the entire Text Buffer (to tile 0)
	;;
	;; REQUIRES 8 bit A, 16 bit Index
	ROUTINE ClearEntireBuffer

	;; Print String Methods
	;; ====================

	;; Prints the string contained in `Text__stringPtr` to the screen
	;; with no special characters and no word wrapping
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; INPUT: `Text__stringPtr` the string to print
	ROUTINE PrintStringBasic

	;; Prints the string contained in `Text__stringPtr` to the screen
	;; with word wrapping
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; INPUT: `Text__stringPtr` the string to print
	ROUTINE PrintStringWordWrapping


	;; Helpful Functions
	;; =================

	;; Converts the signed value in the 16 bit Y to a string stored in `decimalString`
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; INPUT: Y = the number to display (signed)
	;; OUTPUT: A - the bank of the string to print
	;;         X - the location of the string to print 
	ROUTINE ConvertDecimalString_S16Y

	;; Converts the unsigned value in the 16 bit Y to a string stored in `decimalString`
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; INPUT: Y = the number to display
	;; OUTPUT: A - the bank of the string to print
	;;         X - the location of the string to print 
	ROUTINE ConvertDecimalString_U16Y

	;; Converts the unsigned value in the 16 bit Y with padding to a string stored
	;; in `decimalString`
	;;
	;; `Text__paddingCharacter` will be used as the padding character.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; INPUT: Y = the number to display
	;;        A = minimum number of digits to display (must be < 8)
	;; OUTPUT: A - the bank of the string to print
	;;         X - the location of the string to print 
	ROUTINE ConvertDecimalStringPadded_U16Y

	;; Converts the signed value in the 16 bit Y with padding to a string stored
	;; in `decimalString`
	;;
	;; `Text__paddingCharacter` will be used as the padding character.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; INPUT: Y = the number to display (signed)
	;;        A = minimum number of digits to display (must be < 8)
	;; OUTPUT: A - the bank of the string to print
	;;         X - the location of the string to print 
	ROUTINE ConvertDecimalStringPadded_S16Y

	;; Converts the unsigned value in the 32 but XY to a string with padding.
	;;
	;; `Text__paddingCharacter` will be used as the padding character.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; INPUT: XY = the number to print (Y = low byte)
	;;	   A = the number of padding characters (if 0 then show entire string)
	;; OUTPUT: A - the bank of the string to print
	;;         X - the location of the string to print
	ROUTINE ConvertDecimalString_U32XY

ENDMODULE

.if TEXT_N_WINDOWS > 1
;; Selects the given window
;;
;; INPUT: win - the window number
.macro Text_SelectWindow win
	LDA	#win
	JSR	::Text__SelectWindow
.endmacro
.endif

;; Sets up a window with no border with a given dimensions.
;;
;; REQUIRES: 8 bit A, 16 bit Index
.macro Text_SetupWindow startXPos, startYPos, endXPos, endYPos, flags
	.if startXPos < 0 .or startXPos > 31
		.error "startXPos must be between 0 and 31"
	.endif
	.if endXPos < 0 .or endXPos > 31
		.error "endXPos must be between 0 and 31"
	.endif
	.if startYPos < 0 .or startYPos > 31
		.error "startYPos must be between 0 and 31"
	.endif
	.if endYPos < 0 .or endYPos > 31
		.error "endYpos must be between 0 and 31"
	.endif

	LDX	#startYPos * 64 + startXPos * 2
	LDY	#endYPos * 64 + endXPos * 2
	LDA	#flags
	JSR	::Text__SetupWindow
.endmacro

;; Sets the `tileOffset` and `TextInterface` for the selected Window.
;;
;; REQUIRES: 16 bit Index
;;
;; interface must be a TextInterface
.macro Text_SetInterface interface,  tileOffset
	LDX	#.loword(interface)
	STX	::Text__window + TextWindow::textInterfaceAddr

	LDX	#tileOffset
	STX	::Text__window + TextWindow::tilemapOffset
.endmacro

;; Sets the color of the text.
;;
;; REQUIRES: 8 bit A, 16 bit Index
.macro Text_SetColor color
	LDA	#color
	JSR	::Text__SetColor
.endmacro

;; Loads the font
;;
;; A Label called `$tileset_End` must be defined marking the end of the
;; tileset.
;;
;; Also sets the padding character to '0'.
;;
;; REQUIRES: 8 bit A, 16 bit X, DB in shadow, Force or V-Blank
.macro Text_LoadFont tileset, vramTilesetAddr, mapAddr
	LDA	#'0'
	STA	::Text__paddingCharacter

	LDX	#mapAddr
	STX	::Text__vramMapAddr

	TransferToVramLocation tileset, vramTilesetAddr
.endmacro


;; Updates the text buffer in VRAM
;;
;; REQUIRES: 8 bit A
.macro Text_VBlank
	LDA	::Text__updateBufferIfZero
	IF_ZERO
		LDA	#VMAIN_INCREMENT_HIGH | VMAIN_INCREMENT_1
		STA	VMAIN

		LDX	Text__vramMapAddr
		STX	VMADD

		LDX	#DMAP_DIRECTION_TO_PPU | DMAP_TRANSFER_2REGS | (.lobyte(VMDATA) << 8)
		STX	DMAP0			; also sets BBAD0

		LDX	#Text__buffer__size
		STX	DAS0

		LDX	#.loword(Text__buffer)
		STX	A1T0
		LDA	#.bankbyte(Text__buffer)
		STA	A1B0

		LDA	#MDMAEN_DMA0
		STA	MDMAEN

		STA	::Text__updateBufferIfZero
	ENDIF
.endmacro

;; Sets the printing mode to basic
;;
;; REQUIRES: 16 bit Index
;; Modifies: X
;;
;; mode must be a string routine
.macro Text_SetStringBasic
	LDX	#.loword(Text__PrintStringBasic)
	STX	::Text__window + TextWindow::printStringAddr
.endmacro

;; Sets the printing mode to word wrapping
;;
;; REQUIRES: 16 bit Index
;; Modifies: X
;;
;; mode must be a string routine
.macro Text_SetStringWordWrapping
	LDX	#.loword(Text__PrintStringWordWrapping)
	STX	::Text__window + TextWindow::printStringAddr
.endmacro



;; Prints a string
;;
;; REQUIRES: 8 bit A, 16 bit X, DB in shadow
.macro Text_PrintString param
	.if .match(param, "")
		.local check, skip, string

		check:
		.rodata
		string:
			.byte param, 0
		.code
			.assert check = skip, lderror, "Bad flow in Text_PrintString"
		skip:
			LDX	#.loword(string)
			LDA	#.bankbyte(string)

			JSR	::Text__PrintString
	.else
		LDX	#.loword(param)
		LDA	#.bankbyte(param)

		JSR	::Text__PrintString
	.endif
.endmacro

;; Prints a string, followed by a new line
;;
;; REQUIRES: 8 bit A, 16 bit X, DB in shadow
.macro Text_PrintStringLn param
	.if .match(param, "")
		.local check, skip, string

		check:
		.rodata
		string:
			.byte param, EOL, 0
		.code
			.assert check = skip, lderror, "Bad flow in Text_PrintStringLn"
		skip:
			LDX	#.loword(string)
			LDA	#.bankbyte(string)

			JSR	::Text__PrintString
	.else
		LDX	#.loword(param)
		LDA	#.bankbyte(param)

		JSR	::Text__PrintString
		JSR	::Text__NewLine
	.endif
.endmacro

;; Prints a character
;;
;; REQUIRES: 8 bit A, 16 bit Index
.macro Text_PrintChar c
	LDA	#c
	JSR	::Text__PrintChar
.endmacro

;; Prints a new line
;;
;; REQUIRES: DB in Shadow 
;; MODIFIES: A, X
.macro Text_NewLine
	JSR	::Text__NewLine
.endmacro

;; Prints a hexadecimal value for a given variable.
;;
;; The routine that will be called will depend on the value of `$var__type`.
;;
;; REQUIRES: 8 bit A, 16 bit Index
;;
;; @see includes/import_export.inc
.macro Text_PrintHex var
	_Text_PrintHex_Helper var, .ident(.sprintf("%s__type", .string(var))) 
.endmacro

.macro _Text_PrintHex_Helper var, type
	.if type = ::TYPE_BYTE .or type = ::TYPE_UINT8 .or type = ::TYPE_SINT8
		LDA	var
		JSR	::Text__PrintHex_8A
	.elseif type = ::TYPE_WORD .or type = ::TYPE_ADDR .or type = ::TYPE_UINT16 .or type = ::TYPE_SINT16
		LDY	var
		JSR	::Text__PrintHex_16Y
	.elseif type = ::TYPE_FARADDR
		LDY	var + 2
		JSR	::Text__PrintHex_8A
		LDY	var
		JSR	::Text__PrintHex_16Y
	.elseif type = ::TYPE_DWORD .or type = ::TYPE_UINT32 .or type = ::TYPE_SINT32
		LDY	var + 2
		JSR	::Text__PrintHex_16Y
		LDY	var
		JSR	::Text__PrintHex_16Y
	.else
		.error .sprintf("variable %s type (%d) unsupported", .string(var), type)
	.endif
.endmacro

;; Prints a decimal value for a given variable.
;;
;; The routine that will be called will depend on the value of `$var__type`.
;;
;; REQUIRES: 8 bit A, 16 bit Index
;;
;; INPUT: var - the variable to print
;;	  padding - optional - padding to use (use const)
;;
;; @see includes/import_export.inc
.macro Text_PrintDecimal var, padding
	_Text_PrintDecimal_Helper var, .ident(.sprintf("%s__type", .string(var))), padding
.endmacro

.macro _Text_PrintDecimal_Helper var, type, padding
	.if type = ::TYPE_BYTE .or type = ::TYPE_UINT8
		LDA	var
		.ifblank padding
			JSR	::Text__PrintDecimal_U8A
		.else
			.if padding = 1
				JSR	::Text__PrintDecimalPadded_U8A_1
			.elseif padding = 2
				JSR	::Text__PrintDecimalPadded_U8A_2
			.elseif padding = 3
				JSR	::Text__PrintDecimalPadded_U8A_3
			.else
				.error "Unknown padding (only allow 1, 2, or 3)"
			.endif
		.endif
	.elseif type = ::TYPE_SINT8
		LDA	var
		.ifblank padding
			JSR	::Text__PrintDecimal_S8A
		.else
			.error "Padding not supported"
		.endif
	.elseif type = ::TYPE_WORD .or type = ::TYPE_ADDR .or type = ::TYPE_UINT16
		LDY	var
		.ifblank padding
			JSR	::Text__PrintDecimal_U16Y
		.else
			LDA	#padding
			JSR	::Text__PrintDecimalPadded_U16Y
		.endif
	.elseif type = ::TYPE_SINT16
		LDY	var
		.ifblank padding
			JSR	::Text__PrintDecimal_S16Y
		.else
			.error "Padding not supported"
		.endif
	.elseif type = ::TYPE_UINT32 .or type = ::TYPE_DWORD
		LDXY	var
		.ifblank padding
			JSR	::Text__PrintDecimal_U32XY
		.else
			LDA	#padding
			JSR	::Text__PrintDecimalPadded_U32XY
		.endif	
	.elseif type = ::TYPE_SINT32
		LDXY	var
		.ifblank padding
			JSR	::Text__PrintDecimal_S32XY
		.else
			LDA	#padding
			JSR	::Text__PrintDecimalPadded_S32XY
		.endif	
	.else
		.error .sprintf("variable %s type (%d) not supported", .string(var), type)
	.endif
.endmacro


;; Sets the cursor to a given xpos and ypos.
;;
;; REQUIRE: 8 bit A, 16 bit Index
.macro Text_SetCursor xpos, ypos
	LDX	#xpos
	LDY	#ypos
	JSR	::Text__SetCursor
.endmacro

;; End of line character
.define EOL 13


; Extended characters
.define TEXT_DOT		127
.define UP_ARROW		128
.define DOWN_ARROW		129
.define LEFT_ARROW		130
.define RIGHT_ARROW		131
.define SELECT_CHARACTER	132
.define SELECT_CHARACTER_SMALL	133
.define MORE_CHARACTER		134
.define MORE_CHARACTER_SMALL	135
.define BORDER_TOP_LEFT		136
.define BORDER_TOP		137
.define BORDER_TOP_RIGHT	138
.define BORDER_LEFT		139
.define BORDER_RIGHT		140
.define BORDER_BOTTOM_LEFT	141
.define BORDER_BOTTOM		142
.define BORDER_BOTTOM_RIGHT	143

;; Empty filled square
.define TEXT_CLEAR		' '
.define TEXT_INVALID		TEXT_DOT


.endif ; ::_TEXT_H_

; vim: ft=asm:

