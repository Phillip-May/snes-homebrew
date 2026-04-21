local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/manual_title_input_trace.txt"

local ADDR_INPUT_STATE = 0x7E0214
local ADDR_IN_TITLE = 0x7E0227
local ADDR_TITLE_TIMER = 0x7E022F
local ADDR_INPUT_LO = 0x7E0284
local ADDR_ROOM = 0x7E0285
local ADDR_FRAME = 0x7E1488
local ADDR_PLAYER = 0x7E148E
local REG_JOY1L = 0x4218
local REG_JOY1H = 0x4219

local frame = 0
local lines = {}
local last = nil

local function rd8(addr)
    return emu.read(addr, cpu) or 0
end

local function rd16(addr)
    local lo = rd8(addr)
    local hi = rd8(addr + 1)
    return lo + hi * 256
end

local function rs16(addr)
    local v = rd16(addr)
    if v >= 0x8000 then
        return v - 0x10000
    end
    return v
end

local function flush()
    local f = io.open(out, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end

local function log(msg)
    lines[#lines + 1] = msg
    flush()
end

log("RUNNING")

emu.addEventCallback(function()
    frame = frame + 1

    local cur = {
        fc = rd16(ADDR_FRAME),
        room = rd16(ADDR_ROOM),
        state = rd8(ADDR_INPUT_STATE),
        lo = rd8(ADDR_INPUT_LO),
        title = rd8(ADDR_IN_TITLE),
        timer = rd8(ADDR_TITLE_TIMER),
        joy1l = rd8(REG_JOY1L),
        joy1h = rd8(REG_JOY1H),
        px = rs16(ADDR_PLAYER + 0),
        py = rs16(ADDR_PLAYER + 2)
    }

    local changed = false
    if last == nil then
        changed = true
    else
        changed =
            cur.room ~= last.room or
            cur.state ~= last.state or
            cur.lo ~= last.lo or
            cur.title ~= last.title or
            cur.timer ~= last.timer or
            cur.joy1l ~= last.joy1l or
            cur.joy1h ~= last.joy1h
    end

    if changed or (frame % 60) == 0 then
        log(string.format(
            "f=%d fc=%d room=%d state=%02X lo=%02X title=%02X timer=%02X joy1l=%02X joy1h=%02X px=%d py=%d",
            frame, cur.fc, cur.room, cur.state, cur.lo, cur.title, cur.timer,
            cur.joy1l, cur.joy1h, cur.px, cur.py
        ))
    end

    last = cur
end, emu.eventType.endFrame)
