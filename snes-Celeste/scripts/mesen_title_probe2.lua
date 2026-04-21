local vram = emu.memType.snesVideoRam
local cgram = emu.memType.snesCgRam
local outPath = 'C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_title_probe2.txt'
local frame=0
local done=false
local function sum_region(mem,start,len)
  local s=0
  local nz=0
  for i=0,len-1 do local b=emu.read(start+i,mem); s=(s+b)%4294967296; if b~=0 then nz=nz+1 end end
  return s,nz
end
local function wr(t) local f=io.open(outPath,'w') if f then f:write(t) f:close() end end
wr('RUNNING')
emu.addEventCallback(function()
 if done then return end
 frame=frame+1
 if frame<120 then return end
 local addrs={0x1000,0x2000,0x4000,0x8000,0xA000,0xC000,0xE000,0xF800}
 local parts={}
 for _,a in ipairs(addrs) do local s,nz=sum_region(vram,a,512); table.insert(parts,string.format('v%04X=%u/%d',a,s,nz)) end
 local caddrs={0x00,0x10,0x20,0x40,0x60,0x80}
 for _,a in ipairs(caddrs) do local s,nz=sum_region(cgram,a,32); table.insert(parts,string.format('c%02X=%u/%d',a,s,nz)) end
 wr(table.concat(parts,' ')); done=true; if emu.stop then emu.stop(0) end
end, emu.eventType.endFrame)
