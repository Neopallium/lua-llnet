
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

local epoller = require"examples.epoller"

local socks = {}

local poll = epoller.new()

local function new_acceptor(host, port, family, cb)
	local sock = new_sock(family or 'inet', 'stream')
	sock:setblocking(false)
	assert(sock:setopt('socket', 'reuseaddr', 1))
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

local last_len = 0
local read_count = 0
local function tcp_read(sock)
	local data, err = sock:recv(1024)
	if data then
		local recv_bytes = (sock.recv_bytes or 0) + #data
		sock.recv_bytes = recv_bytes
		read_count = read_count + 1
		last_len = #data
		--sock:send(data)
		--print("read:", data)
		-- close after large packet.
		if recv_bytes > 1024 then
			sock_close(sock)
		end
	else
		if err ~= 'EAGAIN' then
			sock_close(sock)
		end
	end
end

local function new_client(server)
	local sock = server:accept()
	if not sock then return end
	sock:setblocking(false)
	tcp_read(sock)
	-- register callback for read events.
	sock.on_io_event = tcp_read
	poll:add(sock, epoller.EPOLLIN)
end

local function new_server(port)
	print("listen on:", port)
	new_acceptor('*', port, 'inet', new_client)
end

for i=1,#arg do
	new_server(arg[i])
end

if #arg == 0 then
	new_server("1040")
end

while true do
	if poll:step(1000) == 0 then
		print("stats:", read_count, last_len)
	end
end

