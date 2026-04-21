-- Debug NMI hang: monitor what happens when NMI fires after room load.
-- Track NMI count, HVBJOY state, stack pointer, program counter.
local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/nmi_debug_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local frame = 0
local done = false
local lines = {}
local nmiCount = 0
local roomReachedFrame = nil

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a) return rd8(a) + rd8(a+1) * 256 end
local function log(t) table.insert(lines, t) end
local function flush()
    local f = io.open(out, "w")
    if f then f:write(table.concat(lines, "\n") .. "\n"); f:close() end
end

-- Count NMI fires by hooking the NMI vector address
local NMI_ADDR = 0x80C0 -- from map file
if emu.addMemoryCallback then
    emu.addMemoryCallback(function()
        nmiCount = nmiCount + 1
    end, emu.callbackType.exec, NMI_ADDR)
end

-- Input injection
local inputState = {
    up=false, down=false, left=false, right=false,
    select=false, start=false, a=false, b=false,
    x=false, y=false, l=false, r=false
}
local function clearPad() for k,_ in pairs(inputState) do inputState[k]=false end end
if emu.setInput then
    emu.addEventCallback(function()
        pcall(function() emu.setInput(inputState, 0) end)
    end, emu.eventType.inputPolled)
end

log("START")
flush()

emu.addEventCallback(function()
    if done then return end
    local ok, err = pcall(function()
        frame = frame + 1

        local fc = rd16(sym.GLOBAL_FrameCount)
        local room = rd16(sym.GLOBAL_ActiveLevel)

        -- Press B to start
        if frame >= 60 and frame <= 180 then
            if (frame % 20) < 10 then clearPad(); inputState.b = true
            else clearPad() end
        else clearPad() end

        if room == 1 and not roomReachedFrame then
            roomReachedFrame = frame
            log(string.format("ROOM 1 at f=%d fc=%d nmi=%d", frame, fc, nmiCount))
        end

        -- Log state around the hang point
        if roomReachedFrame then
            if (frame - roomReachedFrame) <= 30 or (frame % 60) == 0 then
                -- Read HVBJOY and NMITIMEN
                local hvbjoy = rd8(0x4212)
                local nmitimen = rd8(0x4200)
                local sp = nil
                -- Try to get CPU state
                local state = emu.getState and emu.getState() or nil
                local pc = state and state.cpu and state.cpu.pc or -1
                local spVal = state and state.cpu and state.cpu.sp or -1

                log(string.format("f=%d fc=%d nmi=%d room=%d hvb=0x%02X nmit=0x%02X pc=0x%04X sp=0x%04X",
                    frame, fc, nmiCount, room, hvbjoy, nmitimen, pc, spVal))
            end
        end

        if roomReachedFrame and (frame - roomReachedFrame) >= 300 then
            log("DONE")
            flush()
            done = true
            if emu.stop then emu.stop(0) end
            return
        end

        if frame >= 900 then
            log("TIMEOUT room=" .. room .. " nmi=" .. nmiCount)
            flush()
            done = true
            if emu.stop then emu.stop(1) end
        end

        if (frame % 50) == 0 then flush() end
    end)
    if not ok then
        log("LUA ERROR: " .. tostring(err))
        flush(); done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)
