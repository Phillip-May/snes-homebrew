-- Mesen 2 Lua script for SPC700 debugging
-- Reads debug ring buffer at 0xFE00 written by main.c

-- Log to both Mesen Script Window and file (for --testRunner headless mode)
local _logFile = io.open("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/ouptut.log", "w")
local _emu_log = emu.log
emu.log = function(msg)
    _emu_log(msg)
    if _logFile then _logFile:write(msg .. "\n"); _logFile:flush() end
end

local spcRam = emu.memType.spcRam
local spcIO  = emu.memType.spcMemory

-- Debug buffer protocol (must match main.c)
local DBG_BUF  = 0xFE00
local DBG_SIZE = 0xFE
local DBG_WPTR = 0xFEFE
local lastRead = 0

local function readDebugBuffer()
    local wptr = emu.read(DBG_WPTR, spcRam) + emu.read(DBG_WPTR + 1, spcRam) * 256
    if wptr < lastRead then
        emu.log("[SPC] -- reset --")
        lastRead = 0
    end
    if wptr == lastRead then return end

    local buf = ""
    while lastRead ~= wptr and lastRead < DBG_SIZE do
        local ch = emu.read(DBG_BUF + lastRead, spcRam)
        lastRead = lastRead + 1
        if ch == 0x0A or ch == 0x00 then
            if #buf > 0 then emu.log("[SPC] " .. buf) end
            buf = ""
        elseif ch >= 0x20 and ch <= 0x7E then
            buf = buf .. string.char(ch)
        else
            buf = buf .. string.format("\\x%02X", ch)
        end
    end
    if #buf > 0 then emu.log("[SPC] " .. buf) end
end

local SPC_PORT0 = 0xF4
local SPC_PORT1 = 0xF5
local SNES_APU0 = 0x2140
local SNES_APU1 = 0x2141
local snesMem = emu.memType.snesMemory

local function sendCommand(cmd, data)
    emu.log(string.format("[TEST] cmd=%02X data=%02X", cmd, data))
    pcall(emu.write, SNES_APU0, data & 0xFF, snesMem)
    pcall(emu.write, SNES_APU1, cmd & 0xFF, snesMem)
    emu.write(SPC_PORT0, data & 0xFF, spcIO)
    emu.write(SPC_PORT1, cmd & 0xFF, spcIO)
    emu.write(SPC_PORT0, data & 0xFF, spcRam)
    emu.write(SPC_PORT1, cmd & 0xFF, spcRam)
end

-- Test sequence: send SFX commands after boot
local testStep = 0
local testDelay = 0
local function runTest()
    if testDelay > 0 then testDelay = testDelay - 1; return end
    if testStep == 0 then
        testDelay = 120
        testStep = 1
    elseif testStep == 1 then
        sendCommand(0x04, 0x01)
        testDelay = 120
        testStep = 2
    elseif testStep == 2 then
        sendCommand(0x04, 0x02)
        testDelay = 120
        testStep = 3
    elseif testStep == 3 then
        readDebugBuffer()
        emu.log("[TEST] === Done ===")
        if emu.stop then emu.stop(0) end
        testStep = 99
    end
end

emu.addEventCallback(function()
    readDebugBuffer()
    runTest()
end, emu.eventType.endFrame)

readDebugBuffer()
emu.log("[testSPC] Ready")
