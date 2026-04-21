local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room12_monument_min_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_MONUMENT_DISPLAYED = 0x7E0226
local ADDR_MONUMENT_TICK = 0x7E0227
local ADDR_MONUMENT_LINE = 0x7E0228
local ADDR_MONUMENT_CHAR = 0x7E0229

local TARGET_ROOM = 12
local frame = 0
local room12Frame = nil
local lines = {}
local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

local function write_result(text)
    local f = io.open(out, "w")
    if f then
        f:write(text)
        f:close()
    end
end

local function log(line)
    lines[#lines + 1] = line
    write_result(table.concat(lines, "\n") .. "\n")
end

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a)
    local lo = rd8(a)
    local hi = rd8(a + 1)
    if lo == nil or hi == nil then return nil end
    return lo + hi * 256
end

local function set_pad(tbl)
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
    for k, v in pairs(tbl) do
        inputState[string.lower(k)] = v and true or false
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

write_result("START\n")

emu.addEventCallback(function()
    local ok, err = pcall(function()
        frame = frame + 1

        local room = rd16(ADDR_ROOM)
        local fc = rd16(ADDR_FRAME)
        if room == nil or fc == nil then
            log("FAIL nil-state")
            if emu.stop then emu.stop(1) end
            return
        end

        if frame >= 120 and frame <= 360 then
            local phase = frame % 30
            if phase < 10 then
                set_pad({ Start = true })
            elseif phase < 20 then
                set_pad({ A = true })
            else
                set_pad({})
            end
        elseif frame >= 390 and frame <= 540 then
            if (frame % 20) < 10 then
                set_pad({ B = true })
            else
                set_pad({})
            end
        elseif room >= 1 and room < TARGET_ROOM and (frame % 24) == 0 then
            set_pad({ R = true })
        else
            set_pad({})
        end

        if room12Frame == nil and room == TARGET_ROOM then
            room12Frame = frame
            log(string.format("ROOM12 frame=%d fc=%d", frame, fc))
        end

        if room12Frame ~= nil then
            local mon = rd8(ADDR_MONUMENT_DISPLAYED)
            local tick = rd8(ADDR_MONUMENT_TICK)
            local line = rd8(ADDR_MONUMENT_LINE)
            local ch = rd8(ADDR_MONUMENT_CHAR)
            log(string.format("f=%d fc=%d room=%d mon=%d tick=%d line=%d char=%d",
                frame, fc, room, mon or -1, tick or -1, line or -1, ch or -1))
            if room ~= TARGET_ROOM then
                log("ROOM_LEFT")
                if emu.stop then emu.stop(1) end
                return
            end
            if (frame - room12Frame) >= 300 then
                log("DONE")
                if emu.stop then emu.stop(0) end
                return
            end
        elseif frame % 60 == 0 then
            log(string.format("PRE frame=%d fc=%d room=%d", frame, fc, room))
        end

        if frame >= 2400 then
            log("TIMEOUT")
            if emu.stop then emu.stop(1) end
        end
    end)

    if not ok then
        write_result(table.concat(lines, "\n") .. "\nLUA_ERROR " .. tostring(err) .. "\n")
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)
