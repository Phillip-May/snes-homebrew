(define memories
  '(
    ;; ROM in banks $00–$3F, mapped to CPU $8000-$FFFF (LoROM)
    (memory ROM
	  (scatter-to ROM-banks)
	  )
	(memory BANK0 (address (#x8000 . #xFF00))
	  (section CODE_IN_BANK0)
      (section code)
	  (section ROM-banks)
	)	 
	(memory BANK1 (address (#x018000 . #x01FFFF))
      (section farcode cfar chuge switch data_init_table ifar)
	  (section ROM-banks)
	)	 
	(memory BANK2 (address (#x028000 . #x02FFFF))
      (section farcode cfar chuge switch data_init_table ifar)
	  (section ROM-banks)
	)	 
	(memory BANK3 (address (#x038000 . #x03FFFF))
      (section farcode cfar chuge switch data_init_table ifar)
	  (section ROM-banks)
	)	 
	(memory BANK4 (address (#x048000 . #x04FFFF))
      (section farcode cfar chuge switch data_init_table ifar)
	  (section ROM-banks)
	)	 
	(memory BANK5 (address (#x058000 . #x05FFFF))
      (section farcode cfar chuge switch data_init_table ifar)
	  (section ROM-banks)
	)
	(memory BANK6 (address (#x068000 . #x06FFFF))
      (section farcode cfar chuge switch data_init_table ifar)
	  (section ROM-banks)
	)	  
    (memory BANK7 (address (#x078000 . #x07FFFF))
      (section farcode cfar chuge switch data_init_table ifar)
	  (section ROM-banks)
	)
	;;(memory ROM (address (#x8000 . #xFFC0)) ;; LoROM bank 0
    ;;       (section code farcode cfar chuge switch data_init_table ifar))
	;;Bank1
    ;;(memory ROM (address (#x018000 . #x01FFFF)) ;;LoROM bank 1
    ;;       (section code farcode cfar chuge switch data_init_table ifar))

	(memory zeroPage (address (#x00 . #xff))
            (section (ztiny registers)))
	(memory DirectPage (address (#x0100 . #x1FFF))
            (section (registers)))

    (memory STACK (address (#x0100 . #x1FFF))
           (section stack))
		   
    ;; WRAM: internal work RAM, 128KB at $7E:0000–$7F:FFFF
    (memory WRAM (address (#x7e2000 . #x7fffff))
           (section heap far zfar))

    ;; Hardware Vectors at $FFe0–$FFFF
    (memory Vector (address (#xffe0 . #xffff))
           (section (reset #xfffc) ))

    ;; Special block allocations
    (block heap  (size #x2000)) ;; 8KB heap in WRAM
    (base-address _DirectPageStart DirectPage 0)
	(base-address _NearBaseAddress DirectPage 0)
    ))