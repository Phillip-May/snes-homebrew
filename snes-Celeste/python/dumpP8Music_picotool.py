"""
dumpP8Music_picotool.py

Extracts and parses the __sfx__ and __music__ sections from a PICO-8 .p8.png cartridge using picotool.

Outputs:
 - sfx.json
 - music.json
 - sfx.txt
 - music.txt
 - sfx_<i>.hex (raw hex dump per sfx)

Uses picotool library for reliable PICO-8 cartridge parsing.
"""

import json
import re
from pathlib import Path
from pico8.game.file import from_file

def get_note_description(pitch: int) -> str:
    """Convert PICO-8 pitch value to musical description."""
    if pitch == 0:
        return "Rest"
    
    # PICO-8 pitch values are 0-63, where 0-63 are notes
    # Pitch 0 = C0, Pitch 1 = C#0, ..., Pitch 63 = E5
    notes = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
    
    if pitch < 0 or pitch > 63:
        return f"Pitch{pitch}"
    
    # Calculate octave and note within octave
    # Pitch 0 = C0, so octave = pitch // 12
    octave = pitch // 12
    note_name = notes[pitch % 12]
    return f"{note_name}{octave}"

def get_instrument_description(instrument: int) -> str:
    """Convert PICO-8 instrument number to description."""
    instruments = {
        0: "Triangle",
        1: "Tilted Saw",
        2: "Saw",
        3: "Square",
        4: "Pulse",
        5: "Organ",
        6: "Noise",
        7: "Phaser",
        8: "Sine",
        9: "Beep",
        10: "Blip",
        11: "Warm",
        12: "Brass",
        13: "Funky",
        14: "Spooky",
        15: "Squelch"
    }
    return instruments.get(instrument, f"Unknown({instrument})")

def get_effect_description(effect: int) -> str:
    """Convert PICO-8 effect number to description."""
    effects = {
        0: "None",
        1: "Slide",
        2: "Vibrato",
        3: "Drop",
        4: "Fade In",
        5: "Fade Out",
        6: "Arp Fast",
        7: "Arp Slow"
    }
    return effects.get(effect, f"Unknown({effect})")

def pitch_to_14bit(pitch: int) -> int:
    """Convert PICO-8 pitch (0-63) to 14-bit DSP pitch value."""
    if pitch == 0:
        return 0  # Rest/silence
    
    # PICO-8 pitch 1 = C0, pitch 63 = E5
    # Convert to frequency in Hz using equal temperament tuning
    # Use 1000 Hz as reference (0x1000 = 4096 decimal)
    frequency = 1000.0 * (2 ** ((pitch - 33) / 12))  # 1000Hz reference at pitch 33
    
    # Convert to 14-bit DSP pitch
    # If 1000 Hz = 0x1000 (4096), then: value = frequency * 4.096
    value = int(frequency * 4.096)
    
    # Clamp to 14-bit range (0-16383)
    return max(0, min(16383, value))

def get_instrument_c_code(instrument: int) -> str:
    """Convert PICO-8 instrument to C code comment."""
    instruments = {
        0: "Triangle",
        1: "Tilted Saw", 
        2: "Saw",
        3: "Square",
        4: "Pulse",
        5: "Organ",
        6: "Noise",
        7: "Phaser",
        8: "Sine",
        9: "Beep",
        10: "Blip",
        11: "Warm",
        12: "Brass",
        13: "Funky",
        14: "Spooky",
        15: "Squelch"
    }
    return instruments.get(instrument, f"Unknown({instrument})")

def parse_sfx_from_picotool(game):
    """
    Parse SFX data using picotool's built-in SFX access.
    """
    sfx_list = []
    
    # Get the raw SFX data - it's a single bytearray containing all 64 SFX
    sfx_raw_data = game.sfx.to_bytes()
    
    # Parse each SFX from the raw data
    for i in range(64):  # PICO-8 has 64 SFX slots
        try:
            # Each SFX is 68 bytes: 4 bytes global + 64 bytes notes (32 notes × 2 bytes)
            start_idx = i * 68
            end_idx = start_idx + 68
            
            if end_idx <= len(sfx_raw_data):
                sfx_data = sfx_raw_data[start_idx:end_idx]
                
                # Parse global parameters (last 4 bytes: 65-68)
                g_editor_mode = sfx_data[64]  # Byte 65: Editor mode and filter switches
                g_speed = sfx_data[65]        # Byte 66: Speed
                g_loop_start = sfx_data[66]   # Byte 67: Loop start
                g_loop_end = sfx_data[67]     # Byte 68: Loop end
                
                # Parse note data (first 64 bytes = 32 notes, 2 bytes each)
                notes = []
                for n in range(32):
                    note_idx = n * 2
                    if note_idx + 1 < 64:  # Only use the first 64 bytes for notes
                        note_byte1 = sfx_data[note_idx]
                        note_byte2 = sfx_data[note_idx + 1]
                        
                        # Extract pitch (lower 6 bits of first byte)
                        pitch = note_byte1 & 0x3F
                        
                        # Extract instrument (bits 6-7 of first byte + bit 0 of second byte)
                        instrument_low = (note_byte1 >> 6) & 0x03
                        instrument_high = note_byte2 & 0x01
                        instrument = (instrument_high << 2) | instrument_low
                        
                        # Extract volume (bits 1-3 of second byte)
                        volume = (note_byte2 >> 1) & 0x07
                        
                        # Extract effect (bits 4-6 of second byte)
                        effect = (note_byte2 >> 4) & 0x07
                        
                        # Check if custom instrument flag is set (bit 7 of second byte)
                        custom_instrument = (note_byte2 >> 7) & 0x01
                        
                        notes.append({
                            "pitch": pitch,
                            "instrument": instrument,
                            "volume": volume,
                            "effect": effect,
                            "custom_instrument": custom_instrument,
                        })
                
                # Convert to hex string for compatibility
                hex_string = sfx_data.hex()
                
                sfx_list.append({
                    "index": i,
                    "raw_hex": hex_string,
                    "globals": {
                        "editor_mode": g_editor_mode,
                        "speed": g_speed,
                        "loop_start": g_loop_start,
                        "loop_end": g_loop_end,
                    },
                    "notes": notes
                })
        except (IndexError, AttributeError):
            # Skip invalid or missing SFX entries
            continue
    
    return sfx_list

def parse_music_from_picotool(game):
    """
    Parse music data using picotool's built-in music access.
    """
    music_list = []
    
    # Get the raw music data - it's a single bytearray containing all 64 patterns
    music_raw_data = game.music.to_bytes()
    
    # Parse each music pattern from the raw data
    for i in range(64):  # PICO-8 has 64 music pattern slots
        try:
            # Each pattern is 4 bytes: flags + 3 SFX IDs (channel 4 is always silent)
            start_idx = i * 4
            end_idx = start_idx + 4
            
            if end_idx <= len(music_raw_data):
                music_data = music_raw_data[start_idx:end_idx]
                
                flags = music_data[0]
                
                # Extract loop start and end from flags byte
                loop_start = flags & 0x0F  # Lower 4 bits
                loop_end = (flags >> 4) & 0x0F  # Upper 4 bits
                
                # Extract SFX IDs for channels 1-3
                sfx_ids = [
                    music_data[1],  # Channel 1
                    music_data[2],  # Channel 2  
                    music_data[3],  # Channel 3
                    0               # Channel 4 is always silent
                ]
                
                music_list.append({
                    "index": i,
                    "flags": flags,
                    "loop_start": loop_start,
                    "loop_end": loop_end,
                    "pattern_bytes_hex": " ".join([f"{b:02x}" for b in music_data]),
                    "sfx_ids": sfx_ids,
                })
        except (IndexError, AttributeError):
            # Skip invalid or missing music entries
            continue
    
    return music_list

def dump_outputs(basepath: Path, sfx_list, music_list, write_split_hex=True):
    base = basepath.with_suffix('')
    sfx_json_path = base.with_name(base.name + "_sfx.json")
    music_json_path = base.with_name(base.name + "_music.json")
    sfx_txt_path = base.with_name(base.name + "_sfx.txt")
    music_txt_path = base.with_name(base.name + "_music.txt")

    sfx_json_path.write_text(json.dumps(sfx_list, indent=2))
    music_json_path.write_text(json.dumps(music_list, indent=2))

    with sfx_txt_path.open("w", encoding="utf-8") as f:
        f.write("PICO-8 Sound Effects (SFX) Data\n")
        f.write("=" * 50 + "\n\n")
        
        for s in sfx_list:
            f.write(f"SFX #{s['index']:02d}\n")
            f.write("-" * 20 + "\n")
            
            g = s['globals']
            f.write(f"Global Settings:\n")
            f.write(f"  Editor Mode: {g['editor_mode']:3d} (0x{g['editor_mode']:02x})\n")
            f.write(f"  Speed:       {g['speed']:3d} (0x{g['speed']:02x})\n")
            f.write(f"  Loop Start:  {g['loop_start']:3d} (0x{g['loop_start']:02x})\n")
            f.write(f"  Loop End:    {g['loop_end']:3d} (0x{g['loop_end']:02x})\n")
            
            # Count non-empty notes (pitch != 0 AND volume != 0)
            non_empty_notes = [n for n in s['notes'] if n['pitch'] != 0 and n['volume'] != 0]
            f.write(f"\nNotes: {len(non_empty_notes)}/32 used\n")
            
            if non_empty_notes:
                f.write("  Step | Pitch | Inst | Vol | Effect | Custom | Description\n")
                f.write("  -----|-------|------|-----|--------|--------|------------\n")
                
                for ni, note in enumerate(s['notes']):
                    if note['pitch'] != 0 and note['volume'] != 0:
                        # Convert pitch to frequency description
                        pitch_desc = get_note_description(note['pitch'])
                        inst_desc = get_instrument_description(note['instrument'])
                        effect_desc = get_effect_description(note['effect'])
                        custom_flag = "Yes" if note['custom_instrument'] else "No"
                        
                        f.write(f"  {ni:4d} | {note['pitch']:5d} | {note['instrument']:4d} | {note['volume']:3d} | {note['effect']:6d} | {custom_flag:6s} | {pitch_desc} ({inst_desc}) {effect_desc}\n")
            else:
                f.write("  (No notes defined)\n")
            
            f.write(f"\nRaw Data: {s['raw_hex']}\n")
            f.write("\n" + "=" * 50 + "\n\n")

    with music_txt_path.open("w", encoding="utf-8") as f:
        f.write("PICO-8 Music Patterns Data\n")
        f.write("=" * 50 + "\n\n")
        
        for m in music_list:
            f.write(f"Pattern #{m['index']:02d}\n")
            f.write("-" * 20 + "\n")
            
            f.write(f"Flags: 0x{m['flags']:02x} ({m['flags']:3d})\n")
            f.write(f"Loop:  {m.get('loop_start', 0):2d} to {m.get('loop_end', 0):2d}\n")
            f.write(f"Bytes: {m['pattern_bytes_hex']}\n")
            
            f.write(f"\nChannels:\n")
            for ch, sfx_id in enumerate(m['sfx_ids']):
                if sfx_id != 0:
                    f.write(f"  Channel {ch+1}: SFX #{sfx_id:02d}\n")
                else:
                    f.write(f"  Channel {ch+1}: (silent)\n")
            
            f.write("\n" + "=" * 50 + "\n\n")

    if write_split_hex:
        outdir = base.with_name(base.name + "_sfx_hex")
        outdir.mkdir(exist_ok=True)
        for s in sfx_list:
            (outdir / f"sfx_{s['index']:02d}.hex").write_text(s['raw_hex'])

    print("Wrote:")
    print(" -", sfx_json_path)
    print(" -", music_json_path)
    print(" -", sfx_txt_path)
    print(" -", music_txt_path)
    if write_split_hex:
        print(" - per-sfx hex files in:", outdir)

def pitch_to_14bit(pitch: int) -> int:
    """Convert PICO-8 pitch (0-63) to 14-bit DSP pitch value with filter compensation"""
    if pitch == 0:
        return 0
    
    # Convert PICO-8 pitch to frequency
    # PICO-8 pitch 33 = A2 = 110 Hz (reference)
    frequency = 110.0 * (2 ** ((pitch - 33) / 12))
    
    # Apply filter compensation - boost higher frequencies
    # SNES has a 3rd order low-pass filter around 8-10 KHz
    # Boost frequencies above 500 Hz to compensate
    if frequency > 500.0:
        # Apply exponential boost for higher frequencies
        boost_factor = 1.0 + ((frequency - 500.0) / 1000.0) * 0.4  # Up to 40% boost
        frequency = frequency * boost_factor
    
    # Convert to 14-bit value (0x1000 = 1000 Hz in your system)
    value = int((frequency * 1.0) * 16.0)
    
    return max(0, min(0x3FFF, value))

def calculate_note_duration(pitch: int, speed: int = 0) -> int:
    """Calculate delay ticks based on note pitch for musical timing"""
    if pitch == 0:
        return 0
    
    # Convert PICO-8 pitch to frequency (same as pitch_to_14bit)
    frequency = 110.0 * (2 ** ((pitch - 33) / 12))
    
    # Calculate duration based on frequency
    # Higher frequency = shorter duration, lower frequency = longer duration
    # Base duration for 110 Hz = 64 ticks, but scale up by 16x to match pitch scaling
    base_frequency = 110.0
    base_duration = 64 * 16  # 1024 ticks for 110 Hz (scaled up)
    
    # Calculate duration: higher pitch = shorter duration
    duration = int(base_duration * (base_frequency / frequency))
    
    # Apply speed factor - PICO-8 speed affects playback speed
    # Speed 0 = normal, higher values = slower playback (longer delays)
    # In PICO-8, speed is a simple linear multiplier: speed 1 = 1x, speed 2 = 2x, speed 3 = 3x, etc.
    if speed > 0:
        # PICO-8 speed formula: speed is a direct multiplier
        speed_factor = float(speed)
        duration = int(duration * speed_factor)
    
    # Clamp to reasonable range (1-4096 ticks)
    return max(1, min(4096, duration))

def generate_c_code(basepath: Path, sfx_list, music_list):
    """Generate C code for playing back PICO-8 SFX data."""
    base = basepath.with_suffix('')
    c_path = base.with_name(base.name + "_playback.c")
    
    with c_path.open("w", encoding="utf-8") as f:
        f.write("/*\n")
        f.write(" * PICO-8 SFX Playback Code\n")
        f.write(" * Generated from PICO-8 cartridge data\n")
        f.write(" * \n")
        f.write(" * Usage: Call play_sfx_XX() to play a specific sound effect\n")
        f.write(" *        Call play_music_XX() to play a specific music pattern\n")
        f.write(" */\n\n")
        
        f.write("#include <stdint.h>\n")
        f.write("#include <stdbool.h>\n\n")
        
        # Generate individual SFX functions (only for non-empty SFX)
        f.write("// Individual SFX Playback Functions\n")
        for sfx in sfx_list:
            # Only process notes that have pitch and volume (skip rest notes)
            active_notes = [(i, note) for i, note in enumerate(sfx['notes']) if note['pitch'] != 0 and note['volume'] != 0]
            
            # Skip empty SFX
            if not active_notes:
                continue
                
            f.write(f"void play_sfx_{sfx['index']:02d}() {{\n")
            f.write(f"    // SFX {sfx['index']:02d} - {len(active_notes)} notes, Speed: {sfx['globals']['speed']}\n")
            
            for i, note in active_notes:
                pitch_14bit = pitch_to_14bit(note['pitch'])
                duration_ticks = calculate_note_duration(note['pitch'], sfx['globals']['speed'])
                note_desc = get_note_description(note['pitch'])
                inst_desc = get_instrument_c_code(note['instrument'])
                
                f.write(f"    // Step {i:2d}: {note_desc} ({inst_desc}) Vol:{note['volume']} Effect:{note['effect']}\n")
                f.write(f"    set_instrument({note['instrument']});\n")
                f.write(f"    set_volume({note['volume'] * 18}); // PICO-8 vol {note['volume']} -> 0-127 range\n")
                f.write(f"    start_note(0x{pitch_14bit:04X});\n")
                
                # Add effect handling
                if note['effect'] != 0:
                    f.write(f"    // Apply effect {note['effect']} ({get_effect_description(note['effect'])})\n")
                
                # Use T0 (2KHz) for delays > 255, T2 (64KHz) for shorter delays
                if duration_ticks > 255:
                    # Convert T2 ticks to T0 ticks: T0 runs at 2KHz, T2 at 64KHz
                    # So T0 ticks = T2 ticks / 32 (64KHz / 2KHz = 32)
                    t0_ticks = duration_ticks // 32
                    f.write(f"    delayTicksT0({t0_ticks}); // {t0_ticks * 500:.1f}μs at 2KHz (converted from {duration_ticks} T2 ticks)\n")
                else:
                    f.write(f"    delayTicksT2({duration_ticks}); // {duration_ticks * 15.625:.1f}μs at 64KHz\n")
                f.write(f"    stop_note();\n")
                f.write(f"    \n")
            
            f.write("}\n\n")
        
        # Generate individual music functions (only for non-empty music patterns)
        f.write("// Individual Music Pattern Functions\n")
        for music in music_list:
            # Check if any channels have SFX
            active_channels = [i for i, sfx_id in enumerate(music['sfx_ids']) if sfx_id != 0]
            
            # Skip empty music patterns
            if not active_channels:
                continue
                
            f.write(f"void play_music_{music['index']:02d}() {{\n")
            f.write(f"    // Music Pattern {music['index']:02d}\n")
            f.write(f"    // Playing SFX on channels: {', '.join([f'{ch+1}' for ch in active_channels])}\n")
            
            for ch, sfx_id in enumerate(music['sfx_ids']):
                if sfx_id != 0:
                    # Check if this SFX actually exists in our generated functions
                    sfx_exists = any(sfx['index'] == sfx_id for sfx in sfx_list)
                    if sfx_exists:
                        f.write(f"    play_sfx_{sfx_id:02d}(); // Channel {ch+1}: SFX {sfx_id:02d}\n")
            
            f.write("}\n\n")
        
        # Generate lookup functions
        f.write("// Lookup Functions\n")
        f.write("void play_sfx(uint8_t sfx_num) {\n")
        f.write("    switch (sfx_num) {\n")
        for sfx in sfx_list:
            # Only include non-empty SFX in the switch
            active_notes = [note for note in sfx['notes'] if note['pitch'] != 0 and note['volume'] != 0]
            if active_notes:
                f.write(f"        case {sfx['index']:2d}: play_sfx_{sfx['index']:02d}(); break;\n")
        f.write("        default: break; // Invalid or empty SFX\n")
        f.write("    }\n")
        f.write("}\n\n")
        
        f.write("void play_music(uint8_t pattern_num) {\n")
        f.write("    switch (pattern_num) {\n")
        for music in music_list:
            # Only include non-empty music patterns in the switch
            active_channels = [i for i, sfx_id in enumerate(music['sfx_ids']) if sfx_id != 0]
            if active_channels:
                f.write(f"        case {music['index']:2d}: play_music_{music['index']:02d}(); break;\n")
        f.write("        default: break; // Invalid or empty pattern\n")
        f.write("    }\n")
        f.write("}\n\n")
        
    
    print(" -", c_path)

def main():
    # Hardcode the input file to 15133.p8.png in the same directory as this script
    script_dir = Path(__file__).parent
    cart_path = script_dir / "15133.p8.png"
    
    if not cart_path.exists():
        print(f"Error: Input file '{cart_path}' not found!")
        return

    try:
        print(f"Loading PICO-8 cartridge: {cart_path}")
        
        # Load the cartridge using picotool
        game = from_file(str(cart_path))
        
        print("Extracting SFX and music data using picotool...")
        
        # Parse SFX data using picotool
        sfx_list = parse_sfx_from_picotool(game)
        print(f"Parsed {len(sfx_list)} SFX entries using picotool")
        
        # Parse music data using picotool
        music_list = parse_music_from_picotool(game)
        print(f"Parsed {len(music_list)} music patterns using picotool")
        
        # Also check if there are text-based blocks in the Lua code
        lua_code = str(game.lua)
        print(f"Extracted {len(lua_code)} characters of Lua code")
        
        # Look for text-based blocks in Lua section
        all_blocks = re.findall(r'__(\w+)__', lua_code)
        print(f"Found blocks in Lua section: {all_blocks}")
        
        # Try to extract text-based blocks as well (for compatibility)
        def extract_block(contents: str, block_name: str) -> str:
            pattern = re.compile(r"(^__" + re.escape(block_name) + r"__\r?\n)(.*?)(?:\r?\n__[\w]+__\r?\n|$)", re.S | re.M)
            m = pattern.search(contents)
            if not m:
                return ""
            return m.group(2).strip("\r\n")
        
        sfx_text_block = extract_block(lua_code, "sfx")
        music_text_block = extract_block(lua_code, "music")
        
        if sfx_text_block:
            print(f"Found text-based SFX block with {len(sfx_text_block)} characters")
            # Note: We could parse text-based blocks too, but picotool's binary parsing is more reliable
        
        if music_text_block:
            print(f"Found text-based music block with {len(music_text_block)} characters")
            # Note: We could parse text-based blocks too, but picotool's binary parsing is more reliable

        print(f"Total: {len(sfx_list)} SFX entries and {len(music_list)} music patterns")
        
        # Filter out empty SFX entries (notes with pitch != 0 AND volume != 0)
        non_empty_sfx = [sfx for sfx in sfx_list if any(note['pitch'] != 0 and note['volume'] != 0 for note in sfx['notes'])]
        non_empty_music = [music for music in music_list if any(sfx_id != 0 for sfx_id in music['sfx_ids'])]
        
        print(f"Non-empty: {len(non_empty_sfx)} SFX entries and {len(non_empty_music)} music patterns")
        
        dump_outputs(cart_path, non_empty_sfx, non_empty_music, write_split_hex=True)
        generate_c_code(cart_path, non_empty_sfx, non_empty_music)
        
    except Exception as e:
        print(f"Error processing file: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
