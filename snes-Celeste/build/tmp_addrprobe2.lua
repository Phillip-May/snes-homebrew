local out='C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/tmp_addrprobe2_result.txt'
local sm=emu.memType.snesMemory
local function r(a) return emu.read(a,sm) or -1 end
local function wr(t) local f=io.open(out,'w'); if f then f:write(t); f:close(); end end
emu.addEventCallback(function()
 local s='25f='..r(0x7E025F)..' 45f='..r(0x7E045F)..' 65f='..r(0x7E065F)..' 157d='..r(0x7E157D)..' 177d='..r(0x7E177D)..' 197d='..r(0x7E197D)
 wr(s)
 if emu.stop then emu.stop(0) end
end, emu.eventType.endFrame)