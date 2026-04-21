local testlib = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/scripts/mesen_testlib.lua")
local cpu = emu.memType.cpu
local vram = emu.memType.snesVideoRam
local cgram = emu.memType.snesCgRam
local sym = testlib.load_symbols({
    "GLOBAL_ActiveLevel",
    "GLOBAL_FrameCount",
    "GLOBAL_PlayerData",
    "GLOBAL_MonumentTextDisplayed",
    "GLOBAL_MonumentCurLineNum",
    "GLOBAL_MonumentCurLineCharCount",
    "s_monumentTextTick",
    "GLOBAL_OBJList",
    "s_pico8GlyphRows",
    "s_bg1TextInkBits",
    "s_bg1TextCoverScratch",
    "s_bg1TextDmaSlotBytes",
    "s_bg1TextSlotDirtyBits",
    "s_textSlotDmaCount",
    "s_textSlotDmaTile",
    "s_textMapDmaBufs"
})

local outPath, write_result = testlib.make_result_writer("mesen_monument_verify_result.txt")
local shotPath = testlib.buildDir .. "/mesen_monument_verify.png"

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_MON_DISPLAYED = sym.GLOBAL_MonumentTextDisplayed
local ADDR_MON_LINE = sym.GLOBAL_MonumentCurLineNum
local ADDR_MON_CHAR = sym.GLOBAL_MonumentCurLineCharCount
local ADDR_MON_TICK = sym.s_monumentTextTick
local ADDR_OBJLIST = sym.GLOBAL_OBJList
local ADDR_GLYPH_ROWS = sym.s_pico8GlyphRows
local ADDR_TEXT_INK_BITS = sym.s_bg1TextInkBits
local ADDR_TEXT_COVER = sym.s_bg1TextCoverScratch
local ADDR_TEXT_DMA_SLOT_BYTES = sym.s_bg1TextDmaSlotBytes
local ADDR_TEXT_SLOT_DIRTY_BITS = sym.s_bg1TextSlotDirtyBits
local ADDR_TEXT_SLOT_DMA_COUNT = sym.s_textSlotDmaCount
local ADDR_TEXT_SLOT_DMA_TILE = sym.s_textSlotDmaTile
local ADDR_TEXT_MAP_DMA_BUFS = sym.s_textMapDmaBufs

local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2

local OBJ_SIZE = 25
local OBJ_LIST_SIZE = 29
local OBJ_TYPE_OFFSET = 9
local OBJ_MONUMENT = 70
local TARGET_ROOM = 12

local frame = 0
local roomHitFrame = nil
local monumentX = nil
local monumentY = nil
local lastChar = 0
local lastFrameCount = nil
local sameFrameCount = 0
local passPendingFrame = nil
local inputState = testlib.new_input_state()
local snapChar5GlyphRows = nil
local snapChar5Ink0 = nil
local snapChar5Cover0 = nil
local snapChar5Stage0 = nil
local snapChar5Stage1 = nil

local function rd8(a) return testlib.read_u8(a, cpu) or 0 end
local function rd16(a) return testlib.read_u16(a, cpu) or 0 end
local function wr8(a, v) emu.write(a, v & 0xFF, cpu) end
local function wr16(a, v)
    wr8(a, v)
    wr8(a + 1, math.floor(v / 256))
end
local function read_bytes(startAddr, length, memType)
    local out = {}
    for i = 0, length - 1 do
        local b = emu.read(startAddr + i, memType)
        if b == nil then
            return nil
        end
        out[#out + 1] = string.format("%02X", b)
    end
    return table.concat(out, "")
end
local function savePng(path)
    local png = emu.takeScreenshot()
    local f = io.open(path, "wb")
    if f then
        f:write(png)
        f:close()
    end
end
local function checksum_vram(startAddr, length)
    local s1 = 0
    local s2 = 0
    for i = 0, length - 1 do
        local b = emu.read(startAddr + i, vram)
        if b == nil then
            return nil, nil
        end
        s1 = (s1 + b) % 4294967296
        s2 = (s2 + s1) % 4294967296
    end
    return s1, s2
end
local function checksum_mem(startAddr, length, memType)
    local sum = 0
    local nz = 0
    for i = 0, length - 1 do
        local b = emu.read(startAddr + i, memType)
        if b == nil then
            return nil, nil
        end
        sum = (sum + b) % 4294967296
        if b ~= 0 then
            nz = nz + 1
        end
    end
    return sum, nz
end
local function read_hex_bytes(startAddr, length, memType)
    local out = {}
    for i = 0, length - 1 do
        local b = emu.read(startAddr + i, memType)
        if b == nil then
            return nil
        end
        out[#out + 1] = string.format("%02X", b)
    end
    return table.concat(out, "")
end
local function read_hex_words(startAddr, count, memType)
    local out = {}
    for i = 0, count - 1 do
        local lo = emu.read(startAddr + (i * 2), memType)
        local hi = emu.read(startAddr + (i * 2) + 1, memType)
        if lo == nil or hi == nil then
            return nil
        end
        out[#out + 1] = string.format("%02X%02X", hi, lo)
    end
    return table.concat(out, ",")
end
local function scan_ink_slots(startAddr, slotCount)
    local slots = {}
    for slot = 0, slotCount - 1 do
        local sum = 0
        for i = 0, 31 do
            local b = emu.read(startAddr + (slot * 32) + i, cpu)
            if b == nil then
                return nil
            end
            sum = sum + b
        end
        if sum ~= 0 then
            slots[#slots + 1] = string.format("%d:%d", slot, sum)
        end
    end
    return table.concat(slots, ",")
end
local function finish(ok, text)
    write_result(text)
    if emu.stop then
        emu.stop(ok and 0 or 1)
    end
end
local function findMonument()
    for i = 1, (OBJ_LIST_SIZE - 1) do
        local base = ADDR_OBJLIST + (i * OBJ_SIZE)
        if rd8(base + OBJ_TYPE_OFFSET) == OBJ_MONUMENT then
            return rd16(base + 0), rd16(base + 2)
        end
    end
    return nil, nil
end

testlib.install_input_callback(inputState)

emu.addEventCallback(function()
    if monumentX ~= nil and rd16(ADDR_ROOM) == TARGET_ROOM then
        wr16(PLAYER_X, monumentX + 4)
        wr16(PLAYER_Y, monumentY + 8)
    end
end, emu.eventType.inputPolled)

write_result("RUNNING")

emu.addEventCallback(function()
    frame = frame + 1
    testlib.set_pad(inputState, {})

    local room = rd16(ADDR_ROOM)
    local frameCount = rd16(ADDR_FRAME)
    local monDisplayed = rd8(ADDR_MON_DISPLAYED)
    local monLine = rd8(ADDR_MON_LINE)
    local monChar = rd8(ADDR_MON_CHAR)
    local monTick = rd8(ADDR_MON_TICK)

    if frame >= 120 and frame <= 360 then
        local bootPhase = frame % 30
        if bootPhase < 10 then
            inputState.start = true
        elseif bootPhase < 20 then
            inputState.a = true
        else
            inputState.b = true
        end
    elseif room >= 1 and room < TARGET_ROOM then
        local movePhase = (frame - 360) % 16
        if movePhase < 4 then
            inputState.r = true
        end
    end

    if lastFrameCount == frameCount then
        sameFrameCount = sameFrameCount + 1
    else
        sameFrameCount = 0
    end
    lastFrameCount = frameCount

    if room == TARGET_ROOM then
        if roomHitFrame == nil then
            roomHitFrame = frame
        end
        if monumentX == nil then
            monumentX, monumentY = findMonument()
        end
    end

    if room == TARGET_ROOM and monChar > lastChar then
        lastChar = monChar
        if monChar == 5 then
            snapChar5GlyphRows = read_bytes(ADDR_GLYPH_ROWS, 8, cpu)
            snapChar5Ink0 = read_bytes(ADDR_TEXT_INK_BITS, 64, cpu)
            snapChar5Cover0 = read_bytes(ADDR_TEXT_COVER, 64, cpu)
            snapChar5Stage0 = read_bytes(ADDR_TEXT_DMA_SLOT_BYTES, 64, cpu)
            snapChar5Stage1 = read_bytes(ADDR_TEXT_DMA_SLOT_BYTES + 64, 64, cpu)
            savePng(shotPath)
        end
    end

    if room == TARGET_ROOM and monLine >= 3 and passPendingFrame == nil then
        passPendingFrame = frame
    end

    if passPendingFrame ~= nil and (frame - passPendingFrame) >= 20 then
        local mapS1, mapS2 = checksum_vram(0x6000, 2048)
        local tileS1, tileS2 = checksum_vram(0xC000, 4096)
        local pal28 = read_bytes(28, 8, cgram)
        local pal56 = read_bytes(56, 8, cgram)
        local tile18 = read_hex_bytes(0xC120, 32, vram)
        local row11 = read_hex_words(0x6000 + (11 * 64) + (2 * 2), 8, vram)
        local glyphRows = read_bytes(ADDR_GLYPH_ROWS, 8, cpu)
        local ink0 = read_bytes(ADDR_TEXT_INK_BITS, 64, cpu)
        local cover0 = read_bytes(ADDR_TEXT_COVER, 64, cpu)
        local inkSum, inkNz = checksum_mem(ADDR_TEXT_INK_BITS, 320, cpu)
        local coverSum, coverNz = checksum_mem(ADDR_TEXT_COVER, 320, cpu)
        local inkSlots = scan_ink_slots(ADDR_TEXT_INK_BITS, 10)
        local tile18a = read_hex_bytes(0xA120, 32, vram)
        local tile22a = read_hex_bytes(0xA160, 32, vram)
        local tile22 = read_hex_bytes(0xC160, 32, vram)
        local tile24 = read_hex_bytes(0xC180, 32, vram)
        local slotDirty = read_bytes(ADDR_TEXT_SLOT_DIRTY_BITS, 7, cpu)
        local slotTile0 = testlib.read_u16(ADDR_TEXT_SLOT_DMA_TILE, cpu)
        local slotTile1 = testlib.read_u16(ADDR_TEXT_SLOT_DMA_TILE + 2, cpu)
        local stage0 = read_bytes(ADDR_TEXT_DMA_SLOT_BYTES, 64, cpu)
        local stage1 = read_bytes(ADDR_TEXT_DMA_SLOT_BYTES + 64, 64, cpu)
        local mapbuf0 = read_bytes(ADDR_TEXT_MAP_DMA_BUFS, 32, cpu)
        local slotDmaCount = rd8(ADDR_TEXT_SLOT_DMA_COUNT)
        savePng(shotPath)
        finish(true, string.format("PASS frame=%d fc=%d room=%d line=%d char=%d tick=%d map=(%s,%s) tile=(%s,%s) cgram28=%s cgram56=%s tile18=%s tile18a=%s tile22=%s tile22a=%s tile24=%s row11=%s glyphRows=%s ink0=%s cover0=%s inkSum=%s inkNz=%s coverSum=%s coverNz=%s inkSlots=%s slotDirty=%s slotTile0=%s slotTile1=%s snapChar5GlyphRows=%s snapChar5Ink0=%s snapChar5Cover0=%s slotDmaCount=%d stage0=%s stage1=%s snapChar5Stage0=%s snapChar5Stage1=%s mapbuf0=%s",
            frame, frameCount, room, monLine, monChar, monTick,
            tostring(mapS1), tostring(mapS2), tostring(tileS1), tostring(tileS2), tostring(pal28), tostring(pal56), tostring(tile18), tostring(tile18a), tostring(tile22), tostring(tile22a), tostring(tile24), tostring(row11), tostring(glyphRows), tostring(ink0), tostring(cover0), tostring(inkSum), tostring(inkNz), tostring(coverSum), tostring(coverNz), tostring(inkSlots), tostring(slotDirty), tostring(slotTile0), tostring(slotTile1), tostring(snapChar5GlyphRows), tostring(snapChar5Ink0), tostring(snapChar5Cover0), slotDmaCount, tostring(stage0), tostring(stage1), tostring(snapChar5Stage0), tostring(snapChar5Stage1), tostring(mapbuf0)))
        return
    end

    if roomHitFrame ~= nil and room ~= TARGET_ROOM then
        finish(false, string.format("FAIL left room frame=%d fc=%d room=%d", frame, frameCount, room))
        return
    end

    if roomHitFrame ~= nil and sameFrameCount >= 20 then
        finish(false, string.format("FAIL hang frame=%d fc=%d line=%d char=%d tick=%d", frame, frameCount, monLine, monChar, monTick))
        return
    end

    if frame >= 2400 then
        savePng(shotPath)
        finish(false, string.format("FAIL timeout frame=%d fc=%d room=%d line=%d char=%d tick=%d same=%d",
            frame, frameCount, room, monLine, monChar, monTick, sameFrameCount))
    end
end, emu.eventType.endFrame)
