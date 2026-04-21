local cpu = emu.memType.cpu
local outPath = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room12_direct_monument_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData

local frame = 0
local lastFc = nil
local sameFc = 0
local done = false

local function write_result(text)
    local f = io.open(outPath, "w")
    if f then
        f:write(text)
        f:close()
    end
end

local function rd8(addr)
    return emu.read(addr, cpu)
end

local function rd16(addr)
    local lo = rd8(addr)
    local hi = rd8(addr + 1)
    if lo == nil or hi == nil then
        return nil
    end
    return lo + hi * 256
end

write_result("RUNNING")

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1

    local fc = rd16(ADDR_FRAME)
    local room = rd16(ADDR_ROOM)
    local px = rd16(ADDR_PLAYER + 0)
    local py = rd16(ADDR_PLAYER + 2)

    if fc == nil or room == nil or px == nil or py == nil then
        write_result("FAIL nil-read")
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if lastFc == fc then
        sameFc = sameFc + 1
    else
        sameFc = 0
    end
    lastFc = fc

    if sameFc >= 20 then
        write_result("FAIL hang frame=" .. frame .. " fc=" .. fc .. " room=" .. room .. " px=" .. px .. " py=" .. py)
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if frame >= 600 then
        write_result("PASS frame=" .. frame .. " fc=" .. fc .. " room=" .. room .. " px=" .. px .. " py=" .. py)
        done = true
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)
