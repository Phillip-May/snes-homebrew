local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_state_trace.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_MONUMENT_DISPLAYED = 0x7E0222
local ADDR_MONUMENT_TICK = 0x7E0223
local ADDR_MONUMENT_LINE = 0x7E0224
local ADDR_MONUMENT_CHAR = 0x7E0225
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
local function clearPad()
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
end
local function log(line)
    table.insert(lines, line)
end
local function flush()
    local f = io.open(out, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end

if emu.setInput then
    emu.addEventCallback(function()
        pcall(function() emu.setInput(inputState, 0) end)
    end, emu.eventType.inputPolled)
end

local function addWriteTrace(addr, name)
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
        local sp = cpuState and cpuState.sp or -1
        log(string.format(
            "WRITE f=%d fc=%d %s=%02X room=%d pc=%04X k=%02X sp=%04X disp=%d tick=%d line=%d char=%d",
            frame, rd16(ADDR_FRAME), name, val, room, pc, k, sp,
            rd8(ADDR_MONUMENT_DISPLAYED), rd8(ADDR_MONUMENT_TICK),
            rd8(ADDR_MONUMENT_LINE), rd8(ADDR_MONUMENT_CHAR)))
        flush()
    end, emu.callbackType.write, addr)
end

addWriteTrace(ADDR_MONUMENT_DISPLAYED, "disp")
addWriteTrace(ADDR_MONUMENT_TICK, "tick")
addWriteTrace(ADDR_MONUMENT_LINE, "line")
addWriteTrace(ADDR_MONUMENT_CHAR, "char")

log("START")
flush()

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1
    clearPad()

    if frame >= 120 and frame <= 360 then
        local phase = frame % 30
        if phase < 10 then
            inputState.start = true
        elseif phase < 20 then
            inputState.a = true
        else
            inputState.b = true
        end
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
        log(string.format("FRAME f=%d fc=%d room=%d px=%d py=%d disp=%d tick=%d line=%d char=%d",
            frame, fc, room, rd16(PLAYER_X), rd16(PLAYER_Y),
            rd8(ADDR_MONUMENT_DISPLAYED), rd8(ADDR_MONUMENT_TICK),
            rd8(ADDR_MONUMENT_LINE), rd8(ADDR_MONUMENT_CHAR)))
        flush()
    end

    if roomHitFrame ~= nil and (frame - roomHitFrame) >= 120 then
        log("DONE")
        flush()
        done = true
        if emu.stop then emu.stop(0) end
        return
    end

    if frame >= 600 then
        log("TIMEOUT")
        flush()
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)
