local cpu = emu.memType.cpu
local outTxt = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_to_room12_monument_restart_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_MONUMENT_DISPLAYED = sym.GLOBAL_MonumentTextDisplayed
local ADDR_MONUMENT_TICK = sym.s_monumentTextTick
local ADDR_MONUMENT_LINE = sym.GLOBAL_MonumentCurLineNum
local ADDR_MONUMENT_CHAR = sym.GLOBAL_MonumentCurLineCharCount

local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2

local frame = 0
local done = false
local room12Frame = nil
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
local function setPad(tbl)
    clearPad()
    for k, v in pairs(tbl) do
        inputState[string.lower(k)] = v and true or false
    end
end
local function log(line)
    lines[#lines + 1] = line
    local f = io.open(outTxt, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
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

log("START")

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1
    clearPad()

    if frame >= 120 and frame <= 360 then
        local phase = frame % 30
        if phase < 10 then
            setPad({ Start = true })
        elseif phase < 20 then
            setPad({ A = true })
        end
    end

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)
    local mon = rd8(ADDR_MONUMENT_DISPLAYED)
    local tick = rd8(ADDR_MONUMENT_TICK)
    local line = rd8(ADDR_MONUMENT_LINE)
    local char = rd8(ADDR_MONUMENT_CHAR)

    if room == 12 and room12Frame == nil then
        room12Frame = frame
        log(string.format("ROOM12 frame=%d fc=%d", frame, fc))
    end

    if room12Frame ~= nil then
        local age = frame - room12Frame
        if age < 120 then
            wr16(PLAYER_X, 60)
            wr16(PLAYER_Y, 40)
        elseif age < 180 then
            wr16(PLAYER_X, 8)
            wr16(PLAYER_Y, 96)
        else
            wr16(PLAYER_X, 60)
            wr16(PLAYER_Y, 40)
        end
    end

    if frame == 1 or (frame % 10) == 0 or room12Frame ~= nil then
        log(string.format("f=%d fc=%d room=%d mon=%d tick=%d line=%d char=%d px=%d py=%d",
            frame, fc, room, mon, tick, line, char, rd16(PLAYER_X), rd16(PLAYER_Y)))
    end

    if room12Frame ~= nil and (frame - room12Frame) >= 300 then
        log("DONE")
        done = true
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)
