-- Deterministic Lua 5.5 stress kernels shared by the native and Wasmtime
-- backends. Every workload returns an exact integer checksum.
local M = {}
local workloads = {}
global <const> *
global __luarc_benchmark_global = 0

workloads[0] = function(iterations)
    local sum = 0
    for i = 1, iterations do
        sum = sum + i
    end
    return sum
end

workloads[1] = function(iterations)
    local size = 10000
    local values = {}
    for i = 1, size do
        values[i] = i * 2
    end
    local sum = 0
    for i = 1, iterations do
        local slot = ((i - 1) % size) + 1
        local value = values[slot]
        sum = sum + value
        values[slot] = value + 1
    end
    return sum
end

workloads[2] = function(iterations)
    local value = 0xDEADBEEF
    for _ = 1, iterations do
        value = ((value << 3) ~ (value >> 5)) & 0x7FFFFFFF
    end
    return value
end

-- Integrated lookup + call + literal-search case retained for historical
-- comparisons. The focused string.find cases below isolate those costs.
workloads[3] = function(iterations)
    local text = "the quick brown fox jumps over the lazy dog"
    local count = 0
    for _ = 1, iterations do
        if string.find(text, "fox") then
            count = count + 1
        end
    end
    return count
end

workloads[4] = function(iterations)
    local limit = 10000
    local rounds = math.max(1, iterations // limit)
    local is_prime = {}
    local checksum = 0
    for _ = 1, rounds do
        for i = 1, limit do is_prime[i] = true end
        is_prime[1] = false
        for i = 2, math.floor(math.sqrt(limit)) do
            if is_prime[i] then
                for composite = i * i, limit, i do
                    is_prime[composite] = false
                end
            end
        end
        local count = 0
        for i = 1, limit do
            if is_prime[i] then count = count + 1 end
        end
        checksum = checksum + count
    end
    return checksum
end

workloads[5] = function(iterations)
    local data = {}
    for i = 1, 10000 do data[i] = i * 3 end
    local found = 0
    for i = 1, iterations do
        local target = (i % 10000) * 3
        local low, high = 1, #data
        while low <= high do
            local middle = math.floor((low + high) / 2)
            local current = data[middle]
            if current == target then
                found = found + 1
                break
            elseif current < target then
                low = middle + 1
            else
                high = middle - 1
            end
        end
    end
    return found
end

workloads[6] = function(iterations)
    local node_count = 5000
    local head = nil
    for i = 1, node_count do
        head = { value = i, next = head }
    end
    local sum = 0
    local rounds = math.max(1, iterations // node_count)
    for _ = 1, rounds do
        local node = head
        while node do
            sum = sum + node.value
            node = node.next
        end
    end
    return sum
end

workloads[7] = function(iterations)
    local sum = 0
    for i = iterations, 1, -1 do
        sum = sum + (i & 7)
    end
    return sum
end

workloads[8] = function(iterations)
    local i, sum = 0, 0
    while i < iterations do
        i = i + 1
        sum = sum + i
    end
    return sum
end

workloads[9] = function(iterations)
    local i, sum = 0, 0
    repeat
        i = i + 1
        sum = sum + (i & 15)
    until i >= iterations
    return sum
end

workloads[10] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        if (i & 7) == 0 then
            checksum = checksum + 3
        elseif (i & 1) == 0 then
            checksum = checksum + 1
        else
            checksum = checksum - 1
        end
    end
    return checksum
end

workloads[11] = function(iterations)
    local value = 1.25
    for i = 1, iterations do
        value = value * 1.0000001 + (i % 7) * 0.125
        if value > 1000000.0 then value = value * 0.0001 end
    end
    local checksum = math.floor(value * 1024.0)
    return checksum
end

workloads[12] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        checksum = checksum + (i // 7) - (i % 13)
    end
    return checksum
end

workloads[13] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local a, b = i & 1023, (i * 3) & 1023
        if a < b then checksum = checksum + 1 end
        if a <= b then checksum = checksum + 2 end
        if a == b then checksum = checksum + 4 end
    end
    return checksum
end

local function add3(a, b, c)
    return a + b + c
end

workloads[14] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        checksum = checksum + add3(i & 31, 2, 3)
    end
    return checksum
end

local function tail_count(left, value)
    if left == 0 then return value end
    return tail_count(left - 1, value + left)
end

workloads[15] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        checksum = checksum + tail_count(16, i & 15)
    end
    return checksum
end

workloads[16] = function(iterations)
    local captured = 17
    local function read() return captured end
    local checksum = 0
    for _ = 1, iterations do checksum = checksum + read() end
    return checksum
end

workloads[17] = function(iterations)
    local captured = 0
    local function bump()
        captured = captured + 1
        return captured
    end
    local checksum = 0
    for _ = 1, iterations do checksum = checksum + (bump() & 31) end
    return checksum
end

workloads[18] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local value = i & 255
        local function read() return value end
        checksum = checksum + read()
    end
    collectgarbage("collect")
    return checksum
end

local function sum_varargs(...)
    return select("#", ...) + select(1, ...) + select(4, ...)
end

workloads[19] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        checksum = checksum + sum_varargs(i & 15, 2, 3, 4)
    end
    return checksum
end

local function split3(value)
    return value, value + 1, value + 2
end

workloads[20] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local a, b, c = split3(i & 31)
        checksum = checksum + a + b + c
    end
    return checksum
end

workloads[21] = function(iterations)
    local object = { value = 3 }
    function object:add(delta)
        self.value = self.value + delta
        return self.value
    end
    local checksum = 0
    for _ = 1, iterations do checksum = checksum + (object:add(1) & 31) end
    return checksum
end

workloads[22] = function(iterations)
    local size, values, keys = 1024, {}, {}
    for i = 1, size do
        local key = "key_" .. i
        keys[i], values[key] = key, i
    end
    local checksum = 0
    for i = 1, iterations do
        local key = keys[((i - 1) % size) + 1]
        checksum = checksum + values[key]
    end
    return checksum
end

workloads[23] = function(iterations)
    local size, values = 1024, {}
    for i = 1, size do values[i * 4099] = i end
    local checksum = 0
    for i = 1, iterations do
        checksum = checksum + values[(((i - 1) % size) + 1) * 4099]
    end
    return checksum
end

workloads[24] = function(iterations)
    local value = { x = 1, y = 2, z = 3 }
    local checksum = 0
    for _ = 1, iterations do
        checksum = checksum + value.x + value.y + value.z
        value.x = (value.x + 1) & 255
    end
    return checksum
end

workloads[25] = function(iterations)
    local values = {}
    for i = 1, 256 do values[i] = i end
    local checksum = 0
    for _ = 1, iterations do checksum = checksum + #values end
    return checksum
end

workloads[26] = function(iterations)
    local size, values = 128, {}
    for i = 1, size do values["p" .. i] = i end
    local rounds = math.max(1, iterations // size)
    local checksum = 0
    for _ = 1, rounds do
        for _, value in pairs(values) do checksum = checksum + value end
    end
    return checksum
end

workloads[27] = function(iterations)
    local size, values = 128, {}
    for i = 1, size do values[i] = i end
    local rounds = math.max(1, iterations // size)
    local checksum = 0
    for _ = 1, rounds do
        for _, value in ipairs(values) do checksum = checksum + value end
    end
    return checksum
end

workloads[28] = function(iterations)
    local values, checksum = {}, 0
    for i = 1, iterations do
        local slot = (i & 255) + 1
        rawset(values, slot, i)
        checksum = checksum + rawget(values, slot)
    end
    return checksum
end

workloads[29] = function(iterations)
    local insert, remove = table.insert, table.remove
    local values, checksum = {}, 0
    for i = 1, iterations do
        insert(values, i & 255)
        checksum = checksum + remove(values, 1)
    end
    return checksum
end

workloads[30] = function(iterations)
    local source, destination = {}, {}
    for i = 1, 32 do source[i] = i end
    local checksum = 0
    for _ = 1, iterations do
        table.move(source, 1, 32, 1, destination)
        checksum = checksum + destination[32]
    end
    return checksum
end

workloads[31] = function(iterations)
    local values = {}
    for i = 1, 128 do values[i] = (i * 73) % 127 end
    local checksum = 0
    for round = 1, iterations do
        values[1] = (round * 97) % 131
        table.sort(values)
        checksum = checksum + values[1] + values[128]
    end
    return checksum
end

workloads[32] = function(iterations)
    local find = string.find
    local text, checksum = "the quick brown fox jumps over the lazy dog", 0
    for _ = 1, iterations do
        if find(text, "fox") then checksum = checksum + 1 end
    end
    return checksum
end

workloads[33] = function(iterations)
    local find = string.find
    local text, checksum = "fox jumps over the lazy dog and runs away", 0
    for _ = 1, iterations do
        if find(text, "fox", 1, true) then checksum = checksum + 1 end
    end
    return checksum
end

workloads[34] = function(iterations)
    local find = string.find
    local text, checksum = "the quick brown fox jumps over the lazy dog", 0
    for _ = 1, iterations do
        if find(text, "fox", 1, true) then checksum = checksum + 1 end
    end
    return checksum
end

workloads[35] = function(iterations)
    local find = string.find
    local text, checksum = "the quick brown fox jumps over the lazy dog", 0
    for _ = 1, iterations do
        if find(text, "dog", 1, true) then checksum = checksum + 1 end
    end
    return checksum
end

workloads[36] = function(iterations)
    local find = string.find
    local text, checksum = "the quick brown fox jumps over the lazy dog", 0
    for _ = 1, iterations do
        if not find(text, "cat", 1, true) then checksum = checksum + 1 end
    end
    return checksum
end

workloads[37] = function(iterations)
    local find = string.find
    local text, checksum = "user_1234@example.com", 0
    for _ = 1, iterations do
        local first, last = find(text, "(%a+)_(%d+)@")
        checksum = checksum + (first or 0) + (last or 0)
    end
    return checksum
end

workloads[38] = function(iterations)
    local match = string.match
    local text, checksum = "name=Arthur;level=42", 0
    for _ = 1, iterations do
        local name, level = match(text, "name=(%a+);level=(%d+)")
        checksum = checksum + #name + #level
    end
    return checksum
end

workloads[39] = function(iterations)
    local sub = string.sub
    local text, checksum = "abcdefghijklmnopqrstuvwxyz0123456789", 0
    for i = 1, iterations do
        local first = (i % 16) + 1
        checksum = checksum + #sub(text, first, first + 7)
    end
    return checksum
end

workloads[40] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local value = "key:" .. (i & 1023)
        checksum = checksum + #value
    end
    return checksum
end

workloads[41] = function(iterations)
    local format, checksum = string.format, 0
    for i = 1, iterations do
        checksum = checksum + #format("id=%d", i & 1023)
    end
    return checksum
end

workloads[42] = function(iterations)
    local gsub, checksum = string.gsub, 0
    local text = "alpha-123-beta-456"
    for _ = 1, iterations do
        local result, count = gsub(text, "%d+", "#")
        checksum = checksum + #result + count
    end
    return checksum
end

workloads[43] = function(iterations)
    local byte, checksum = string.byte, 0
    local text = "abcdefghijklmnopqrstuvwxyz"
    for i = 1, iterations do
        checksum = checksum + byte(text, ((i - 1) % 26) + 1)
    end
    return checksum
end

workloads[44] = function(iterations)
    local lower, upper = string.lower, string.upper
    local checksum = 0
    for i = 1, iterations do
        local value = (i & 1) == 0 and lower("AbCdEf") or upper("AbCdEf")
        checksum = checksum + #value
    end
    return checksum
end

workloads[45] = function(iterations)
    local len, checksum = string.len, 0
    local value = "0123456789abcdef"
    for _ = 1, iterations do checksum = checksum + len(value) end
    return checksum
end

workloads[46] = function(iterations)
    local pack, unpack = string.pack, string.unpack
    local checksum = 0
    for i = 1, iterations do
        local encoded = pack("<i4I4", i & 0x7FFFFFFF, (i * 3) & 0xFFFFFFFF)
        local a, b = unpack("<i4I4", encoded)
        checksum = checksum + (a & 255) + (b & 255)
    end
    return checksum
end

workloads[47] = function(iterations)
    local abs, floor, ceil = math.abs, math.floor, math.ceil
    local checksum = 0
    for i = 1, iterations do
        local value = (i & 1) == 0 and i * 0.25 or -i * 0.25
        checksum = checksum + abs(floor(value)) + abs(ceil(value))
    end
    return checksum
end

workloads[48] = function(iterations)
    local sqrt, floor = math.sqrt, math.floor
    local checksum = 0
    for i = 1, iterations do
        checksum = checksum + floor(sqrt((i % 997) + 1) * 1024)
    end
    return checksum
end

workloads[49] = function(iterations)
    local tonumber, tostring = tonumber, tostring
    local checksum = 0
    for i = 1, iterations do
        local text = tostring((i & 1023) + 1000)
        checksum = checksum + tonumber(text) + #text
    end
    return checksum
end

workloads[50] = function(iterations)
    local fallback = { value = 7 }
    local object = setmetatable({}, { __index = fallback })
    local checksum = 0
    for _ = 1, iterations do checksum = checksum + object.value end
    return checksum
end

workloads[51] = function(iterations)
    local object = setmetatable({}, {
        __index = function(_, key) return #key end
    })
    local checksum = 0
    for _ = 1, iterations do checksum = checksum + object.missing end
    return checksum
end

workloads[52] = function(iterations)
    local sink = {}
    local object = setmetatable({}, {
        __newindex = function(_, key, value) sink[key] = value end
    })
    local checksum = 0
    for i = 1, iterations do
        object.value = i & 255
        checksum = checksum + sink.value
    end
    return checksum
end

workloads[53] = function(iterations)
    local mt = { __add = function(a, b) return a.value + b.value end }
    local a = setmetatable({ value = 3 }, mt)
    local b = setmetatable({ value = 5 }, mt)
    local checksum = 0
    for _ = 1, iterations do checksum = checksum + (a + b) end
    return checksum
end

workloads[54] = function(iterations)
    local object = setmetatable({}, { __len = function() return 11 end })
    local checksum = 0
    for _ = 1, iterations do checksum = checksum + #object end
    return checksum
end

workloads[55] = function(iterations)
    local object = setmetatable({ value = 9 }, {
        __call = function(self, delta) return self.value + delta end
    })
    local checksum = 0
    for i = 1, iterations do checksum = checksum + object(i & 7) end
    return checksum
end

workloads[56] = function(iterations)
    local function add(value)
        return value + 1
    end
    local checksum = 0
    for i = 1, iterations do
        local ok, value = pcall(add, i & 31)
        if ok then checksum = checksum + value end
    end
    return checksum
end

local function pcall_fail()
    error("benchmark failure", 0)
end

workloads[57] = function(iterations)
    local checksum = 0
    for _ = 1, iterations do
        local ok = pcall(pcall_fail)
        if not ok then checksum = checksum + 1 end
    end
    return checksum
end

workloads[58] = function(iterations)
    local function range(limit, index)
        index = index + 1
        if index <= limit then return index, index * 2 end
    end
    local width = 32
    local rounds = iterations // width
    if rounds < 1 then rounds = 1 end
    local checksum = 0
    for _ = 1, rounds do
        for key, value in range, width, 0 do
            checksum = checksum + key + value
        end
    end
    return checksum
end

workloads[59] = function(iterations)
    local codepoint, len = utf8.codepoint, utf8.len
    local text, checksum = "Lua: \xF0\x9F\x90\x8E \xE2\x98\x85", 0
    for _ = 1, iterations do
        checksum = checksum + codepoint(text, 6) + len(text)
    end
    return checksum
end

workloads[60] = function(iterations)
    local concat = table.concat
    local values = { "red", "dead", "redemption", "two" }
    local checksum = 0
    for _ = 1, iterations do checksum = checksum + #concat(values, ":") end
    return checksum
end

workloads[61] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local value = { i, i + 1, key = i & 255 }
        checksum = checksum + value[1] + value.key
    end
    collectgarbage("collect")
    return checksum
end

workloads[62] = function(iterations)
    _G.__benchmark_counter = 0
    for _ = 1, iterations do
        _G.__benchmark_counter = _G.__benchmark_counter + 1
    end
    local result = _G.__benchmark_counter
    _G.__benchmark_counter = nil
    return result
end

workloads[63] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local a = (i & 1) == 0
        local b = (i & 2) == 0
        if (a and not b) or (b and not a) then checksum = checksum + 1 end
    end
    return checksum
end

workloads[64] = function(iterations)
    local unpack = table.unpack
    local values = { 1, 2, 3, 4 }
    local checksum = 0
    for _ = 1, iterations do
        local a, b, c, d = unpack(values)
        checksum = checksum + a + b + c + d
    end
    return checksum
end

workloads[65] = function(iterations)
    local reverse, rep, char = string.reverse, string.rep, string.char
    local checksum = 0
    for i = 1, iterations do
        local value = reverse(rep(char(65 + (i % 26)), 4))
        checksum = checksum + #value + string.byte(value, 1)
    end
    return checksum
end

workloads[66] = function(iterations)
    local floor, checksum = math.floor, 0
    for i = 1, iterations do
        local left = (i & 31) + 1
        local right = (i & 3) + 1
        local quotient = left / right
        local power = left ^ right
        checksum = checksum + floor(quotient * 1024.0) + floor(power)
    end
    return checksum
end

workloads[67] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local value = (i & 255) + 1
        checksum = checksum + (value - 17) + (value ^ 2)
    end
    local result = math.floor(checksum)
    return result
end

workloads[68] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local value = i & 65535
        checksum = checksum + (value | 90) + (value ~ 165) + (3 << (i & 7))
    end
    return checksum
end

workloads[69] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local left = i & 65535
        local right = (i * 3) & 65535
        local shift = i & 7
        checksum = checksum + (left & right) + (left | right) +
            (left ~ right) + (left << shift) + (right >> shift)
    end
    return checksum
end

workloads[70] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local value = i & 65535
        checksum = checksum + ((~value) & 65535) + (-value)
        if not (value == 0) then checksum = checksum + 1 end
    end
    return checksum
end

workloads[71] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local enabled = (i & 1) == 0
        local value = enabled and (i & 31) or 7
        checksum = checksum + value
    end
    return checksum
end

workloads[72] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local value = i & 31
        if value <= 7 then checksum = checksum + 1 end
        if value > 7 then checksum = checksum + 2 end
        if value >= 15 then checksum = checksum + 4 end
        if value == 23 then checksum = checksum + 8 end
    end
    return checksum
end

workloads[73] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local value = (i & 1) == 0 and "horse" or "rider"
        if value == "horse" then checksum = checksum + 1 end
        if value ~= "rider" then checksum = checksum + 2 end
    end
    return checksum
end

workloads[74] = function(iterations)
    local checksum = 0.0
    for value = 1.0, iterations, 1.0 do
        checksum = checksum + value
    end
    local result = math.floor(checksum)
    return result
end

workloads[75] = function(iterations)
    for i = 1, iterations do
        __luarc_benchmark_global = i
    end
    local result = __luarc_benchmark_global
    __luarc_benchmark_global = 0
    return result
end

workloads[76] = function(iterations)
    local closed = 0
    local metatable = {
        __close = function()
            closed = closed + 1
        end
    }
    for _ = 1, iterations do
        local value <close> = setmetatable({}, metatable)
    end
    return closed
end

workloads[77] = function(iterations)
    local function sum(...values)
        return values[1] + values[2] + values.n
    end
    local checksum = 0
    for i = 1, iterations do
        checksum = checksum + sum(i & 15, 3, 5)
    end
    return checksum
end

workloads[78] = function(iterations)
    local checksum = 0
    local first = "abcdefghijklmnopqrstuvwxyz0123456789"
    local second = "abcdefghijklmnopqrstuvwxyz0123456789"
    for _ = 1, iterations do
        if first == second then checksum = checksum + 1 end
        if first <= second then checksum = checksum + 2 end
    end
    return checksum
end

workloads[79] = function(iterations)
    local checksum = 0
    local gmatch = string.gmatch
    for _ = 1, iterations do
        for word in gmatch("red dead redemption two", "%a+") do
            checksum = checksum + #word
        end
    end
    return checksum
end

workloads[80] = function(iterations)
    local min, max = math.min, math.max
    local checksum = 0
    for i = 1, iterations do
        checksum = checksum + min(i & 31, 9, 17) + max(i & 31, 9, 17)
    end
    return checksum
end

workloads[81] = function(iterations)
    local sin, cos, floor = math.sin, math.cos, math.floor
    local checksum = 0
    for i = 1, iterations do
        local angle = (i & 255) * 0.01
        checksum = checksum + floor((sin(angle) + cos(angle) + 2.0) * 1024.0)
    end
    return checksum
end

workloads[82] = function(iterations)
    local pack = table.pack
    local checksum = 0
    for i = 1, iterations do
        local values = pack(i & 31, 2, 3, 4)
        checksum = checksum + values[1] + values.n
    end
    return checksum
end

workloads[83] = function(iterations)
    local create, resume, yield = coroutine.create, coroutine.resume, coroutine.yield
    local function producer(count)
        for i = 1, count do yield(i) end
        return count + 1
    end
    local rounds = (iterations // 16) + 1
    local checksum = 0
    for _ = 1, rounds do
        local thread = create(producer)
        for _ = 1, 17 do
            local ok, value = resume(thread, 16)
            if ok and value then checksum = checksum + value end
        end
    end
    return checksum
end

workloads[84] = function(iterations)
    local function fail(value) error(value, 0) end
    local function handler(value) return value + 7 end
    local checksum = 0
    for i = 1, iterations do
        local ok, value = xpcall(fail, handler, i & 31)
        if not ok then checksum = checksum + value end
    end
    return checksum
end

workloads[85] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local value = i & 255
        checksum = checksum + tonumber(string.format("%x", value), 16)
    end
    return checksum
end

workloads[86] = function(iterations)
    local codes, checksum = utf8.codes, 0
    local text = "A\xC3\xA9\xF0\x9F\x90\x8EZ"
    for _ = 1, iterations do
        for position, codepoint in codes(text) do
            checksum = checksum + position + codepoint
        end
    end
    return checksum
end

workloads[87] = function(iterations)
    local offset, checksum = utf8.offset, 0
    local text = "A\xC3\xA9\xF0\x9F\x90\x8EZ"
    for _ = 1, iterations do
        checksum = checksum + offset(text, 3) + offset(text, -2)
    end
    return checksum
end

workloads[88] = function(iterations)
    local sort = table.sort
    local function descending(left, right) return left > right end
    local checksum = 0
    for round = 1, iterations do
        local values = { 7, 1, 9, 3, 5, round & 15 }
        sort(values, descending)
        checksum = checksum + values[1] + values[6]
    end
    return checksum
end

workloads[89] = function(iterations)
    local gsub, checksum = string.gsub, 0
    local function replace(digits) return "[" .. digits .. "]" end
    for _ = 1, iterations do
        local result, count = gsub("a12b345c", "%d+", replace)
        checksum = checksum + #result + count
    end
    return checksum
end

workloads[90] = function(iterations)
    local metatable = {
        __lt = function(left, right) return left.value < right.value end,
        __le = function(left, right) return left.value <= right.value end
    }
    local left = setmetatable({ value = 1 }, metatable)
    local right = setmetatable({ value = 2 }, metatable)
    local checksum = 0
    for _ = 1, iterations do
        if left < right then checksum = checksum + 1 end
        if left <= right then checksum = checksum + 2 end
    end
    return checksum
end

workloads[91] = function(iterations)
    local metatable = {
        __concat = function(left, right) return left.value .. right.value end
    }
    local left = setmetatable({ value = "red" }, metatable)
    local right = setmetatable({ value = "dead" }, metatable)
    local checksum = 0
    for _ = 1, iterations do checksum = checksum + #(left .. right) end
    return checksum
end

workloads[92] = function(iterations)
    local values = {}
    for i = 1, 2048 do values["key_" .. i] = i end
    local checksum = 0
    for i = 1, iterations do
        checksum = checksum + values["key_" .. ((i & 2047) + 1)]
    end
    return checksum
end

workloads[93] = function(iterations)
    local find, checksum = string.find, 0
    local text = string.rep("abcdefgh", 128) .. "TARGET"
    for _ = 1, iterations do
        local position = find(text, "TARGET", 1, true)
        checksum = checksum + position
    end
    return checksum
end

workloads[94] = function(iterations)
    local checksum = 0
    for i = 1, iterations do
        local value = string.rep(string.char(65 + (i & 15)), 32)
        checksum = checksum + #value + string.byte(value, 1)
    end
    collectgarbage("collect")
    return checksum
end

workloads[95] = function(iterations)
    local function values(seed) return seed, seed + 1, seed + 2 end
    local function forward(seed) return values(seed) end
    local checksum = 0
    for i = 1, iterations do
        local a, b, c = forward(i & 31)
        checksum = checksum + a + b + c
    end
    return checksum
end

local function false_constant()
    return not not nil
end

workloads[96] = function(iterations)
    local values = { false, true }
    local checksum = false_constant() and 1 or 0
    for i = 1, iterations do
        local inverse = not values[(i & 1) + 1]
        if inverse then checksum = checksum + 1 end
    end
    return checksum
end

workloads[97] = function(iterations)
    local floor, checksum = math.floor, 0
    for i = 1, iterations do
        local value = (i & 255) - 17.5
        checksum = checksum + floor(value)
    end
    return checksum
end

workloads[98] = function(iterations)
    local samples = { "in\x80valid", "\xBFstart", "ab\xFF" }
    local function consume(text)
        local checksum = 0
        for position, codepoint in utf8.codes(text) do
            checksum = checksum + position + codepoint
        end
        return checksum
    end
    local checksum = 0
    for i = 1, iterations do
        local ok = pcall(consume, samples[((i - 1) % 3) + 1])
        if not ok then checksum = checksum + (i & 7) + 1 end
    end
    return checksum
end

workloads[99] = function(iterations)
    local codes, checksum = utf8.codes, 0
    local text = utf8.char(0x4000000, 0x7FFFFFFF)
    for _ = 1, iterations do
        for position, codepoint in codes(text, true) do
            checksum = checksum + position + (codepoint & 0xFFFF)
        end
    end
    return checksum
end

function M.run(workload, iterations, seed)
    local kernel = workloads[workload]
    if not kernel then return -1 end
    return kernel(iterations, seed)
end

return M
