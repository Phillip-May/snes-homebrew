"""Tiled (.tmx) map loader.

Parses a Tiled `.tmx` map (the editor's native XML) into plain Python so a
project's convert.py can build SNES assets from the map that is its source of
truth -- the tile layout, each tile's custom properties (e.g. a collision
`attribute`), and the object layer (entity spawns). Generic: it keeps property
values as raw strings and leaves their meaning to the caller.

    import pySnesDevTools as snes
    m = snes.load_tmx("level.tmx")
    m.width, m.height            # map size in tiles
    m.tilelayer                  # flat list of gids, row-major (0 = empty)
    m.tile_props[gid]["attribute"]   # a tile's custom property (raw string)
    for o in m.objects: o.name, o.x, o.y, o.props
"""

import xml.etree.ElementTree as ET


class TiledObject:
    """One object from an <objectgroup> (an entity spawn, region, ...)."""

    def __init__(self, name, kind, x, y, width, height, props):
        self.name = name
        self.kind = kind          # Tiled "class" (older files: "type")
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.props = props        # {name: raw string value}


class TiledMap:
    def __init__(self, width, height, tilewidth, tileheight,
                 tilelayer, tile_props, objects):
        self.width = width
        self.height = height
        self.tilewidth = tilewidth
        self.tileheight = tileheight
        self.tilelayer = tilelayer    # flat list[int] of gids, row-major
        self.tile_props = tile_props  # {gid: {prop name: raw string}}
        self.objects = objects        # list[TiledObject]


def _props(elem):
    """{name: value} for the <properties> child of elem (empty if none)."""
    out = {}
    p = elem.find("properties")
    if p is not None:
        for prop in p.findall("property"):
            out[prop.get("name")] = prop.get("value")
    return out


def load_tmx(path):
    """Parse a Tiled .tmx file into a TiledMap.

    Reads the first <layer> (its <data encoding="csv"> tile gids, row-major),
    every <tileset>'s per-tile <properties> (keyed by global tile id =
    firstgid + local id), and every <object> across all <objectgroup>s. Only
    CSV-encoded layer data is supported (base64/compressed is not).
    """
    root = ET.parse(path).getroot()
    width = int(root.get("width"))
    height = int(root.get("height"))
    tilewidth = int(root.get("tilewidth"))
    tileheight = int(root.get("tileheight"))

    tile_props = {}
    for ts in root.findall("tileset"):
        firstgid = int(ts.get("firstgid", 1))
        for tile in ts.findall("tile"):
            props = _props(tile)
            if props:
                tile_props[firstgid + int(tile.get("id"))] = props

    layer = root.find("layer")
    data = layer.find("data")
    if data.get("encoding") != "csv":
        raise ValueError("only CSV-encoded layer data is supported (got %r)"
                         % data.get("encoding"))
    tilelayer = [int(v) for v in data.text.replace("\n", "").split(",") if v.strip()]

    objects = []
    for group in root.findall("objectgroup"):
        for o in group.findall("object"):
            objects.append(TiledObject(
                o.get("name"), o.get("class") or o.get("type"),
                float(o.get("x", 0)), float(o.get("y", 0)),
                float(o.get("width", 0)), float(o.get("height", 0)),
                _props(o)))

    return TiledMap(width, height, tilewidth, tileheight,
                    tilelayer, tile_props, objects)
