-- Monitor s_titleMode to see when it flips back to true
local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_flag_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")
local ADDR_TITLE_MODE = 0x7E11B2

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

local inputState = {up=false,down=false,left=false,right=false,select=false,start=false,a=false,b=false,x=false,y=false,l=false,r=false}
local function clearPad() for k,_ in pairs(inputState) do inputState[k]=false end end
if emu.setInput then
    emu.addEventCallback(function()
        pcall(function() emu.setInput(inputState, 0) end)
    end, emu.eventType.inputPolled)
end

-- Also monitor s_titleMode via write callback
if emu.addMemoryCallback then
    emu.addMemoryCallback(function(addr, val)
        log(string.format("  WRITE s_titleMode = %d at mesen_frame=%d", val, frame))
    end, emu.callbackType.write, ADDR_TITLE_MODE)
end

log("START")
flush()

emu.addEventCallback(function()
    if done then return end
    local ok, err = pcall(function()
        frame = frame + 1
        local fc = rd16(sym.GLOBAL_FrameCount)
        local room = rd16(sym.GLOBAL_ActiveLevel)
        local tm = rd8(ADDR_TITLE_MODE)

        if frame >= 60 and frame <= 180 then
            if (frame % 20) < 10 then clearPad(); inputState.b = true
            else clearPad() end
        else clearPad() end

        if room == 1 and not roomReachedFrame then
            roomReachedFrame = frame
            log(string.format("ROOM 1 at f=%d fc=%d titleMode=%d", frame, fc, tm))
        end

        if (frame % 10) == 0 then
            log(string.format("f=%d fc=%d room=%d titleMode=%d", frame, fc, room, tm))
        end

        if roomReachedFrame and (frame - roomReachedFrame) >= 60 then
            log("DONE"); flush(); done = true
            if emu.stop then emu.stop(0) end; return
        end
        if frame >= 400 then
            log("TIMEOUT"); flush(); done = true
            if emu.stop then emu.stop(1) end
        end
        if (frame % 30) == 0 then flush() end
    end)
    if not ok then log("ERR: "..tostring(err)); flush(); done=true; if emu.stop then emu.stop(1) end end
end, emu.eventType.endFrame)
