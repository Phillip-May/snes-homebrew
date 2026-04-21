local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room8_stage_trace_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_STAGE = 0x7E027B
local ADDR_A0 = 0x7E027C
local ADDR_A1 = 0x7E027D
local ADDR_A2 = 0x7E027E

local frame = 0
local lastRoom = -1
local lines = {}
local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

local function rd8(a) return emu.read(a, cpu) or 0 end
local function rd16(a) return rd8(a) + rd8(a + 1) * 256 end
local function rs16(a)
    local v = rd16(a)
    if v >= 0x8000 then return v - 0x10000 end
    return v
end
local function flush()
    local f = io.open(out, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end
local function log(s)
    lines[#lines + 1] = s
    flush()
end
local function set_pad(tbl)
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
    for k, v in pairs(tbl) do
        inputState[string.lower(k)] = v and true or false
    end
end

if emu.setInput then
    emu.addEventCallback(function()
        local ok = pcall(function() emu.setInput(inputState, 0) end)
        if not ok then
            pcall(function() emu.setInput(inputState, 1) end)
        end
    end, emu.eventType.inputPolled)
end

log("RUNNING")

emu.addEventCallback(function()
    frame = frame + 1

    if frame >= 120 and frame <= 220 then
        if (frame % 20) < 10 then
            set_pad({ start = true })
        else
            set_pad({})
        end
    else
        set_pad({})
    end

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)
    local px = rs16(ADDR_PLAYER)
    local py = rs16(ADDR_PLAYER + 2)
    local st = rd8(ADDR_STAGE)
    local a0 = rd8(ADDR_A0)
    local a1 = rd8(ADDR_A1)
    local a2 = rd8(ADDR_A2)

    if room ~= lastRoom then
        log(string.format("ROOM f=%d fc=%u room=%u px=%d py=%d st=%02X a0=%02X a1=%02X a2=%02X", frame, fc, room, px, py, st, a0, a1, a2))
        lastRoom = room
    end

    if room == 8 or frame % 15 == 0 then
        log(string.format("TRACE f=%d fc=%u room=%u px=%d py=%d st=%02X a0=%02X a1=%02X a2=%02X", frame, fc, room, px, py, st, a0, a1, a2))
    end

    if frame >= 360 then
        log("DONE")
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)
