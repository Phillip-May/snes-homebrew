local cpu=emu.memType.cpu
local sym=dofile('C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua')
local out='C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_smoke_probe2.txt'
local frame=0
local done=false
local function wr(t) local f=io.open(out,'w'); if f then f:write(t); f:close(); end end
local function r8(a) return emu.read(a,cpu) end
local function r16(a) local lo=r8(a); local hi=r8(a+1); if lo==nil or hi==nil then return nil end; return lo+hi*256 end
wr('RUNNING')
emu.addEventCallback(function()
 local ok,err=pcall(function()
  if done then return end
  frame=frame+1
  local fc=r16(sym.GLOBAL_FrameCount)
  local room=r16(sym.GLOBAL_ActiveLevel)
  if frame>=120 then
    wr('PASS frame='..frame..' fc='..tostring(fc)..' room='..tostring(room))
    done=true
    if emu.stop then emu.stop(0) end
  end
 end)
 if not ok then wr('FAIL '..tostring(err)); done=true; if emu.stop then emu.stop(1) end end
end, emu.eventType.endFrame)
