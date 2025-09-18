-- Print initial heap usage
print("Initial heap: " .. collectgarbage("count") .. " KB")

--local start = os.clock()
local start = 0
local N = 1000
local objects = {}

for i = 1, N do
    -- Allocate tables, strings, and closures
    objects[i] = {i, tostring(i), function() return i end}

    -- Occasionally free some to trigger GC
    if i % 100 == 0 then
        objects[i-500] = nil
        print("Heap at i=" .. i .. ": " .. collectgarbage("count") .. " KB")
    end
end

-- Force a full garbage collection
collectgarbage("collect")
print("Heap after GC: " .. collectgarbage("count") .. " KB")

local finish = 0
--local finish = os.clock()
print("Elapsed time: " .. (finish - start) .. " seconds")
