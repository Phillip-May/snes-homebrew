local out='C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/tmp_probe2_result.txt'
local sym=dofile('C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua')
local cpu=emu.memType.cpu
local wram=emu.memType.snesWorkRam
local frame=0
local function r16(mem,a) local lo=emu.read(a,mem); local hi=emu.read(a+1,mem); return (lo or 0)+(hi or 0)*256 end
local function append(t) local f=io.open(out,'a'); if f then f:write(t..'\n'); f:close(); end end
if io.open(out,'r') then os.remove(out) end
local a=sym.GLOBAL_FrameCount
local aw=a-0x7E0000
emu.addEventCallback(function()
 frame=frame+1
 if frame==120 then
  append('cpu='..r16(cpu,a)..' wram='..r16(wram,aw))
  if emu.stop then emu.stop(0) end
 end
end, emu.eventType.endFrame)