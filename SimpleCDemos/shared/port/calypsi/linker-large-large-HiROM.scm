(define memories
  '(
    ;; ROM in banks $00–$3F, mapped to CPU $8000-$FFFF (HiROM)
    (memory ROM
	  (scatter-to ROM-banks)
	  )
	(memory BANK0 (address (#x8000 . #xFFDF))
	  (section CODE_IN_BANK0)
      (section code)
	)	 
	(memory BANK1 (address (#xC10000 . #xC1FFFF))
      (section farcode cfar chuge ihuge huge switch data_init_table ifar)
	  (section ROM-banks)
	)	 
	(memory BANK2 (address (#xC20000 . #xC2FFFF))
      (section farcode cfar chuge ihuge huge switch data_init_table ifar)
	  (section ROM-banks)
	)	 
	(memory BANK3 (address (#xC30000 . #xC3FFFF))
      (section farcode cfar chuge ihuge huge switch data_init_table ifar)
	  (section ROM-banks)
	)	 
	(memory BANK4 (address (#xC40000 . #xC4FFFF))
      (section farcode cfar chuge ihuge huge switch data_init_table ifar)
	  (section ROM-banks)
	)	 
	(memory BANK5 (address (#xC50000 . #xC5FFFF))
      (section farcode cfar chuge ihuge huge switch data_init_table ifar)
	  (section ROM-banks)
	)
	(memory BANK6 (address (#xC60000 . #xC6FFFF))
      (section farcode cfar chuge ihuge huge switch data_init_table ifar)
	  (section ROM-banks)
	)	  
    (memory BANK7 (address (#xC70000 . #xC7FFFF))
      (section farcode cfar chuge ihuge huge switch data_init_table ifar)
	  (section ROM-banks)
	)
	;;(memory ROM (address (#x8000 . #xFFC0)) ;; LoROM bank 0
    ;;       (section code farcode cfar chuge switch data_init_table ifar))
	;;Bank1
    ;;(memory ROM (address (#x018000 . #x01FFFF)) ;;LoROM bank 1
    ;;       (section code farcode cfar chuge switch data_init_table ifar))

	(memory DirectPage (address (#x100 . #x01FF))
            (section (registers)))

    (memory STACK (address (#x0200 . #x1FFF))
           (section stack))
		   
    ;; WRAM: internal work RAM, 128KB at $7E:0000–$7E:FFFF
    (memory WRAM  (address (#x7e2000 . #x7effff))
           (section heap far zfar zhuge))

    ;; WRAM: internal work RAM, 64KB at $7F:0000–$7F:FFFF
    (memory WRAM2 (address (#x7f0000 . #x7fffff))
           (section BANKED_HEAP1))


    ;; Hardware Vectors at $FFe0–$FFFF
    (memory Vector (address (#xffe0 . #xffff))
           (section (reset #xfffc) ))

    ;; No heap block needed - using custom umm_malloc allocator
    ;; The heap will be managed entirely by umm_malloc as a static array
    (base-address _DirectPageStart DirectPage 0)
	(base-address _NearBaseAddress DirectPage 0)
    ))