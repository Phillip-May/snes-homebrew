-- NMI diagnostic: check if game loop is progressing
local cpu = emu.memType.snesMemory
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/nmi_diag_result.txt"
local frame = 0
local done = false

local ADDR_FRAME_COUNT   = 0x7E14F3  -- GLOBAL_FrameCount (uint16)
local ADDR_VBLANK_READY  = 0x7E14F6  -- s_vblankReady (bool)
local ADDR_TITLE_MODE    = 0x7E11B2  -- s_titleMode (bool)
local ADDR_IN_TITLE      = 0x7E0227  -- s_inTitleScreen (bool)
local ADDR_ROOM_ID       = 0x7E0283  -- GLOBAL_ActiveLevel.currentRoomID

local function wr(t)
    local f = io.open(out, "w")
    if f then f:write(t .. "\n"); f:close() end
end

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a) return rd8(a) + rd8(a+1) * 256 end

local log_lines = {}
local function log(t)
    table.insert(log_lines, t)
end

wr("RUNNING")

emu.addEventCallback(function()
    if done then return end
    frame = frame + 1

    local fc = rd16(ADDR_FRAME_COUNT)
    local ready = rd8(ADDR_VBLANK_READY)
    local titleMode = rd8(ADDR_TITLE_MODE)
    local inTitle = rd8(ADDR_IN_TITLE)
    local roomId = rd16(ADDR_ROOM_ID)

    if frame <= 10 or (frame % 60) == 0 or frame == 300 then
        log(string.format("f=%d fc=%d ready=%d title=%d inTitle=%d room=%d",
            frame, fc, ready, titleMode, inTitle, roomId))
    end

    if frame >= 600 then
        log("DONE at frame " .. frame)
        local result = table.concat(log_lines, "\n")
        wr(result)
        done = true
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)
