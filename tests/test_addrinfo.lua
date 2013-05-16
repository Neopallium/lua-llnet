
local llnet = require"llnet"
local P = llnet.Protocols

local host, port = arg[1], arg[2]

if #arg < 2 then
	print(arg[0], ": <host> <port>")
	return
end

local info = assert(llnet.LAddrInfo(host, port))
local addr = llnet.LSockAddr()

if info:first() then
	repeat
		local canon = info:get_canonname()
		if canon then
			print("canon name:", canon)
		end
		info:get_addr(addr)
		print("addr:", addr)
		print("info:", info:get_family(), info:get_socktype(), P[info:get_protocol()])
	until not info:next()
end

