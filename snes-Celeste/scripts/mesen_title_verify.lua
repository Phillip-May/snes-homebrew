-- Mesen 2 Lua test: verify SNES title assets are actually loaded into VRAM.
-- This catches regressions where boot falls back to gameplay-room rendering
-- instead of the dedicated title renderer path (BG3 in mode 0).

local cpu = emu.memType.cpu
local vram = emu.memType.snesVideoRam
local projectRoot = os.getenv("SNES_CELESTE_ROOT") or "."
local buildDir = projectRoot .. "/build"
local outPath = buildDir .. "/mesen_title_verify_result.txt"
local checksums = dofile(buildDir .. "/title_screen_checksums.lua")

local sym = dofile(buildDir .. "/mesen_symbols.lua")
local ADDR_GLOBAL_ACTIVE_LEVEL = sym.GLOBAL_ActiveLevel
local ADDR_GLOBAL_FRAME_COUNT = sym.GLOBAL_FrameCount

-- Expected checksums are generated alongside title_screen_snes.h by
-- python/convert_title_screen_snes.py.
local EXPECTED_TITLE_TILE_S1 = checksums.tile_s1
local EXPECTED_TITLE_TILE_S2 = checksums.tile_s2
local EXPECTED_TITLE_TILE_BYTES = checksums.tile_bytes or 4096
local EXPECTED_TITLE_MAP_S1 = checksums.map_s1
local EXPECTED_TITLE_MAP_S2 = checksums.map_s2

local frame = 0
local done = false

local function write_result(text)
    local f = io.open(outPath, "w")
    if f then
        f:write(text)
        f:close()
    end
end

local function read_u8(addr, mem)
    return emu.read(addr, mem)
end

local function read_u16(addr, mem)
    local lo = read_u8(addr, mem)
    local hi = read_u8(addr + 1, mem)
    if lo == nil or hi == nil then return nil end
    return lo + hi * 256
end

local function checksum_vram(startAddr, length)
    local s1 = 0
    local s2 = 0
    for i = 0, length - 1 do
        local b = emu.read(startAddr + i, vram)
        if b == nil then return nil, nil end
        s1 = (s1 + b) % 4294967296
        s2 = (s2 + s1) % 4294967296
    end
    return s1, s2
end

write_result("RUNNING")
emu.log("[TITLE VERIFY] Starting")

emu.addEventCallback(function()
    local ok, err = pcall(function()
        if done then
            return
        end

        frame = frame + 1

        -- Keep title active for verification (no Start presses).
        if frame < 90 then
            return
        end

        local roomId = read_u16(ADDR_GLOBAL_ACTIVE_LEVEL, cpu)
        local frameCount = read_u16(ADDR_GLOBAL_FRAME_COUNT, cpu)
        if roomId == nil or frameCount == nil then
            write_result("FAIL nil-state-read")
            done = true
            if emu.stop then emu.stop(1) end
            return
        end

        local tileS1, tileS2 = checksum_vram(0x6000, EXPECTED_TITLE_TILE_BYTES)
        local mapS1, mapS2 = checksum_vram(0x4000, 2048)
        if tileS1 == nil or mapS1 == nil then
            write_result("FAIL nil-vram-read")
            done = true
            if emu.stop then emu.stop(1) end
            return
        end

        if tileS1 ~= EXPECTED_TITLE_TILE_S1 or tileS2 ~= EXPECTED_TITLE_TILE_S2 then
            write_result(string.format("FAIL title-tile-mismatch room=%d frame=%d got=(%u,%u) exp=(%u,%u)",
                roomId, frameCount, tileS1, tileS2, EXPECTED_TITLE_TILE_S1, EXPECTED_TITLE_TILE_S2))
            done = true
            if emu.stop then emu.stop(1) end
            return
        end

        if mapS1 ~= EXPECTED_TITLE_MAP_S1 or mapS2 ~= EXPECTED_TITLE_MAP_S2 then
            write_result(string.format("FAIL title-map-mismatch room=%d frame=%d got=(%u,%u) exp=(%u,%u)",
                roomId, frameCount, mapS1, mapS2, EXPECTED_TITLE_MAP_S1, EXPECTED_TITLE_MAP_S2))
            done = true
            if emu.stop then emu.stop(1) end
            return
        end

        write_result(string.format("PASS room=%d frame=%d tile=(%u,%u) map=(%u,%u)",
            roomId, frameCount, tileS1, tileS2, mapS1, mapS2))
        done = true
        if emu.stop then emu.stop(0) end
    end)

    if not ok then
        write_result("FAIL lua-error " .. tostring(err))
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)
