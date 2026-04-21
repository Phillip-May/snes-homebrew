local cpu = emu.memType.cpu
local outTxt = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_verify_result.txt"
local shotTouch = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_verify_touch.png"
local shotComplete = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_verify_complete.png"
local shotRestart = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_verify_restart.png"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_MONUMENT_DISPLAYED = sym.GLOBAL_MonumentTextDisplayed
local ADDR_MONUMENT_TICK = sym.s_monumentTextTick
local ADDR_MONUMENT_LINE = sym.GLOBAL_MonumentCurLineNum
local ADDR_MONUMENT_CHAR = sym.GLOBAL_MonumentCurLineCharCount

local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2
local TOUCH_X = 60
local TOUCH_Y = 40
local OFF_X = 100
local OFF_Y = 80

local frame = 0
local done = false
local room12Frame = nil
local phase = "boot"
local phaseFrame = 0
local lastLine = 0xFF
local lastChar = 0xFF
local touchShotTaken = false
local completeShotTaken = false
local restartShotTaken = false
local lines = {}
local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a)
    local lo = rd8(a)
    local hi = rd8(a + 1)
    return lo + hi * 256
end
local function wr8(a, v) emu.write(a, v & 0xFF, cpu) end
local function wr16(a, v)
    wr8(a, v)
    wr8(a + 1, math.floor(v / 256))
end
local function clearPad()
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
end
local function setPad(tbl)
    clearPad()
    for k, v in pairs(tbl) do
        inputState[string.lower(k)] = v and true or false
    end
end
local function flush()
    local f = io.open(outTxt, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end
local function log(line)
    lines[#lines + 1] = line
    flush()
end
local function screenshot(path)
    local png = emu.takeScreenshot()
    local f = io.open(path, "wb")
    if f then
        f:write(png)
        f:close()
    end
end
local function fail(msg)
    log("FAIL " .. msg)
    done = true
    if emu.stop then emu.stop(1) end
end
local function succeed()
    log("DONE")
    done = true
    if emu.stop then emu.stop(0) end
end

if emu.setInput then
    emu.addEventCallback(function()
        local ok = pcall(function() emu.setInput(inputState, 0) end)
        if not ok then
            pcall(function() emu.setInput(inputState, 1) end)
        end
    end, emu.eventType.inputPolled)
end

log("START")

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1
    clearPad()

    if frame >= 120 and frame <= 360 and room12Frame == nil then
        local bootPhase = frame % 30
        if bootPhase < 10 then
            setPad({ Start = true })
        elseif bootPhase < 20 then
            setPad({ A = true })
        end
    end

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)
    local mon = rd8(ADDR_MONUMENT_DISPLAYED)
    local tick = rd8(ADDR_MONUMENT_TICK)
    local line = rd8(ADDR_MONUMENT_LINE)
    local ch = rd8(ADDR_MONUMENT_CHAR)

    if room == 12 and room12Frame == nil then
        room12Frame = frame
        phase = "touch"
        phaseFrame = 0
        log(string.format("ROOM12 frame=%d fc=%d", frame, fc))
    end

    if room12Frame ~= nil then
        phaseFrame = phaseFrame + 1
        if phase == "touch" then
            wr16(PLAYER_X, TOUCH_X)
            wr16(PLAYER_Y, TOUCH_Y)
            if mon ~= 1 then
                -- give the object a short settle window after room load
                if phaseFrame > 150 then
                    fail(string.format("monument never displayed fc=%d line=%d char=%d", fc, line, ch))
                    return
                end
            else
                if not touchShotTaken and (line > 0 or ch >= 8) then
                    screenshot(shotTouch)
                    touchShotTaken = true
                end
                if line == 3 then
                    log(string.format("TEXT_COMPLETE frame=%d fc=%d", frame, fc))
                    phase = "complete"
                    phaseFrame = 0
                end
            end
        elseif phase == "complete" then
            wr16(PLAYER_X, TOUCH_X)
            wr16(PLAYER_Y, TOUCH_Y)
            if phaseFrame >= 20 then
                if not completeShotTaken then
                    screenshot(shotComplete)
                    completeShotTaken = true
                end
                phase = "off"
                phaseFrame = 0
            end
        elseif phase == "off" then
            wr16(PLAYER_X, OFF_X)
            wr16(PLAYER_Y, OFF_Y)
            if mon == 0 and line == 0 and ch == 0 and tick == 0 then
                log(string.format("RESET_OK frame=%d fc=%d", frame, fc))
                phase = "restart"
                phaseFrame = 0
            elseif phaseFrame > 180 then
                fail(string.format("reset did not clear fc=%d mon=%d tick=%d line=%d char=%d", fc, mon, tick, line, ch))
                return
            end
        elseif phase == "restart" then
            wr16(PLAYER_X, TOUCH_X)
            wr16(PLAYER_Y, TOUCH_Y)
            if mon == 1 and line == 0 and ch >= 1 and ch <= 4 then
                if not restartShotTaken then
                    screenshot(shotRestart)
                    restartShotTaken = true
                end
                log(string.format("RESTART_OK frame=%d fc=%d line=%d char=%d tick=%d", frame, fc, line, ch, tick))
                succeed()
                return
            elseif line > 0 or ch > 4 then
                fail(string.format("restart advanced too far fc=%d mon=%d line=%d char=%d tick=%d", fc, mon, line, ch, tick))
                return
            elseif phaseFrame > 180 then
                fail(string.format("restart did not begin fc=%d mon=%d line=%d char=%d tick=%d", fc, mon, line, ch, tick))
                return
            end
        end
    end

    if line ~= lastLine or ch ~= lastChar or frame == 1 or (frame % 60) == 0 then
        log(string.format("f=%d fc=%d room=%d phase=%s px=%d py=%d mon=%d tick=%d line=%d char=%d",
            frame, fc, room, phase, rd16(PLAYER_X), rd16(PLAYER_Y), mon, tick, line, ch))
        lastLine = line
        lastChar = ch
    end

    if room12Frame ~= nil and room ~= 12 then
        fail(string.format("room changed to %d fc=%d", room, fc))
        return
    end

    if frame >= 2400 then
        fail("timeout")
    end
end, emu.eventType.endFrame)
