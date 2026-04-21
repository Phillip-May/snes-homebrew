local vram = emu.memType.snesVideoRam
local cgram = emu.memType.snesCgRam
local outPath = 'C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_title_probe.txt'
local frame=0
local done=false
local function sum_region(mem,start,len)
  local s=0
  local nz=0
  for i=0,len-1 do
    local b=emu.read(start+i,mem)
    s=(s+b)%4294967296
    if b~=0 then nz=nz+1 end
  end
  return s,nz
end
local function wr(t)
  local f=io.open(outPath,'w') if f then f:write(t) f:close() end
end
wr('RUNNING')
emu.addEventCallback(function()
  if done then return end
  frame=frame+1
  if frame<120 then return end
  local s2,nz2=sum_region(vram,0x2000,1024)
  local s3,nz3=sum_region(vram,0x4000,1024)
  local p2,np2=sum_region(cgram,0x20,64)
  local p3,np3=sum_region(cgram,0x40,64)
  wr(string.format('frame=%d vram2_sum=%u vram2_nz=%d vram3_sum=%u vram3_nz=%d cgram2_sum=%u cgram2_nz=%d cgram3_sum=%u cgram3_nz=%d',frame,s2,nz2,s3,nz3,p2,np2,p3,np3))
  done=true
  if emu.stop then emu.stop(0) end
end, emu.eventType.endFrame)
