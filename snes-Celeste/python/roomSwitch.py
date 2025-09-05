def generate_switch_statement(levels=32):
    print("switch (roomID) {")
    for i in range(1, levels + 1):
        print(f"    case {i}:")
        print(f"        GLOBAL_ActiveLevel.tilemapBg2 = (uint8_t *)tilemap_level{i}_bg2;")
        print(f"        GLOBAL_ActiveLevel.tilemapBg3 = (uint8_t *)tilemap_level{i}_bg3;")
        print(f"        GLOBAL_ActiveLevel.paletteBg = (uint8_t *)palette_level{i};")
        print(f"        GLOBAL_ActiveLevel.collisionFlags = (uint8_t *)collision_level{i};")
        print()
        print(f"        GLOBAL_ActiveLevel.scrollPointY = scroll_yPoint_level{i};")
        print(f"        GLOBAL_ActiveLevel.playerSpawnX = spawn_level{i}[0];")
        print(f"        GLOBAL_ActiveLevel.playerSpawnY = spawn_level{i}[1];")
        print(f"        break;")
    print("    default:")
    print("        break;")
    print("}")

generate_switch_statement()

#Strawberry table

import math

def P8sin(x):
    return math.sin(x * 2 * math.pi)

amplitude = 2.5 * 2
values = []

for off in range(40):
    y = P8sin(off / 40) * amplitude
    y_int8 = round(y)
    values.append(y_int8)

# Format as C int8_t array
print("int8_t y_positions[40] = {")
for i in range(0, 40, 10):
    print("    " + ", ".join(f"{v:3}" for v in values[i:i+10]) + ",")
print("};")

#Balloon string table
offset = 0.0
sprite_table = []
pattern_only = []
max_frames = 1000
min_match_len = 20
cycle_len = None

while True:
    sprite = 0x28 + (int(offset * 8) % 3) * 2
    sprite_table.append(sprite)
    pattern_only.append(sprite - 0x28)  # store just 0, 1, 2

    length = len(pattern_only)

    for cand_len in range(1, length // 2):
        if length >= 2 * cand_len and cand_len >= min_match_len:
            # Check if one cycle matches the next
            if pattern_only[-cand_len:] == pattern_only[-2 * cand_len:-cand_len]:
                cycle_len = cand_len
                break
    if cycle_len is not None or len(sprite_table) >= max_frames:
        break
    offset += 0.01

if cycle_len:
    final_table = sprite_table[-cycle_len:]
    print(f"Cycle detected! Length: {cycle_len} frames")
else:
    final_table = sprite_table
    print("No cycle found within frame cap.")

# Output minimal single-cycle table
print(f"\nuint8_t balloon_sprites[{len(final_table)}] = {{")
for i in range(0, len(final_table), 20):
    print("    " + ", ".join(f"{s}" for s in final_table[i:i+20]) + ",")
print("};")

#balloon y table
offset = 0.0
sprite_table = []
pattern_only = []
max_frames = 3000
min_match_len = 80
cycle_len = None
import math
scale = 2
frameCount = 0

while True:
    y = math.sin(offset) * 4
    sprite_table.append(int(y))
    pattern_only.append(int(y))  # store just 0, 1, 2

    #sprite = 0x28 + (int(offset * 8) % 3) * 2
    #sprite_table.append(sprite)
    #pattern_only.append(sprite - 0x28)  # store just 0, 1, 2

    length = len(pattern_only)

    for cand_len in range(1, length // 2):
        if length >= 2 * cand_len and cand_len >= min_match_len:
            # Check if one cycle matches the next
            if pattern_only[-cand_len:] == pattern_only[-2 * cand_len:-cand_len]:
                cycle_len = cand_len
                break
    if cycle_len is not None or len(sprite_table) >= max_frames:
        break
    offset += 0.01

if cycle_len:
    final_table = sprite_table[-cycle_len:]
    print(f"Cycle detected! Length: {cycle_len} frames")
else:
    final_table = sprite_table
    print("No cycle found within frame cap.")

# Output minimal single-cycle table
print(f"\nuint8_t balloon_ytable[{len(final_table)}] = {{")
for i in range(0, len(final_table), 20):
    print("    " + ", ".join(f"{s}" for s in final_table[i:i+20]) + ",")
print("};")

#Balloon y
offset = 0.0
sprite_table = []
pattern_only = []
max_frames = 3000
min_match_len = 80
cycle_len = None
import math
scale = 2
frameCount = 0

while True:
    y = math.sin(offset) * 4
    sprite_table.append(int(y))
    pattern_only.append(int(y))  # store just 0, 1, 2

    #sprite = 0x28 + (int(offset * 8) % 3) * 2
    #sprite_table.append(sprite)
    #pattern_only.append(sprite - 0x28)  # store just 0, 1, 2

    length = len(pattern_only)

    for cand_len in range(1, length // 2):
        if length >= 2 * cand_len and cand_len >= min_match_len:
            # Check if one cycle matches the next
            if pattern_only[-cand_len:] == pattern_only[-2 * cand_len:-cand_len]:
                cycle_len = cand_len
                break
    if cycle_len is not None or len(sprite_table) >= max_frames:
        break
    offset += 0.01

if cycle_len:
    final_table = sprite_table[-cycle_len:]
    print(f"Cycle detected! Length: {cycle_len} frames")
else:
    final_table = sprite_table
    print("No cycle found within frame cap.")

# Output minimal single-cycle table
print(f"\nuint8_t balloon_ytable[{len(final_table)}] = {{")
for i in range(0, len(final_table), 16):
    print("    " + ", ".join(f"{s}" for s in final_table[i:i+16]) + ",")
print("};")
