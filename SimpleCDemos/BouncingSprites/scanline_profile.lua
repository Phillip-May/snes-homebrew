-- Mesen 2 scanline profiler for BouncingSprites.
--
-- Measures, compiler-agnostically, how many scanlines the per-frame main-loop
-- work takes. The C main loop only reads HVBJOY ($4212) while spin-waiting for
-- vblank; it never touches $4212 during the OAM/gradient/physics work. So each
-- loop iteration produces exactly one large read-to-read gap == its work span
-- (from the read that detects vblank-start to the first post-work read). We
-- collect those gaps and report them in scanlines.
--
-- Notes from probing Mesen 2.1.0:
--  * emu.getState() returns a FLAT table keyed by dotted strings, e.g.
--    s["masterClock"] (monotonic master cycles) and s["frameCount"].
--  * different compilers read $4212 under different data banks, so the read
--    callback must cover every CPU-visible mirror ($00-$3F and $80-$BF).

local OUT   = os.getenv("PROFILE_OUT")   or "scanline_profile_result.txt"
local LABEL = os.getenv("PROFILE_LABEL") or "rom"

local CYC_PER_LINE  = 1364     -- master cycles per scanline (NTSC, 21477270 Hz)
local WARMUP_FRAMES = 45       -- skip initSNES + boot settling
local SAMPLE_COUNT  = 150      -- per-iteration work samples to collect
local TIMEOUT_FRAME = 2000
local GAP_THRESHOLD = 5.0      -- scanlines; spin reads are << 1 line apart

local samples   = {}
local startFrame = nil
local prevVbl, waitingEnd, workStart = nil, false, nil
local readCount, gFrame = 0, 0
local cbErr     = "untried"
local done, diagged = false, false

local function write_result(t) local f = io.open(OUT, "w"); if f then f:write(t); f:close() end end

local function getclock()
    local s = emu.getState()
    if not s then return nil, nil end
    return s["frameCount"], s["masterClock"]
end

local function finish(tag)
    table.sort(samples)
    local n = #samples
    local rpf = (gFrame > 0) and (readCount / gFrame) or 0
    if n == 0 then
        write_result(string.format("FAIL label=%s no-samples reads=%d cbErr=%s %s",
            LABEL, readCount, tostring(cbErr), tostring(tag)))
    else
        local sum = 0
        for _, v in ipairs(samples) do sum = sum + v end
        write_result(string.format(
            "DONE label=%s iters=%d min=%.1f median=%.1f avg=%.1f max=%.1f readsPerFrame=%.0f%s",
            LABEL, n, samples[1], samples[math.floor(n / 2) + 1], sum / n, samples[n], rpf,
            tag and (" " .. tag) or ""))
    end
    done = true
    if emu.stop then emu.stop(0) end
end

-- The callback receives (address, value). HVBJOY bit7 = vblank. The work begins
-- right after the read that sees vblank onset (bit7 clear -> set, i.e. L2 exit)
-- and ends at the very next read (no $4212 reads happen during the work). So we
-- only need getState() at those two boundaries (~2 reads/frame), not on every
-- spin read -- calling getState() on every read is too slow for ROMs that spin
-- thousands of times per frame.
local function onRead(addr, value)
    readCount = readCount + 1
    if done then return end
    local vbl = (value and value >= 0x80) and 1 or 0
    if waitingEnd then
        waitingEnd = false
        local fc, mc = getclock()
        if mc and workStart and startFrame and (fc - startFrame) >= WARMUP_FRAMES then
            local lines = (mc - workStart) / CYC_PER_LINE
            if lines >= GAP_THRESHOLD then
                samples[#samples + 1] = lines
                if #samples >= SAMPLE_COUNT then finish(nil); return end
            end
        end
    end
    if prevVbl == 0 and vbl == 1 then        -- vblank onset: work starts next
        local fc, mc = getclock()
        if startFrame == nil then startFrame = fc end
        workStart, waitingEnd = mc, true
    end
    prevVbl = vbl
end

write_result("RUNNING")
emu.log("[SCANLINE PROFILE] " .. LABEL .. " starting")

-- endFrame FIRST: guarantees timeout + a diagnostic snapshot.
emu.addEventCallback(function()
    gFrame = gFrame + 1
    if not diagged then
        diagged = true
        local fc, mc = getclock()
        write_result(string.format("DIAG label=%s fc=%s mc=%s cbErr=%s reads=%d",
            LABEL, tostring(fc), tostring(mc), tostring(cbErr), readCount))
    end
    if not done and gFrame > TIMEOUT_FRAME then finish("(timeout)") end
end, emu.eventType.endFrame)

-- $4212 is a CPU register mirrored across banks $00-$3F and $80-$BF; the data
-- bank used to read it varies per compiler, so cover them all.
local ok, err = pcall(function()
    for bank = 0x00, 0x3F do emu.addMemoryCallback(onRead, emu.callbackType.read, bank * 0x10000 + 0x4212) end
    for bank = 0x80, 0xBF do emu.addMemoryCallback(onRead, emu.callbackType.read, bank * 0x10000 + 0x4212) end
end)
cbErr = ok and "none" or tostring(err)
