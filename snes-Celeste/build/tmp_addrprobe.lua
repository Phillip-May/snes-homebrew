local out='C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/tmp_addrprobe_result.txt'
local sm=emu.memType.snesMemory
local function r(a) return emu.read(a,sm) or -1 end
local function wr(t) local f=io.open(out,'w'); if f then f:write(t); f:close(); end end
emu.addEventCallback(function()
 local s='fffc='..r(0x00FFFC)..' fffd='..r(0x00FFFD)..' w0='..r(0x7E0000)..' w1='..r(0x7E0001)..' lo='..r(0x157D)
 wr(s)
 if emu.stop then emu.stop(0) end
end, emu.eventType.endFrame)