
local llnet = require"llnet"

local buf = llnet.LIOBuffer.new(10)

local function d(buf)
	print("buf =", buf, #buf)
end
local function test_fail(buf, name, ...)
	print("test for error:", name, ...)
	local rc = {pcall(buf[name], buf, ...)}
	if rc[1] then
		error("!!!!!!!!!!!!!! method call should have thrown error: " .. name)
	end
	print("  returned:", unpack(rc, 2))
end

d(buf)

buf:set_data("hello")
d(buf)

print("get_byte[0] =", buf:get_byte(0))
print("get_byte[1] =", buf:get_byte(1))
print("get_byte[2] =", buf:get_byte(2))
print("get_byte[3] =", buf:get_byte(3))
print("get_byte[4] =", buf:get_byte(4))

test_fail(buf, 'get_byte', 5)
test_fail(buf, 'get_byte', -1)
test_fail(buf, 'get_byte', 10)

print("get_bytes(0,5) =", buf:get_bytes(0,5))
print("get_bytes(0,0) =", buf:get_bytes(0,0))
print("get_bytes() =", buf:get_bytes())
test_fail(buf, 'get_bytes', -1, 0)
test_fail(buf, 'get_bytes', 0, -1)
test_fail(buf, 'get_bytes', 0, 100)

print("get_data(0,5) =", buf:get_data(0,5))
print("get_data(0,0) =", buf:get_data(0,0))
print("get_data() =", buf:get_data())
test_fail(buf, 'get_data', -1, 0)
test_fail(buf, 'get_data', 0, -1)
test_fail(buf, 'get_data', 0, 100)

d(buf)
buf:set_data("hello, world! 123456789012345678901234567890")
d(buf)

local buf2 = llnet.LIOBuffer.new(1)

print("buf2:copy_buffer(buf, 5, 10):")
buf2:copy_buffer(buf, 5, 10)
print("buffer sizes = ", #buf, #buf2)
d(buf2)

print("buf2:copy_buffer(buf):")
buf2:copy_buffer(buf)
print("buffer sizes = ", #buf, #buf2)
d(buf2)

test_fail(buf2, 'copy_buffer', buf, -1, 1)
test_fail(buf2, 'copy_buffer', buf, 0, -1)
test_fail(buf2, 'copy_buffer', buf, 0, 1000)

