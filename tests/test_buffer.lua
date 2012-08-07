
local llnet = require"llnet"

local buf = llnet.LIOBuffer.new(10)

local function d(buf)
	print("buf =", buf, #buf)
end
local function t(buf, name, ...)
	print("test method:", name, pcall(buf[name], buf, ...))
end

d(buf)

buf:set_data("hello")
d(buf)

print("get_byte[0] =", buf:get_byte(0))
print("get_byte[1] =", buf:get_byte(1))
print("get_byte[2] =", buf:get_byte(2))
print("get_byte[3] =", buf:get_byte(3))
print("get_byte[4] =", buf:get_byte(4))

t(buf, 'get_byte', 5)
t(buf, 'get_byte', -1)
t(buf, 'get_byte', 10)

print("get_bytes(0,5) =", buf:get_bytes(0,5))
print("get_bytes(0,0) =", buf:get_bytes(0,0))
print("get_bytes() =", buf:get_bytes())
t(buf, 'get_bytes', -1, 0)
t(buf, 'get_bytes', 0, -1)
t(buf, 'get_bytes', 0, 100)

print("get_data(0,5) =", buf:get_data(0,5))
print("get_data(0,0) =", buf:get_data(0,0))
print("get_data() =", buf:get_data())
t(buf, 'get_data', -1, 0)
t(buf, 'get_data', 0, -1)
t(buf, 'get_data', 0, 100)

d(buf)
buf:set_data("hello, world! 123456789012345678901234567890")
d(buf)

