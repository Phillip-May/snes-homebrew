local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_write_trace.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_DEBUG_LOGIC = 0x7E0222
local ADDR_DEBUG_MONUMENT_CALLS = 0x7E0224
local ADDR_MONUMENT_DISPLAYED = 0x7E0226
local ADDR_MONUMENT_TICK = 0x7E0227
local ADDR_MONUMENT_LINE = 0x7E0228
local ADDR_MONUMENT_CHAR = 0x7E0229

local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2

local frame = 0
local done = false
local roomHitFrame = nil
local lines = {}
local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

local function write_out()
    local f = io.open(out, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end
local function log(line)
    lines[#lines + 1] = line
    write_out()
end
local function rd8(a) return emu.read(a, cpu) end
local function rd16(a)
    local lo = rd8(a)
    local hi = rd8(a + 1)
    return lo + hi * 256
end
local function wr8(a, v) emu.write(a, v & 0xFF, cpu) end
local function wr16(a, v)
    wr8(a, v)
    wr8(a + 1, math.floor(v / 256))
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

local function trace_addr(addr, name)
    if not emu.addMemoryCallback then
        return
    end
    emu.addMemoryCallback(function(_, val)
        local room = rd16(ADDR_ROOM)
        if room ~= 12 then
            return
        end
        local state = emu.getState and emu.getState() or nil
        local cpuState = state and state.cpu or nil
        local pc = cpuState and cpuState.pc or -1
        local k = cpuState and cpuState.k or -1
        log(string.format(
            "WRITE f=%d fc=%d %s=%02X pc=%04X k=%02X disp=%d tick=%d line=%d char=%d",
            frame, rd16(ADDR_FRAME), name, val, pc, k,
            rd8(ADDR_MONUMENT_DISPLAYED), rd8(ADDR_MONUMENT_TICK),
            rd8(ADDR_MONUMENT_LINE), rd8(ADDR_MONUMENT_CHAR)))
    end, emu.callbackType.write, addr)
end

trace_addr(ADDR_MONUMENT_DISPLAYED, "disp")
trace_addr(ADDR_MONUMENT_TICK, "tick")
trace_addr(ADDR_MONUMENT_LINE, "line")
trace_addr(ADDR_MONUMENT_CHAR, "char")

log("START")

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1

    if frame >= 120 and frame <= 360 then
        local phase = frame % 30
        if phase < 10 then
            set_pad({ Start = true })
        elseif phase < 20 then
            set_pad({ A = true })
        else
            set_pad({})
        end
    elseif frame >= 390 and frame <= 540 then
        if (frame % 20) < 10 then
            set_pad({ B = true })
        else
            set_pad({})
        end
    else
        set_pad({})
    end

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)
    if room == 12 then
        if roomHitFrame == nil then
            roomHitFrame = frame
            log(string.format("ROOM12 frame=%d fc=%d", frame, fc))
        end
        wr16(PLAYER_X, 60)
        wr16(PLAYER_Y, 40)
    end

    if frame == 1 or room == 12 then
        log(string.format("FRAME f=%d fc=%d room=%d logic=%d moncalls=%d disp=%d tick=%d line=%d char=%d",
            frame, fc, room,
            rd16(ADDR_DEBUG_LOGIC), rd16(ADDR_DEBUG_MONUMENT_CALLS),
            rd8(ADDR_MONUMENT_DISPLAYED), rd8(ADDR_MONUMENT_TICK),
            rd8(ADDR_MONUMENT_LINE), rd8(ADDR_MONUMENT_CHAR)))
    end

    if roomHitFrame ~= nil and (frame - roomHitFrame) >= 180 then
        log("DONE")
        done = true
        if emu.stop then emu.stop(0) end
        return
    end

    if frame >= 1200 then
        log("TIMEOUT")
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)
