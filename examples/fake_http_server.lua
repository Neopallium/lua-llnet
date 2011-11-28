
local backends = {
llnet = true,
ffi = true,
nixio = true,
}
local backend = 'llnet'

-- check first command line argument for backend type
if backends[arg[1]] then
	backend = arg[1]
	table.remove(arg, 1)
end

print("using backend:", backend)

local sock = require("examples.sock_" .. backend)
local new_sock = sock.new
local sock_flags = sock.NONBLOCK + sock.CLOEXEC

local epoller = require"examples.epoller"

local socks = {}

local poll = epoller.new()

local function new_acceptor(host, port, family, cb)
	local sock = new_sock(family or 'inet', 'stream', 0, sock_flags)
	assert(sock:setopt('socket', 'reuseaddr', 1))
	assert(sock:setopt('tcp', 'defer_accept', 1))
	assert(sock:bind(host, port))
	assert(sock:listen(30000))
	-- register callback for read events.
	sock.on_io_event = cb
	poll:add(sock, epoller.EPOLLIN)
	return sock
end

local function sock_close(sock)
	poll:del(sock)
	sock:close()
end

local RESPONSE =
  "HTTP/1.0 200 OK\r\n" ..
  "Content-Type: text/plain\r\n" ..
  "Content-Length: 13\r\n" ..
	"Connection: keep-alive\r\n" ..
  "\r\n" ..
  "Hello,world!\n"

local READ_LEN = 2 * 1024

local function http_parse(sock)
	local data, err = sock:recv(READ_LEN)
	if data then
		sock:send(RESPONSE)
	else
		if err ~= 'EAGAIN' then
			sock_close(sock)
			return false
		end
	end
	return true
end

local MAX_ACCEPT = 100
local function new_client(server)
	for i=1,MAX_ACCEPT do
		local sock, err = server:accept(nil, sock_flags)
		if not sock then return end
		-- register callback for read events.
		sock.on_io_event = http_parse
		-- read first request.
		if http_parse(sock) then
			-- add socket to poller.
			poll:add(sock, epoller.EPOLLIN)
		end
	end
end

local function new_server(port)
	print("listen on:", port)
	new_acceptor('*', port, 'inet', new_client)
end

for i=1,#arg do
	new_server(arg[i])
end

if #arg == 0 then
	new_server("1080")
end

--local luatrace = require"luatrace"
--luatrace.tron()
--print(pcall(function()
poll:start()
--end))
--luatrace.troff()

