-- Mesen 2 Lua smoke test for the main SNES ROM.
-- Goal: prove we reached active gameplay logic, not just "emulator stayed open".

local cpu = emu.memType.cpu
local projectRoot = os.getenv("SNES_CELESTE_ROOT") or "."
local buildDir = projectRoot .. "/build"
local outPath = buildDir .. "/mesen_main_smoke_result.txt"
local maxFrames = 2400 -- ~40s at 60fps
local frame = 0
local done = false

local sym = dofile(buildDir .. "/mesen_symbols.lua")
local ADDR_GLOBAL_ACTIVE_LEVEL = sym.GLOBAL_ActiveLevel
local ADDR_GLOBAL_PLAYER_DATA = sym.GLOBAL_PlayerData
local ADDR_GLOBAL_FRAME_COUNT = sym.GLOBAL_FrameCount

local function write_result(text)
    local f = io.open(outPath, "w")
    if f then
        f:write(text)
        f:close()
    end
end

local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

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

local function read_u8(addr)
    return emu.read(addr, cpu)
end

local function read_u16(addr)
    local lo = read_u8(addr)
    local hi = read_u8(addr + 1)
    if lo == nil or hi == nil then return nil end
    return lo + hi * 256
end

local function read_s16(addr)
    local v = read_u16(addr)
    if v == nil then return nil end
    if v >= 0x8000 then return v - 0x10000 end
    return v
end

write_result("RUNNING")
emu.log("[MAIN SMOKE] Starting")

local startFrameCount = nil
local roomIdSeen = nil
local baseX = nil
local movedX = false

emu.addEventCallback(function()
    local ok, err = pcall(function()
        if done then
            return
        end

        frame = frame + 1

        -- Try multiple buttons to reliably exit title across input mappings.
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
        elseif frame == 600 then
            set_pad({ Right = true })
        elseif frame == 750 then
            set_pad({})
        elseif frame == 780 then
            set_pad({ Left = true })
        elseif frame == 930 then
            set_pad({})
        elseif frame == 960 then
            set_pad({ B = true })
        elseif frame == 990 then
            set_pad({})
        end

        local frameCount = read_u16(ADDR_GLOBAL_FRAME_COUNT)
        local roomId = read_u16(ADDR_GLOBAL_ACTIVE_LEVEL + 0)
        local playerX = read_s16(ADDR_GLOBAL_PLAYER_DATA + 0)
        local playerY = read_s16(ADDR_GLOBAL_PLAYER_DATA + 2)
        if frameCount == nil or roomId == nil or playerX == nil or playerY == nil then
            write_result("FAIL nil-state-read frame=" .. frame)
            done = true
            if emu.stop then emu.stop(1) end
            return
        end

        if frame == 1 then
            startFrameCount = frameCount
        end

        if roomId > 0 and roomId < 64 then
            roomIdSeen = roomId
        end

        if frame == 60 then
            baseX = playerX
        end
        if baseX ~= nil and math.abs(playerX - baseX) >= 2 then
            movedX = true
        end

        if (frame % 120) == 0 then
            write_result("RUNNING frame=" .. tostring(frame) ..
                " room=" .. tostring(roomId) ..
                " fc=" .. tostring(frameCount) ..
                " x=" .. tostring(playerX) ..
                " y=" .. tostring(playerY))
        end

        if frame >= 900 then
            local advanced = false
            if startFrameCount ~= nil then
                local delta = (frameCount - startFrameCount) % 65536
                advanced = (delta > 120)
            end
            if not advanced then
                write_result("FAIL framecount-stalled start=" .. tostring(startFrameCount) .. " cur=" .. tostring(frameCount))
                done = true
                if emu.stop then emu.stop(1) end
                return
            end
            -- This is a gameplay smoke test, not a boot/title smoke test.
            -- The SNES title path keeps currentRoomID at 0 until input starts room 1.
            if roomId ~= 1 then
                write_result("FAIL gameplay-not-reached room=" .. tostring(roomId))
                done = true
                if emu.stop then emu.stop(1) end
                return
            end
            -- Input mapping can vary by emulator profile; movement is best-effort only.
            if playerY < -64 or playerY > 256 then
                write_result("FAIL player-state-invalid x=" .. tostring(playerX) .. " y=" .. tostring(playerY))
                done = true
                if emu.stop then emu.stop(1) end
                return
            end

            write_result("PASS frame=" .. frame .. " room=" .. roomId .. " x=" .. playerX .. " y=" .. playerY .. " moved=" .. tostring(movedX))
            done = true
            emu.log("[MAIN SMOKE] PASS gameplay-reached")
            if emu.stop then emu.stop(0) end
            return
        end

        if frame >= maxFrames then
            write_result("FAIL timeout room=" .. tostring(roomId) .. " moved=" .. tostring(movedX))
            done = true
            if emu.stop then emu.stop(1) end
        end
    end)

    if not ok then
        write_result("FAIL lua-error " .. tostring(err))
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)
