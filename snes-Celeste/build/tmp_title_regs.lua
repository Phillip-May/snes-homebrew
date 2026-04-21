local out = 'C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_probe_result.txt'
local lines = {}
local function w(addr) return emu.readWord(addr, emu.memType.workRam) end
local function b(addr) return emu.read(addr, emu.memType.workRam) end
local frame=0
local syms = dofile('C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua')
local WRAM=syms.WRAM_BASE
local fc = syms.GLOBAL_FrameCount - WRAM
local lvl = syms.GLOBAL_ActiveLevel - WRAM
local function flush()
  local f=io.open(out,'w') if f then f:write(table.concat(lines,'\n')..'\n') f:close() end
end
emu.addEventCallback(function()
  frame=frame+1
  if frame==180 then
    table.insert(lines, string.format('fc=%u room=%u scrollY=%u bg2y=%u bg3x=%u bg3y=%u bg4y=%u inidisp=%02X tm=%02X bgmode=%02X bg3sc=%02X bg34nba=%02X',
      w(fc), b(lvl+0), w(0x23A), b(0x210F), b(0x2110), b(0x2111), b(0x2114), emu.read(0x2100, emu.memType.cpu), emu.read(0x212C, emu.memType.cpu), emu.read(0x2105, emu.memType.cpu), emu.read(0x2109, emu.memType.cpu), emu.read(0x210C, emu.memType.cpu)))
    flush()
    emu.stop(0)
  end
end, emu.eventType.endFrame)
