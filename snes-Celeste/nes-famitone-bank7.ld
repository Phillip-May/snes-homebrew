/* Redirect FamiTone2 code from fixed bank to bank 7 (.prg_rom_7 already defined by platform) */
SECTIONS {
  .prg_rom_7 : {
    *(.text.famitone_update)
    *(.text.famitone_channel_update)
    *(.text.famitone_set_instrument)
    *(.text.famitone_sfx_update)
    *(.text.famitone_sfx_clear_channel)
    *(.text.famitone_sfx_play)
    *(.text.neslib_sfx_play)
    *(.text.famitone_sfx_init)
    *(.text.famitone_init)
    *(.text.famitone_music_play)
    *(.text.famitone_music_stop)
    *(.text.famitone_music_pause)
    *(.text.famitone_sample_play)
    *(.text.famitone_sample_stop)
    *(.text.music_init)
    *(.rodata.famitone_note_table)
    *(.rodata.famitone_dummy_envelope)
    *(.rodata.sfx_priority)
  }
} INSERT BEFORE .dpcm
