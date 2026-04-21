local out='C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/tmp_probe3_result.txt'
local wram=emu.memType.snesWorkRam
local frame=0
local function r8(a) return emu.read(a,wram) or 0 end
local function append(t) local f=io.open(out,'a'); if f then f:write(t..'\n'); f:close(); end end
if io.open(out,'r') then os.remove(out) end
emu.addEventCallback(function()
 frame=frame+1
 if frame==1 or frame==120 then
  append('f='..frame..' dbg='..r8(0x025f)..' apu0='..r8(0x0260)..' fcLo='..r8(0x157d)..' fcHi='..r8(0x157e)..' ready='..r8(0x157f))
 end
 if frame==120 then if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)