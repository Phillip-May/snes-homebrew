local out = 'C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_scroll_probe_result.txt'
local cpu = emu.memType.cpu
local lines = {}
local frame = 0
local function rd8(addr) return emu.read(addr, cpu) or 0 end
local function rd16(addr) local lo=rd8(addr); local hi=rd8(addr+1); return lo + hi*256 end
local function flush() local f=io.open(out,'w'); if f then f:write(table.concat(lines,'\n')..'\n'); f:close(); end end
emu.addEventCallback(function()
  frame = frame + 1
  if frame == 180 then
    table.insert(lines, string.format('frame=%u fc=%u title=%u room=%u start=%u bg2x=%u bg2y=%u bg3x=%u bg3y=%u bg4x=%u bg4y=%u playerY=%u scrollPointY=%u',
      frame, rd16(0x7E1488), rd8(0x7E11B2), rd8(0x7E0285), rd8(0x7E022F), rd16(0x7E026F), rd16(0x7E0271), rd16(0x7E0273), rd16(0x7E0275), rd16(0x7E0277), rd16(0x7E0279), rd16(0x7E1492), rd16(0x7E028F)))
    flush()
    if emu.stop then emu.stop(0) end
  end
end, emu.eventType.endFrame)
