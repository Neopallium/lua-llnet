
local llnet = require"llnet"
local P = llnet.Protocols

local host, port = arg[1], arg[2], arg[3], arg[4], arg[5]

if #arg < 1 then
	print(arg[0], ": <host> [<port>]")
	return
end

local info = llnet.LAddrInfo(host, port)
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

