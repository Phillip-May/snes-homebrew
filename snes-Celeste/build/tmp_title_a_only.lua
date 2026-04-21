local cpu = emu.memType.cpu
local outTxt = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_a_only_result.txt"
local outTitlePng = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_a_only_before.png"
local outGamePng = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_a_only_after.png"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData

local frame = 0
local done = false
local lines = {}
local lastFc = nil
local sameFc = 0

local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

local function rd8(addr) return emu.read(addr, cpu) end
local function rd16(addr)
    local lo = rd8(addr)
    local hi = rd8(addr + 1)
    if lo == nil or hi == nil then return nil end
    return lo + hi * 256
end
local function rs16(addr)
    local v = rd16(addr)
    if v == nil then return nil end
    if v >= 0x8000 then return v - 0x10000 end
    return v
end
local function flush()
    local f = io.open(outTxt, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end
local function log(msg)
    lines[#lines + 1] = msg
    flush()
end
local function write_png(path, png)
    local f = io.open(path, "wb")
    if f then
        f:write(png)
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

log("RUNNING")

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end

    if frame >= 240 and frame <= 360 then
        inputState.a = true
    end

    local fc = rd16(ADDR_FRAME)
    local room = rd16(ADDR_ROOM)
    local px = rs16(ADDR_PLAYER + 0)
    local py = rs16(ADDR_PLAYER + 2)

    if fc == nil or room == nil or px == nil or py == nil then
        log("FAIL nil-state-read frame=" .. tostring(frame))
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if frame == 180 then
        write_png(outTitlePng, emu.takeScreenshot())
        log(string.format("SHOT_TITLE frame=%d fc=%d room=%d", frame, fc, room))
    end

    if frame == 420 then
        write_png(outGamePng, emu.takeScreenshot())
        log(string.format("SHOT_POST_A frame=%d fc=%d room=%d x=%d y=%d", frame, fc, room, px, py))
    end

    if lastFc == fc then
        sameFc = sameFc + 1
    else
        sameFc = 0
    end
    lastFc = fc

    if frame == 1 or (frame % 60) == 0 or room ~= 0 then
        log(string.format("TRACE frame=%d fc=%d room=%d a=%s x=%d y=%d same=%d", frame, fc, room, tostring(inputState.a), px, py, sameFc))
    end

    if sameFc >= 120 then
        log(string.format("FAIL framecount-stalled frame=%d fc=%d room=%d", frame, fc, room))
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if frame >= 900 then
        if room ~= 1 then
            log(string.format("FAIL gameplay-not-reached frame=%d fc=%d room=%d x=%d y=%d", frame, fc, room, px, py))
            done = true
            if emu.stop then emu.stop(1) end
            return
        end
        log(string.format("PASS frame=%d fc=%d room=%d x=%d y=%d", frame, fc, room, px, py))
        done = true
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)
