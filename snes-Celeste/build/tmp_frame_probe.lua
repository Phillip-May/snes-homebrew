local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/frame_probe_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local frame = 0
local lines = {}
local done = false

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a) return rd8(a) + rd8(a + 1) * 256 end
local function log(t) table.insert(lines, t) end
local function flush()
    local f = io.open(out, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end

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
    frame = frame + 1

    if frame >= 60 and frame <= 180 then
        if (frame % 20) < 10 then clearPad(); inputState.b = true
        else clearPad() end
    else
        clearPad()
    end

    if frame <= 180 and ((frame % 10) == 0) then
        log(string.format(
            "f=%d fc7e=%d fc00=%d room=%d",
            frame,
            rd16(sym.GLOBAL_FrameCount),
            rd16(0x14EB),
            rd16(sym.GLOBAL_ActiveLevel)
        ))
        flush()
    end

    if frame >= 180 then
        log("DONE")
        flush()
        done = true
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)
