local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_crash_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM_ID = sym.GLOBAL_ActiveLevel
local ADDR_FRAME_COUNT = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local PLAYER_POS_X = ADDR_PLAYER + 0
local PLAYER_POS_Y = ADDR_PLAYER + 2

local TARGET_ROOM = 12
local TARGET_X = 60
local TARGET_Y = 40

local frame = 0
local done = false
local lines = {}
local lastFc = nil
local sameFcCount = 0
local room12Frame = nil

local inputState = {
    up=false, down=false, left=false, right=false,
    select=false, start=false, a=false, b=false,
    x=false, y=false, l=false, r=false
}

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a) return rd8(a) + rd8(a + 1) * 256 end
local function wr8(a,v) emu.write(a, v & 0xFF, cpu) end
local function wr16(a,v)
    wr8(a, v)
    wr8(a + 1, math.floor(v / 256))
end
local function clearPad() for k,_ in pairs(inputState) do inputState[k] = false end end
local function log(s) table.insert(lines, s) end
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

log("START")
flush()

emu.addEventCallback(function()
    if done then return end

    frame = frame + 1
    clearPad()

    local fc = rd16(ADDR_FRAME_COUNT)
    local room = rd16(ADDR_ROOM_ID)

    if lastFc == fc then
        sameFcCount = sameFcCount + 1
    else
        sameFcCount = 0
    end
    lastFc = fc

    if frame >= 60 and frame <= 180 then
        if (frame % 20) < 10 then
            inputState.b = true
        end
    elseif room > 0 and room < TARGET_ROOM and (frame % 12) == 0 then
        inputState.r = true
    end

    if room == TARGET_ROOM then
        if not room12Frame then
            room12Frame = frame
            log(string.format("ROOM12 frame=%d fc=%d", frame, fc))
        end
        wr16(PLAYER_POS_X, TARGET_X)
        wr16(PLAYER_POS_Y, TARGET_Y)
    end

    if (frame % 10) == 0 or room == TARGET_ROOM then
        log(string.format("f=%d fc=%d room=%d sameFc=%d px=%d py=%d", frame, fc, room, sameFcCount, rd16(PLAYER_POS_X), rd16(PLAYER_POS_Y)))
    end

    if sameFcCount >= 20 then
        log("HANG")
        flush()
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if room12Frame and (frame - room12Frame) >= 180 then
        log("DONE")
        flush()
        done = true
        if emu.stop then emu.stop(0) end
        return
    end

    if frame >= 1200 then
        log("TIMEOUT")
        flush()
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if (frame % 20) == 0 then
        flush()
    end
end, emu.eventType.endFrame)
