local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_read_trace_result.txt"
local lines = {}
local frame = 0
local done = false

local function flush()
    local f = io.open(out, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end

local function log(msg)
    lines[#lines + 1] = msg
    flush()
end

local function dump_state(tag, addr, val)
    local state = emu.getState and emu.getState() or nil
    local cpuState = state and state.cpu or nil
    local pc = cpuState and cpuState.pc or -1
    local k = cpuState and cpuState.k or -1
    local sp = cpuState and cpuState.sp or -1
    local a = cpuState and cpuState.a or -1
    local x = cpuState and cpuState.x or -1
    local y = cpuState and cpuState.y or -1
    local p = cpuState and cpuState.ps or -1
    log(string.format(
        "%s frame=%d addr=%06X val=%02X pc=%04X k=%02X sp=%04X a=%04X x=%04X y=%04X p=%02X",
        tag, frame, addr, val, pc, k, sp, a, x, y, p))
end

log("START")

if emu.addMemoryCallback then
    emu.addMemoryCallback(function(addr, val)
        if done then
            return
        end
        dump_state("READ_RC", addr, val)
        done = true
        if emu.stop then
            emu.stop(0)
        end
    end, emu.callbackType.read, 0x000019)

    emu.addMemoryCallback(function(addr, val)
        if done then
            return
        end
        dump_state("READ_LOW", addr, val)
        done = true
        if emu.stop then
            emu.stop(0)
        end
    end, emu.callbackType.read, 0x0C0021)

    emu.addMemoryCallback(function(addr, val)
        if done then
            return
        end
        dump_state("READ_HIGH", addr, val)
        done = true
        if emu.stop then
            emu.stop(0)
        end
    end, emu.callbackType.read, 0x0C1F60)
end

emu.addEventCallback(function()
    if done then
        return
    end
    frame = frame + 1
    if frame >= 240 then
        log("TIMEOUT")
        done = true
        if emu.stop then
            emu.stop(1)
        end
    end
end, emu.eventType.endFrame)
