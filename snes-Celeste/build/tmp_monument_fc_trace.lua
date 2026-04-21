local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_fc_trace_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local TARGET_ROOM = 12
local TOUCH_X = 60
local TOUCH_Y = 40
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
local textStarted = false
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
    for k, _ in pairs(inputState) do inputState[k] = false end
end
local function log(line)
    lines[#lines + 1] = line
end
local function flush()
    local f = io.open(out, "w")
    if f then f:write(table.concat(lines, "\n") .. "\n"); f:close() end
end

if emu.setInput then
    emu.addEventCallback(function()
        local ok = pcall(function() emu.setInput(inputState, 0) end)
        if not ok then pcall(function() emu.setInput(inputState, 1) end) end
    end, emu.eventType.inputPolled)
end

if emu.addMemoryCallback then
    local function traceWrite(addr, val)
        local room = rd16(ADDR_ROOM)
        if room ~= TARGET_ROOM then
            return
        end
        local mon = rd8(ADDR_MONUMENT_DISPLAYED)
        local ch = rd8(ADDR_MONUMENT_CHAR)
        if not textStarted and (mon == 0 and ch == 0) then
            return
        end
        local state = emu.getState and emu.getState() or nil
        local cpuState = state and state.cpu or nil
        log(string.format(
            "WRITE f=%d fc=%04X room=%04X addr=%06X val=%02X pc=%04X k=%02X sp=%04X a=%04X x=%04X y=%04X p=%02X mon=%d tick=%d line=%d char=%d",
            frame,
            rd16(ADDR_FRAME),
            room,
            addr,
            val,
            cpuState and cpuState.pc or -1,
            cpuState and cpuState.k or -1,
            cpuState and cpuState.sp or -1,
            cpuState and cpuState.a or -1,
            cpuState and cpuState.x or -1,
            cpuState and cpuState.y or -1,
            cpuState and cpuState.ps or -1,
            mon,
            rd8(ADDR_MONUMENT_TICK),
            rd8(ADDR_MONUMENT_LINE),
            ch
        ))
        flush()
    end
    emu.addMemoryCallback(traceWrite, emu.callbackType.write, ADDR_FRAME)
    emu.addMemoryCallback(traceWrite, emu.callbackType.write, ADDR_FRAME + 1)
end

log("START")
flush()

emu.addEventCallback(function()
    if done then return end

    frame = frame + 1
    clearPad()

    if frame >= 120 and frame <= 360 then
        local phase = frame % 30
        if phase < 10 then inputState.start = true
        elseif phase < 20 then inputState.a = true end
    elseif frame >= 390 and frame <= 540 then
        if (frame % 20) < 10 then inputState.b = true end
    end

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)

    if room == TARGET_ROOM then
        if roomHitFrame == nil then
            roomHitFrame = frame
            log(string.format("ROOM%d frame=%d fc=%d", TARGET_ROOM, frame, fc))
        end
        wr16(PLAYER_X, TOUCH_X)
        wr16(PLAYER_Y, TOUCH_Y)
    end

    local mon = rd8(ADDR_MONUMENT_DISPLAYED)
    local tick = rd8(ADDR_MONUMENT_TICK)
    local line = rd8(ADDR_MONUMENT_LINE)
    local ch = rd8(ADDR_MONUMENT_CHAR)
    if room == TARGET_ROOM and not textStarted and (mon ~= 0 or line ~= 0 or ch ~= 0) then
        textStarted = true
        log(string.format("TEXT_START frame=%d fc=%d line=%d char=%d tick=%d", frame, fc, line, ch, tick))
        flush()
    end

    if frame == 1 or (frame % 30) == 0 or room == TARGET_ROOM then
        log(string.format("FRAME f=%d fc=%d room=%d mon=%d tick=%d line=%d char=%d",
            frame, fc, room, mon, tick, line, ch))
        flush()
    end

    if roomHitFrame ~= nil and room ~= TARGET_ROOM then
        log(string.format("ROOM_CORRUPT frame=%d room=%d fc=%d", frame, room, fc))
        flush()
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if roomHitFrame ~= nil and (frame - roomHitFrame) >= 300 then
        log("DONE")
        flush()
        done = true
        if emu.stop then emu.stop(0) end
        return
    end

    if frame >= 1500 then
        log("TIMEOUT")
        flush()
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)
