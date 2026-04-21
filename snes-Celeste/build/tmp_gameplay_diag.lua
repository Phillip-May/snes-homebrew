-- Check if gameplay works after room load: player moves, frames advance
local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/gameplay_diag_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local frame = 0
local done = false
local lines = {}
local roomReachedFrame = nil

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a) return rd8(a) + rd8(a+1) * 256 end
local function rds16(a) local v=rd16(a); if v>=0x8000 then return v-0x10000 end; return v end
local function log(t) table.insert(lines, t) end
local function flush()
    local f = io.open(out, "w")
    if f then f:write(table.concat(lines, "\n") .. "\n"); f:close() end
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

emu.addEventCallback(function()
    if done then return end
    local ok, err = pcall(function()
        frame = frame + 1

        local fc = rd16(sym.GLOBAL_FrameCount)
        local room = rd16(sym.GLOBAL_ActiveLevel)
        local px = rds16(sym.GLOBAL_PlayerData)
        local py = rds16(sym.GLOBAL_PlayerData + 2)

        -- Press B to start game
        if frame >= 60 and frame <= 180 then
            if (frame % 20) < 10 then
                clearPad(); inputState.b = true
            else clearPad() end
        -- After room 1 reached, try moving right
        elseif roomReachedFrame and frame >= roomReachedFrame + 60 then
            clearPad(); inputState.right = true
        else
            clearPad()
        end

        if room == 1 and not roomReachedFrame then
            roomReachedFrame = frame
            log(string.format("ROOM 1 at f=%d fc=%d px=%d py=%d", frame, fc, px, py))
        end

        -- Log every 30 frames after room reached
        if roomReachedFrame and (frame % 30) == 0 then
            log(string.format("f=%d fc=%d room=%d px=%d py=%d (since_room=%d)",
                frame, fc, room, px, py, frame - roomReachedFrame))
        end

        -- After 600 frames past room load, check results
        if roomReachedFrame and (frame - roomReachedFrame) >= 600 then
            log("DONE")
            flush()
            done = true
            if emu.stop then emu.stop(0) end
            return
        end

        if frame >= 1800 then
            log("TIMEOUT room=" .. room)
            flush()
            done = true
            if emu.stop then emu.stop(1) end
        end

        if (frame % 200) == 0 then flush() end
    end)
    if not ok then
        log("LUA ERROR: " .. tostring(err))
        flush(); done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)
