-- Find where port_vblank hangs after room load.
-- s_nmiEnabled at 0x7E14EE is used as a progress marker:
-- 0 = not in vblank poll, 1 = entered poll, 2 = passed first loop, 3 = passed both
local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/hang_debug_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")
local ADDR_DBG = 0x7E14EE -- s_nmiEnabled = debug progress marker

local frame = 0
local done = false
local lines = {}
local roomReachedFrame = nil

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a) return rd8(a) + rd8(a+1) * 256 end
local function log(t) table.insert(lines, t) end
local function flush()
    local f = io.open(out, "w")
    if f then f:write(table.concat(lines, "\n") .. "\n"); f:close() end
end

local inputState = {
    up=false,down=false,left=false,right=false,
    select=false,start=false,a=false,b=false,
    x=false,y=false,l=false,r=false
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
        local dbg = rd8(ADDR_DBG)

        if frame >= 60 and frame <= 180 then
            if (frame % 20) < 10 then clearPad(); inputState.b = true
            else clearPad() end
        else clearPad() end

        if room == 1 and not roomReachedFrame then
            roomReachedFrame = frame
            log(string.format("ROOM 1 at f=%d fc=%d dbg=%d", frame, fc, dbg))
        end

        if roomReachedFrame and ((frame - roomReachedFrame) <= 20 or (frame % 30) == 0) then
            log(string.format("f=%d fc=%d dbg=%d room=%d", frame, fc, dbg, room))
        end

        if roomReachedFrame and (frame - roomReachedFrame) >= 180 then
            log("DONE"); flush(); done = true
            if emu.stop then emu.stop(0) end; return
        end
        if frame >= 600 then
            log("TIMEOUT room=" .. room .. " dbg=" .. dbg); flush(); done = true
            if emu.stop then emu.stop(1) end
        end
        if (frame % 50) == 0 then flush() end
    end)
    if not ok then log("ERR: "..tostring(err)); flush(); done=true; if emu.stop then emu.stop(1) end end
end, emu.eventType.endFrame)
