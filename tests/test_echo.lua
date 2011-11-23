local llnet = require"llnet"

local host = arg[1] or "127.0.0.1"
local port = arg[2] or 2007
local domain = llnet["AF_" .. (arg[3] or "inet"):upper()]
local sock_type = llnet["SOCK_" .. (arg[4] or "stream"):upper()]
local proto = llnet.Protocols[arg[4] or "tcp"]

if type(port) == 'string' then
	port = llnet.Services[port]
end

local server = llnet.LSocketFD(domain, sock_type, proto, llnet.SOCK_CLOEXEC)
local addr = llnet.LSockAddr(host, port)
assert(server:bind(addr) == 0)
assert(server:listen(10) == 0)

while true do
	local client = server:accept(nil, nil)
	print("got client:", client)
	print("client:send()", client:send("Hello\n"))
	while true do
		local data = client:recv(1024)
		if data == nil then break end
		if type(data) == 'number' then print("client:recv() error", data); break end
		client:send(data)
	end
	print("close client:", client:close())
	--break
end

