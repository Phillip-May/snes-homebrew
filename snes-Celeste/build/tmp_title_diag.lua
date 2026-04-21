-- Diagnose title-to-gameplay transition.
-- Press B repeatedly, monitor room ID and frame count.
local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_diag_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local frame = 0
local done = false
local lines = {}

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a) return rd8(a) + rd8(a+1) * 256 end

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

flush()
log("START")

emu.addEventCallback(function()
    if done then return end
    local ok, err = pcall(function()
        frame = frame + 1

        local fc = rd16(sym.GLOBAL_FrameCount)
        local room = rd16(sym.GLOBAL_ActiveLevel)
        local px = rd16(sym.GLOBAL_PlayerData)
        local py = rd16(sym.GLOBAL_PlayerData + 2)

        -- Press B every other 10 frames from frame 60 to 300
        if frame >= 60 and frame <= 300 then
            if (frame % 20) < 10 then
                clearPad(); inputState.b = true
            else
                clearPad()
            end
        -- Then press Start every other 10 frames from 300 to 540
        elseif frame >= 300 and frame <= 540 then
            if (frame % 20) < 10 then
                clearPad(); inputState.start = true
            else
                clearPad()
            end
        -- Then Y from 540 to 780
        elseif frame >= 540 and frame <= 780 then
            if (frame % 20) < 10 then
                clearPad(); inputState.y = true
            else
                clearPad()
            end
        else
            clearPad()
        end

        -- Log every 10 frames for the first 200, then every 60
        if frame <= 200 and (frame % 10) == 0 then
            log(string.format("f=%d fc=%d room=%d px=%d py=%d btn=%s",
                frame, fc, room, px, py,
                inputState.b and "B" or (inputState.start and "Start" or (inputState.y and "Y" or "-"))))
        elseif (frame % 60) == 0 then
            log(string.format("f=%d fc=%d room=%d px=%d py=%d",
                frame, fc, room, px, py))
        end

        if room == 1 and frame > 100 then
            log("SUCCESS: room=1 reached at frame " .. frame)
            flush()
            done = true
            if emu.stop then emu.stop(0) end
            return
        end

        if frame >= 1200 then
            log("FAIL: timeout, room=" .. room .. " fc=" .. fc)
            flush()
            done = true
            if emu.stop then emu.stop(1) end
        end

        if (frame % 100) == 0 then flush() end
    end)
    if not ok then
        log("LUA ERROR: " .. tostring(err))
        flush()
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)
