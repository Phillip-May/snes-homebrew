import json
import os
import base64
import zlib
import gzip
import struct
from PIL import Image


far_background_gids = [40,41,42,56,57,58, 16,103,104,88]
solid_gids = [32, 33, 34, 35, 36, 37, 38, 39, 48, 49, 50, 51, 52, 53, 54, 55, 66,67,68,69,82,83,84,85,98,99,100,101,114,115,116,117]
pointy_gids = [17,27, 43, 59]
icy_gids = [66,67,68,69,82,83,84,85,98,99,100,101,114,115,116,117]

arrMustBeObject = [8, 9, 10, 11, 12, 13, 14, 15, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 30, 31, 45,46,47, 60,62,64, 70,71,86,87, 96,97, 102, 118,119,120]


#This array is grabbed from convertBaseSpriteSheet.py
outputIndexTranslationTable = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0, 0, 0, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 0, 29, 0, 30, 0, 31, 32, 33, 34, 35, 0, 0, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 0, 0, 50, 51, 52, 53, 54, 55, 56, 57, 0, 0, 58, 59, 60, 61, 0, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 0, 0, 0, 77, 78, 79, 80, 81, 82, 83]
#From convertBaseSpriteSheet.py
all_2bpp_palettes = [[(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (95, 87, 79), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (194, 195, 199), (255, 241, 232), (95, 87, 79)], [(0, 0, 0), (194, 195, 199), (255, 241, 232), (95, 87, 79)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (95, 87, 79)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (95, 87, 79)], [(0, 0, 0), (41, 173, 255), (255, 241, 232), (0, 0, 0)], [(0, 0, 0), (41, 173, 255), (255, 241, 232), (95, 87, 79)], [(0, 0, 0), (41, 173, 255), (255, 241, 232), (0, 0, 0)], [(0, 0, 0), (41, 173, 255), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (41, 173, 255), (255, 241, 232), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (95, 87, 79)], [(0, 0, 0), (95, 87, 79), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (95, 87, 79), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (95, 87, 79), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (194, 195, 199), (255, 241, 232), (95, 87, 79)], [(0, 0, 0), (255, 241, 232), (0, 135, 81), (0, 228, 54)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (41, 173, 255), (255, 241, 232), (95, 87, 79)], [(0, 0, 0), (41, 173, 255), (255, 241, 232), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (95, 87, 79)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (95, 87, 79)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (95, 87, 79)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (95, 87, 79)], [(0, 0, 0), (95, 87, 79), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (95, 87, 79), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (95, 87, 79), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (194, 195, 199), (255, 241, 232), (95, 87, 79)], [(0, 0, 0), (0, 135, 81), (0, 228, 54), (0, 0, 0)], [(0, 0, 0), (0, 135, 81), (0, 228, 54), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (95, 87, 79)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (41, 173, 255), (255, 241, 232), (0, 0, 0)], [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (194, 195, 199), (131, 118, 156), (41, 173, 255)], [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (95, 87, 79)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (95, 87, 79)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (41, 173, 255), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (95, 87, 79), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (194, 195, 199), (131, 118, 156), (0, 0, 0)], [(0, 0, 0), (194, 195, 199), (131, 118, 156), (0, 0, 0)], [(0, 0, 0), (194, 195, 199), (41, 173, 255), (131, 118, 156)], [(0, 0, 0), (194, 195, 199), (131, 118, 156), (41, 173, 255)], [(0, 0, 0), (194, 195, 199), (131, 118, 156), (41, 173, 255)], [(0, 0, 0), (194, 195, 199), (131, 118, 156), (0, 0, 0)], [(0, 0, 0), (194, 195, 199), (131, 118, 156), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (95, 87, 79), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (95, 87, 79), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (131, 118, 156), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (131, 118, 156), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (131, 118, 156), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (131, 118, 156), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (131, 118, 156), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (131, 118, 156), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (131, 118, 156), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (255, 236, 39), (171, 82, 54), (255, 163, 0)], [(0, 0, 0), (255, 236, 39), (171, 82, 54), (255, 163, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (255, 241, 232), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (29, 43, 83), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (41, 173, 255), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0)], [(0, 0, 0), (29, 43, 83), (41, 173, 255), (0, 0, 0)], [(0, 0, 0), (29, 43, 83), (41, 173, 255), (0, 0, 0)]]


def decode_tiled_layer_data(layer):
    """Decodes layer data if it is base64 encoded and compressed."""
    if layer.get("encoding") == "base64" and isinstance(layer.get("data"), str):
        decoded_data = base64.b64decode(layer["data"])
        compression = layer.get("compression")

        if compression == "zlib":
            decompressed_data = zlib.decompress(decoded_data)
        elif compression == "gzip":
            decompressed_data = gzip.decompress(decoded_data)
        else:
            decompressed_data = decoded_data

        num_tiles = len(decompressed_data) // 4
        unpacked_data = struct.unpack(f"<{num_tiles}I", decompressed_data)
        layer["data"] = list(unpacked_data)


def visualize_map(tilemap_path, spritesheet_path, output_path, output_path_bg3):
    """
    Generates an image of the map from a Tiled JSON file and a spritesheet.
    """
    try:
        with open(tilemap_path, "r") as f:
            tilemap_data = json.load(f)

        spritesheet = Image.open(spritesheet_path)

        map_width_tiles_global = tilemap_data["width"]
        map_height_tiles_global = tilemap_data["height"]
        tile_width = tilemap_data["tilewidth"]
        tile_height = tilemap_data["tileheight"]

        map_image = Image.new(
            "RGBA",
            (map_width_tiles_global * tile_width, map_height_tiles_global * tile_height),
            (0, 0, 0, 255)  # Black background
        )
        bg3_image = Image.new(
            "RGBA",
            (map_width_tiles_global * tile_width, map_height_tiles_global * tile_height),
            (0, 0, 0, 0)  # Transparent background
        )

        for layer in tilemap_data["layers"]:
            if layer["type"] == "tilelayer":
                decode_tiled_layer_data(layer)
                xMaps = 8
                yMaps = 4
                tile_data_all = layer["data"]
                for mapNum in range(xMaps * yMaps):
                    layer_name = f"level{mapNum+1}"
                    map_width_tiles_local = 16
                    map_height_tiles_local = 16

                    topLeftX = (mapNum % xMaps) * map_width_tiles_local
                    topLeftY = (mapNum // xMaps) * map_height_tiles_local
                    #These values are tiles
                    submap_width = 16
                    submap_height = 16

                    # Ensure we don't try to access pixels outside the map
                    actual_submap_height = min(submap_height, map_height_tiles_local)
                    actual_submap_width = min(submap_width, map_width_tiles_local)

                    submap_data = [[0] * actual_submap_width for _ in range(actual_submap_height)]
                    tile_data = []
                    for i in range(actual_submap_height):
                        start_index = (topLeftY + i) * map_width_tiles_global + topLeftX
                        end_index = start_index + actual_submap_width
                        tile_data.extend(tile_data_all[start_index:end_index])

                    for i, tile_gid in enumerate(tile_data):
                        map_x_tile = (i % map_width_tiles_local)
                        map_y_tile = (i // map_width_tiles_local)

                        if map_x_tile < actual_submap_width and map_y_tile < actual_submap_height:
                            submap_data[map_y_tile][map_x_tile] = tile_gid

                        if tile_gid == 0:
                            continue

                        original_gid = tile_gid

                        # Tile GID transformation flags
                        FLIPPED_HORIZONTALLY_FLAG = 0x80000000
                        FLIPPED_VERTICALLY_FLAG = 0x40000000
                        FLIPPED_DIAGONALLY_FLAG = 0x20000000

                        flipped_horizontally = bool(tile_gid & FLIPPED_HORIZONTALLY_FLAG)
                        flipped_vertically = bool(tile_gid & FLIPPED_VERTICALLY_FLAG)
                        flipped_diagonally = bool(tile_gid & FLIPPED_DIAGONALLY_FLAG)

                        # Clear the flags to get the actual GID
                        tile_gid &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG)

                        tileset = find_tileset(tile_gid, tilemap_data["tilesets"])
                        if not tileset:
                            print(f"Warning: No tileset found for GID {tile_gid} (original: {original_gid})")
                            continue

                        first_gid = tileset["firstgid"]
                        local_tile_id = tile_gid - first_gid

                        #Debug
                        if outputIndexTranslationTable[local_tile_id] == 0:
                            local_tile_id = 0
                            #local_tile_id = tileIndexTranslationTable[local_tile_id]
                        else:
                            pass
                        spritesheet_cols = tileset.get("columns")
                        if not spritesheet_cols:
                            print(f"Warning: Tileset '{tileset.get('name')}' has no 'columns' attribute.")
                            continue

                        tile_x_in_spritesheet = (local_tile_id % spritesheet_cols) * tile_width
                        tile_y_in_spritesheet = (local_tile_id // spritesheet_cols) * tile_height

                        spritesheet_width, spritesheet_height = spritesheet.size
                        if tile_x_in_spritesheet + tile_width > spritesheet_width or \
                        tile_y_in_spritesheet + tile_height > spritesheet_height:
                            print(f"Warning: Tile coordinates out of spritesheet bounds for GID {tile_gid} (original: {original_gid}).")
                            print(f"  local_id: {local_tile_id}, coords: ({tile_x_in_spritesheet}, {tile_y_in_spritesheet}), spritesheet_size: {spritesheet.size}")
                            continue

                        tile_image = spritesheet.crop((
                            tile_x_in_spritesheet,
                            tile_y_in_spritesheet,
                            tile_x_in_spritesheet + tile_width,
                            tile_y_in_spritesheet + tile_height,
                        ))

                        # Apply transformations based on the flags
                        if flipped_diagonally:
                            if flipped_horizontally and flipped_vertically:
                                # rotate 90 degrees right and flip horizontally
                                tile_image = tile_image.transpose(Image.ROTATE_270)
                                tile_image = tile_image.transpose(Image.FLIP_LEFT_RIGHT)
                            elif flipped_horizontally:
                                # rotate 90 degrees right
                                tile_image = tile_image.transpose(Image.ROTATE_270)
                            elif flipped_vertically:
                                # rotate 270 degrees right
                                tile_image = tile_image.transpose(Image.ROTATE_90)
                            else:
                                # rotate 90 degrees right and flip vertically
                                tile_image = tile_image.transpose(Image.ROTATE_270)
                                tile_image = tile_image.transpose(Image.FLIP_TOP_BOTTOM)
                        else:
                            if flipped_horizontally:
                                tile_image = tile_image.transpose(Image.FLIP_LEFT_RIGHT)
                            if flipped_vertically:
                                tile_image = tile_image.transpose(Image.FLIP_TOP_BOTTOM)

                        map_x = (i % map_width_tiles_local) * tile_width
                        map_y = (i // map_width_tiles_local) * tile_height

                        tile_index = (tile_gid - 1) & 0x3FFF
                        if tile_index in far_background_gids:
                            bg3_image.paste(tile_image, (map_x, map_y), tile_image)
                        elif tile_gid not in arrMustBeObject:
                            if tile_gid == 64:
                                pass
                            map_image.paste(tile_image, (map_x, map_y), tile_image)
                        else:
                            pass

                    print(f"\nSubmap data for layer '{layer.get('name')}':")
                    # Convert GIDs to SNES tilemap data and write to C header file
                    snes_tilemap_data = []
                    snes_tilemap_data_bg3 = []
                    snes_tilemap_colour_data = []
                    snes_tilemap_collision_data = []
                    snes_tilemap_playerStartXY = [0, 0]
                    snes_tilemap_list_of_objects = []

                    curTileX = -1
                    curTileY = -1
                    for row in submap_data:
                        curTileX = -1
                        curTileY += 1
                        snes_row = []
                        snes_row_bg3 = []
                        for gid in row:
                            curTileX += 1
                            if gid == 0:
                                # Empty tile
                                snes_row.append(0)
                                snes_row_bg3.append(0)
                                snes_tilemap_collision_data.append(0)
                                continue

                            tile_index = (gid - 1) & 0x3FFF  # 14-bit tile index
                            if tile_index == 1:
                                snes_tilemap_playerStartXY[0] = curTileX
                                snes_tilemap_playerStartXY[1] = curTileY

                            if tile_index in arrMustBeObject:
                                snes_tilemap_list_of_objects.append((tile_index, curTileX, curTileY))

                            if tile_index in solid_gids:
                                snes_tilemap_collision_data.append(0x01)
                            elif tile_index in pointy_gids:
                                if  tile_index == 17:
                                    snes_tilemap_collision_data.append(0x04)
                                elif tile_index == 27:
                                    snes_tilemap_collision_data.append(0x08)
                                elif tile_index == 43:
                                    snes_tilemap_collision_data.append(0x10)
                                elif tile_index == 59:
                                    snes_tilemap_collision_data.append(0x20)
                            else:
                                snes_tilemap_collision_data.append(0)

                            flipped_horizontally = bool(gid & 0x4000)
                            flipped_vertically = bool(gid & 0x8000)

                            # Convert GID to SNES tile index (assuming 16x16 tiles)
                            # SNES tilemap format: VHPPCCCCCCCCCCCC
                            # V=Vertical flip, H=Horizontal flip, PP=Priority, CCCCCCCCCCCC=Tile index
                            tile_index = (gid - 1) & 0x3FFF  # 14-bit tile index

                            snes_tile_id = outputIndexTranslationTable[tile_index]
                            curPal = all_2bpp_palettes[snes_tile_id]

                            snes_tile_row = snes_tile_id % 8
                            snes_tile_col = snes_tile_id // 8
                            snes_tile_id = snes_tile_row * 2 + snes_tile_col * 16 * 2

                            if curPal == [(0, 0, 0), (95, 87, 79), (0, 0, 0), (0, 0, 0)]:
                                if tile_index not in far_background_gids:
                                    print("Found a new far background tile")
                                    print(tile_index)
                            # Check if palette is a subset of any existing palette (same position)
                            # Treat (0,0,0) as unused outside of slot 0
                            palette_added = False
                            matching_palette_index = -1
                            for i, existing_pal in enumerate(snes_tilemap_colour_data):
                                if len(curPal) <= len(existing_pal):
                                    is_subset = True
                                    for j, color in enumerate(curPal):
                                        # Skip unused colors (0,0,0) except in slot 0
                                        if j == 0 or color != (0, 0, 0):
                                            if j >= len(existing_pal) or color != existing_pal[j]:
                                                is_subset = False
                                                break
                                    if is_subset:
                                        palette_added = True
                                        matching_palette_index = i
                                        break

                            if not palette_added:
                                snes_tilemap_colour_data.append(curPal)
                                snes_pal_index = len(snes_tilemap_colour_data) - 1
                            else:
                                snes_pal_index = matching_palette_index

                            # Apply flip flags to SNES tilemap entry
                            snes_entry = snes_tile_id
                            if flipped_horizontally:
                                snes_entry |= 0x4000  # Set horizontal flip bit
                            if flipped_vertically:
                                snes_entry |= 0x8000  # Set vertical flip bit

                            # Encode palette index into the entry
                            snes_entry |= (snes_pal_index << 10)  # Set palette bits (bits 10-11)

                            if tile_index in far_background_gids:
                                snes_row.append(0)  # Treat as empty tile
                                snes_row_bg3.append(snes_entry)
                            else:
                                snes_row.append(snes_entry)
                                snes_row_bg3.append(0)

                        snes_tilemap_data.append(snes_row)
                        snes_tilemap_data_bg3.append(snes_row_bg3)
                        # Add 16 words of padding at the end of each row
                        for _ in range(16):
                            snes_row.append(0)
                            snes_row_bg3.append(0)

                    # Write SNES tilemap data to C header file
                    header_filename = f"tilemap_{layer_name}.h"
                    header_guard = f"TILEMAP_{layer_name.upper()}_H"

                    with open(header_filename, 'w') as f:
                        f.write(f"// SNES tilemap data for layer '{layer_name}'\n\n")
                        f.write(f"#ifndef {header_guard}\n")
                        f.write(f"#define {header_guard}\n\n")

                        f.write(f"// Tilemap dimensions: {len(snes_tilemap_data[0])}x{len(snes_tilemap_data)} tiles\n")
                        f.write(f"#define TILEMAP_{layer_name.upper()}_WIDTH {len(snes_tilemap_data[0])}\n")
                        f.write(f"#define TILEMAP_{layer_name.upper()}_HEIGHT {len(snes_tilemap_data)}\n\n")

                        f.write(f"// SNES tilemap data (16-bit entries)\n")
                        f.write(f"const unsigned short tilemap_{layer_name}_bg2[] = {{\n")

                        for row_idx, row in enumerate(snes_tilemap_data):
                            f.write(f"    // Row {row_idx}\n    ")
                            for col_idx, entry in enumerate(row):
                                f.write(f"0x{entry:04x}")
                                if col_idx < len(row) - 1:
                                    f.write(", ")
                            if row_idx < len(snes_tilemap_data) - 1:
                                f.write(",\n")
                            else:
                                f.write("\n")

                        f.write("};\n\n")

                        f.write(f"// SNES tilemap data (16-bit entries)\n")
                        f.write(f"const unsigned short tilemap_{layer_name}_bg3[] = {{\n")

                        for row_idx, row in enumerate(snes_tilemap_data_bg3):
                            f.write(f"    // Row {row_idx}\n    ")
                            for col_idx, entry in enumerate(row):
                                f.write(f"0x{entry:04x}")
                                if col_idx < len(row) - 1:
                                    f.write(", ")
                            if row_idx < len(snes_tilemap_data_bg3) - 1:
                                f.write(",\n")
                            else:
                                f.write("\n")

                        f.write("};\n\n")

                        # Write palette data
                        f.write(f"// Palette data for layer '{layer_name}'\n")
                        f.write(f"const unsigned short palette_{layer_name}[] = {{\n")

                        for palette_idx, palette in enumerate(snes_tilemap_colour_data):
                            f.write(f"    // Palette {palette_idx}\n    ")
                            for color_idx, color in enumerate(palette):
                                # Convert RGB to SNES 15-bit color format (BGR555)
                                r, g, b = color
                                snes_color = ((b >> 3) << 10) | ((g >> 3) << 5) | (r >> 3)
                                f.write(f"0x{snes_color:04x}")
                                if color_idx < len(palette) - 1:
                                    f.write(", ")
                            if palette_idx < len(snes_tilemap_colour_data) - 1:
                                f.write(",\n")
                            else:
                                f.write("\n")
                        f.write("};\n\n")
                        f.write(f"#define PALETTE_{layer_name.upper()}_COUNT {len(snes_tilemap_colour_data)}\n\n")

                        #write collision data
                        f.write(f"// Collision data for layer '{layer_name}'\n")
                        f.write(f"const unsigned char collision_{layer_name}[] = {{\n")

                        for curByte in snes_tilemap_collision_data:
                            f.write(f"{curByte}")
                            if curByte < len(snes_tilemap_collision_data) - 1:
                                f.write(", ")
                            if (curByte + 1) % 16 == 0:  # New line every 16 entries
                                f.write("\n    ")
                        f.write("\n};\n\n")
                        f.write(f"#define COLLISION_{layer_name.upper()}_COUNT {len(snes_tilemap_collision_data)}\n\n")

                        #write object data
                        if len(snes_tilemap_list_of_objects) == 0:
                            f.write(f"const unsigned char object_{layer_name}[];\n")
                            f.write(f"#define OBJECT_{layer_name.upper()}_COUNT {0}\n\n")
                        else:
                            f.write(f"// Object data for layer '{layer_name}'\n")
                            f.write(f"const unsigned char object_{layer_name}[] = {{\n")
                            for i, obj in enumerate(snes_tilemap_list_of_objects):
                                f.write(f"{obj[0]}, {obj[1]}, {obj[2]}")
                                if i < len(snes_tilemap_list_of_objects) - 1:
                                    f.write(", ")
                            f.write("\n};\n\n")
                            f.write(f"#define OBJECT_{layer_name.upper()}_COUNT {len(snes_tilemap_list_of_objects)}\n\n")

                        #Write spawnXY
                        f.write(f"// Player start location for layer '{layer_name}'\n")
                        f.write(f"const unsigned char spawn_{layer_name}[] = {{ {snes_tilemap_playerStartXY[0]}, {snes_tilemap_playerStartXY[1]} }};\n\n")

                        f.write(f"#endif // {header_guard}\n")

                    print(f"SNES tilemap data for layer '{layer_name}' written to {header_filename}")

                    map_image.save(output_path)
                    print(f"Map image saved to {output_path}")
                    bg3_image.save(output_path_bg3)
                    print(f"Background image saved to {output_path_bg3}")

    except FileNotFoundError as e:
        print(f"Error: File not found - {e}")
    #except Exception as e:
    #    print(f"An unexpected error occurred: {e}")


def find_tileset(gid, tilesets):
    """Finds the tileset that a given GID belongs to."""
    for tileset in reversed(tilesets):
        if gid >= tileset["firstgid"]:
            return tileset
    return None


if __name__ == "__main__":
    script_dir = os.path.dirname(__file__)
    tilemap_path = os.path.join(script_dir, "baseCelesteTileMap.json")
    spritesheet_path = os.path.join(script_dir, "baseCelesteSpriteSheet.png")
    output_path = os.path.join(script_dir, "BG2.png")
    output_path_bg3 = os.path.join(script_dir, "BG3.png")

    visualize_map(tilemap_path, spritesheet_path, output_path, output_path_bg3)
