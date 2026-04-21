local cpu = emu.memType.cpu
local outPath = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_farcall_probe_result.txt"
local frame = 0
local start = nil
local changed = false
local function wr(t)
  local f=io.open(outPath,"w") if f then f:write(t) f:close() end
end
wr("RUNNING")
emu.addEventCallback(function()
  frame = frame + 1
  local lo = emu.read(0x7E1689, cpu)
  local hi = emu.read(0x7E168A, cpu)
  if lo == nil or hi == nil then wr("FAIL nil") if emu.stop then emu.stop(1) end return end
  local v = lo + hi*256
  if frame == 30 then start = v end
  if frame > 30 and start ~= nil and v ~= start then changed = true end
  if frame == 300 then
    if changed then wr("PASS scroll_changed start="..tostring(start).." cur="..tostring(v)) else wr("FAIL scroll_unchanged start="..tostring(start).." cur="..tostring(v)) end
    if emu.stop then emu.stop(changed and 0 or 1) end
  end
end, emu.eventType.endFrame)
