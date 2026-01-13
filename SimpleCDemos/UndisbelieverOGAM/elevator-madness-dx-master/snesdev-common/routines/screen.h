;;
;; Screen control macros
;; =====================
;;
;;

.ifndef ::_SCREEN_H_
::_SCREEN_H_ = 1

.include "includes/import_export.inc"

IMPORT_MODULE Screen

	;; Incremented every VBlank, used as a frame counter
	UINT16	frameCounter

	;; Pauses execution for one frame
	;;
	;; REQUIRES: VBlank enabled
	ROUTINE	WaitFrame

	;; Pauses execution for a given number of frames
	;;
	;; REQUIRES: VBlank enabled
	;;
	;; INPUT: A - number of frames to wait
	ROUTINE	WaitManyFrames

	;; Starts at Screen brightness 0 and fades in to full brightness
	;;
	;; REQUIRES: 8 bit A, VBlank Enabled
	ROUTINE	FadeIn

	;; Starts at Screen brightness 0 and fades in to full brightness
	;;
	;; REQUIRES: 8 bit A, VBlank Enabled
	;;
	;; INPUT: A = number of frames per brightness increment (0 = 256 frames)
	ROUTINE	SlowFadeIn

	;; Starts at full brightness and fades the screen out to Force Blank
	;;
	;; REQUIRES: 8 bit A, VBlank Enabled
	ROUTINE	FadeOut

	;; Starts at full brightness and slowly fades the screen out to Force Blank
	;;
	;; REQUIRES: 8 bit A, VBlank Enabled
	;;
	;; INPUT: A = number of frames per decrement (0 = 256 frames)
	ROUTINE	SlowFadeOut

	;; Increments `frameCounter`, signifying a new frame.
	;;
	;; Must be called once during v-blank
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; MODIFIES: Y
	.macro Screen_VBlank
		.export _Screen_VBlank__Called = 1

		LDY	Screen__frameCounter
		INY
		STY	Screen__frameCounter
	.endmacro


	;; Sets the VRAM size and position registers.
	;;
	;; The sizes are taken from the variables BGx_MAP (word address in VRAM), BGx_SIZE (matches the values BGXSC_SIZE_*),
	;; BGx_TILES (word address in VRAM), OAM_TILES (word address in VRAM), OAM_NAME (matches the values OBSEL_NAME_*)
	;; and OAM_SIZE (matches the values OBSEL_SIZE_*). Where BGx represents optional BG1 - BG4.
	;;
	;; Alternatively a *prefix* may be supplied, in which the variables used are
	;; <prefix>_BGx_MAP, <prefix>_BGx_SIZE, <prefix>_BGx_TILES, <prefix>_OAM_TILES, and <prefix>_OAM_SIZE.
	;; Where BGx represents the optional BG1 - BG4.
	;;
	;; If a BGx_MAP or BGx_TILES is missing then the register will not be set.
	;;
	;; As the `BG12NBA` and `BG34NBA` registers handle 2 layers, if one layer's tile address variable is missing its word address is 0.
	;;
	;; REQUIRES: 8 bit A, 16 bit Index
	;; MODIFIES: A, Y
	.macro Screen_SetVramBaseAndSize prefix

		.ifblank prefix
			_Screen_SetVramBaseAndSize_map BG1_MAP, BG1_SIZE, BG2_MAP, BG2_SIZE, BG1SC
			_Screen_SetVramBaseAndSize_map BG3_MAP, BG3_SIZE, BG4_MAP, BG4_SIZE, BG3SC
			_Screen_SetVramBaseAndSize_oam OAM_SIZE, OAM_NAME, OAM_TILES
			_Screen_SetVramBaseAndSize_tiles BG1_TILES, BG2_TILES, BG3_TILES, BG4_TILES
		.else
			_Screen_SetVramBaseAndSize_map .ident(.sprintf("%s_BG1_MAP", .string(prefix))), .ident(.sprintf("%s_BG1_SIZE", .string(prefix))), .ident(.sprintf("%s_BG2_MAP", .string(prefix))), .ident(.sprintf("%s_BG2_SIZE", .string(prefix))), BG1SC
			_Screen_SetVramBaseAndSize_map .ident(.sprintf("%s_BG3_MAP", .string(prefix))), .ident(.sprintf("%s_BG3_SIZE", .string(prefix))), .ident(.sprintf("%s_BG4_MAP", .string(prefix))), .ident(.sprintf("%s_BG4_SIZE", .string(prefix))), BG3SC
			_Screen_SetVramBaseAndSize_oam .ident(.sprintf("%s_OAM_SIZE", .string(prefix))), .ident(.sprintf("%s_OAM_NAME", .string(prefix))), .ident(.sprintf("%s_OAM_TILES", .string(prefix)))
			_Screen_SetVramBaseAndSize_tiles .ident(.sprintf("%s_BG1_TILES", .string(prefix))), .ident(.sprintf("%s_BG2_TILES", .string(prefix))), .ident(.sprintf("%s_BG3_TILES", .string(prefix))), .ident(.sprintf("%s_BG4_TILES", .string(prefix)))
		.endif
	.endmacro

		.macro _Screen_SetVramBaseAndSize_map mapWordAddress1, size1, mapWordAddress2, size2, register
			.ifdef mapWordAddress1
				.assert (mapWordAddress1 / BGXSC_BASE_WALIGN) * BGXSC_BASE_WALIGN = mapWordAddress1, error, "mapWordAddress1 does not align with BGXSC_BASE_WALIGN"
				.assert mapWordAddress1 < $8000, error, "mapWordAddress1 too large"

				.ifdef mapWordAddress2
					.assert (mapWordAddress2 / BGXSC_BASE_WALIGN) * BGXSC_BASE_WALIGN = mapWordAddress2, error, "mapWordAddress2 does not align with BGXSC_BASE_WALIGN"
					.assert mapWordAddress2 < $8000, error, "mapWordAddress2 too large"

					LDY	#((mapWordAddress1 / BGXSC_BASE_WALIGN) << BGXSC_BASE_SHIFT | (size1 & BGXSC_MAP_SIZE_MASK)) | ((mapWordAddress2 / BGXSC_BASE_WALIGN) << BGXSC_BASE_SHIFT | (size2 & BGXSC_MAP_SIZE_MASK)) << 8
					STY	register
				.else
					LDA	#(mapWordAddress1 / BGXSC_BASE_WALIGN) << BGXSC_BASE_SHIFT | (size1 & BGXSC_MAP_SIZE_MASK)
					STA	register
				.endif
			.else
				.ifdef mapWordAddress2
					.assert (mapWordAddress2 / BGXSC_BASE_WALIGN) * BGXSC_BASE_WALIGN = mapWordAddress2, error, "mapWordAddress2 does not align with BGXSC_BASE_WALIGN"
					.assert mapWordAddress2 < $8000, error, "mapWordAddress2 too large"

					LDA	#(mapWordAddress2 / BGXSC_BASE_WALIGN) << BGXSC_BASE_SHIFT | (size2 & BGXSC_MAP_SIZE_MASK)
					STA	register + 1
				.endif
			.endif
		.endmacro

		.macro _Screen_SetVramBaseAndSize_oam size, name, oamTileWordAddress
			.ifdef oamTileWordAddress
				.assert (oamTileWordAddress / OBSEL_BASE_WALIGN) * OBSEL_BASE_WALIGN = oamTileWordAddress, error, "oamTileWordAddress does not align with OBSEL_BASE_WALIGN"
				.assert oamTileWordAddress < $8000, error, "oamTileWordAddress too large"

				LDA	#(size & OBSEL_SIZE_MASK) | (name & OBSEL_NAME_MASK) | (oamTileWordAddress / OBSEL_BASE_WALIGN) & OBSEL_BASE_MASK
				STA	OBSEL
			.endif
		.endmacro

		.macro _Screen_SetVramBaseAndSize_tiles bg1, bg2, bg3, bg4
			.ifndef bg1
				bg1 = 0
			.endif
			.ifndef bg2
				bg2 = 0
			.endif
			.ifndef bg3
				bg3 = 0
			.endif
			.ifndef bg4
				bg4 = 0
			.endif

			.assert (bg1 / BG12NBA_BASE_WALIGN) * BG12NBA_BASE_WALIGN = bg1, error, "bg1 map word adddress does not align with BG12NBA_BASE_WALIGN"
			.assert bg1 < $8000, error, "bg1 map address too large"
			.assert (bg2 / BG12NBA_BASE_WALIGN) * BG12NBA_BASE_WALIGN = bg2, error, "bg2 map word adddress does not align with BG12NBA_BASE_WALIGN"
			.assert bg2 < $8000, error, "bg2 map address too large"
			.assert (bg3 / BG34NBA_BASE_WALIGN) * BG34NBA_BASE_WALIGN = bg3, error, "bg3 map word adddress does not align with BG34NBA_BASE_WALIGN"
			.assert bg3 < $8000, error, "bg3 map address too large"
			.assert (bg4 / BG34NBA_BASE_WALIGN) * BG34NBA_BASE_WALIGN = bg4, error, "bg4 map word adddress does not align with BG34NBA_BASE_WALIGN"
			.assert bg4 < $8000, error, "bg4 map address too large"

			LDY	#(((bg2 / BG12NBA_BASE_WALIGN) << BG12NBA_BG2_SHIFT) & BG12NBA_BG2_MASK) | ((bg1 / BG12NBA_BASE_WALIGN) & BG12NBA_BG1_MASK) | ((((bg4 / BG12NBA_BASE_WALIGN) << BG34NBA_BG4_SHIFT) & BG34NBA_BG4_MASK) | ((bg3 / BG34NBA_BASE_WALIGN) & BG34NBA_BG3_MASK)) << 8
			STY	BG12NBA
		.endmacro
ENDMODULE


.endif ; ::_SCREEN_H_

; vim: set ft=asm:

