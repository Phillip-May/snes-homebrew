"""Write C data as a .inc fragment (#included into one translation unit)."""


def _rows(values, per_line, fmt):
    lines = []
    for i in range(0, len(values), per_line):
        chunk = values[i:i + per_line]
        lines.append("    " + ", ".join(fmt % v for v in chunk))
    return ",\n".join(lines)


def u8(name, values):
    return {"kind": "u8", "name": name, "values": list(values)}


def u16(name, values):
    return {"kind": "u16", "name": name, "values": list(values)}


def u16_2d(name, rows):
    return {"kind": "u16_2d", "name": name, "rows": [list(r) for r in rows]}


def write_inc(path, arrays, defines=None, comment=None):
    parts = []
    if comment:
        parts.append("/* %s */" % comment)
        parts.append("")
    for a in arrays:
        # Optional placement annotation (e.g. a PORT_DATA_BANK1 macro that
        # collapses to nothing on toolchains that don't need it).
        pfx = a.get("prefix")
        pfx = (pfx + " ") if pfx else ""
        if a["kind"] == "u8":
            v = a["values"]
            parts.append("%sconst unsigned char %s[%d] = {" % (pfx, a["name"], len(v)))
            parts.append(_rows(v, 16, "0x%02X"))
            parts.append("};")
        elif a["kind"] == "u16":
            v = a["values"]
            parts.append("%sconst unsigned short %s[%d] = {" % (pfx, a["name"], len(v)))
            parts.append(_rows(v, 16, "0x%04X"))
            parts.append("};")
        elif a["kind"] == "u16_2d":
            rows = a["rows"]
            cols = len(rows[0]) if rows else 0
            parts.append("%sconst unsigned short %s[%d][%d] = {"
                         % (pfx, a["name"], len(rows), cols))
            body = []
            for r in rows:
                body.append("    { " + ", ".join("0x%04X" % v for v in r) + " }")
            parts.append(",\n".join(body))
            parts.append("};")
        parts.append("")
    if defines:
        for k, val in defines.items():
            parts.append("#define %s %d" % (k, val))
        parts.append("")
    with open(path, "w") as f:
        f.write("\n".join(parts))
