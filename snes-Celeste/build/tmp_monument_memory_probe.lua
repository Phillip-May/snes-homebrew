local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_memory_probe.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_MONUMENT_DISPLAYED = sym.GLOBAL_MonumentTextDisplayed
local ADDR_MONUMENT_LINE = sym.GLOBAL_MonumentCurLineNum
local ADDR_MONUMENT_CHAR = sym.GLOBAL_MonumentCurLineCharCount
local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2

local ADDR_BG1_INK = 0x7E150C
local ADDR_BG1_COVER = 0x7E093E
local ADDR_BG1_SLOT_CELL = 0x7E1BD6
local ADDR_BG1_CELL_SLOT = 0x7E1C08
local ADDR_MASK0 = 0x09EF21
local CELL = 0xA1 -- row10 col1

local frame = 0
local done = false
local lines = {}
local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a) return rd8(a) + rd8(a + 1) * 256 end
local function wr8(a, v) emu.write(a, v & 0xFF, cpu) end
local function wr16(a, v) wr8(a, v); wr8(a + 1, math.floor(v / 256)) end
local function clearPad() for k, _ in pairs(inputState) do inputState[k] = false end end
local function setPad(tbl)
  clearPad()
  for k, v in pairs(tbl) do inputState[string.lower(k)] = v and true or false end
end
local function hexbytes(base, count)
  local t = {}
  for i = 0, count - 1 do t[#t + 1] = string.format("%02X", rd8(base + i)) end
  return table.concat(t, " ")
end
local function log(s)
  lines[#lines + 1] = s
  local f = io.open(out, "w")
  if f then f:write(table.concat(lines, "\n") .. "\n"); f:close() end
end

if emu.setInput then
  emu.addEventCallback(function()
    local ok = pcall(function() emu.setInput(inputState, 0) end)
    if not ok then pcall(function() emu.setInput(inputState, 1) end) end
  end, emu.eventType.inputPolled)
end

emu.addEventCallback(function()
  if done then return end
  frame = frame + 1
  clearPad()
  if frame >= 120 and frame <= 360 and rd16(ADDR_ROOM) == 0 then
    local phase = frame % 30
    if phase < 10 then setPad({ Start = true })
    elseif phase < 20 then setPad({ A = true }) end
  end

  if rd16(ADDR_ROOM) == 12 then
    wr16(PLAYER_X, 60)
    wr16(PLAYER_Y, 40)
  end

  local mon = rd8(ADDR_MONUMENT_DISPLAYED)
  local line = rd8(ADDR_MONUMENT_LINE)
  local ch = rd8(ADDR_MONUMENT_CHAR)
  if rd16(ADDR_ROOM) == 12 and mon == 1 and line == 0 and ch >= 5 and frame >= 230 then
    local slot = rd8(ADDR_BG1_CELL_SLOT + CELL)
    if slot ~= 0xFF then
      log(string.format("frame=%d slot=%d char=%d", frame, slot, ch))
      log("rom=" .. hexbytes(ADDR_MASK0, 32))
      log("ink=" .. hexbytes(ADDR_BG1_INK + slot * 32, 32))
      log("cover=" .. hexbytes(ADDR_BG1_COVER + slot * 32, 32))
      for s = 0, 49 do
        local ink = hexbytes(ADDR_BG1_INK + s * 32, 32)
        if ink ~= string.rep("00 ", 31) .. "00" then
          log(string.format("slot%d cell=%d ink=%s", s, rd8(ADDR_BG1_SLOT_CELL + s), ink))
        end
      end
      done = true
      if emu.stop then emu.stop(0) end
    end
  end

  if frame > 1200 then
    log("timeout")
    done = true
    if emu.stop then emu.stop(1) end
  end
end, emu.eventType.endFrame)
