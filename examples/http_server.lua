
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

local lhp = require 'http.parser'

local epoller = require"examples.epoller"

local socks = {}

local poll = epoller.new()

local function new_acceptor(host, port, family, cb)
	local sock = new_sock(family or 'inet', 'stream')
	sock:setblocking(false)
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

local parser_cache = {}
local parser_mt = {}
parser_mt.__index = parser_mt

function parser_mt:execute(data)
	return self.lhp:execute(data)
end
function parser_mt:error()
	return self.lhp:error()
end
function parser_mt:close()
	self.sock = nil
	parser_cache[#parser_cache + 1] = self
	self.lhp = lhp.request(self.cbs)
end
function parser_mt:on_headers_complete()
	local sock = self.sock
	sock:send(RESPONSE)
	if not self.lhp:should_keep_alive() then
		self:close()
		sock_close(sock)
	end
end

local function create_parser()
	local parser
	local count = #parser_cache
	if count > 0 then
		parser = parser_cache[count]
		parser_cache[count] = nil
		return parser
	end
	parser = setmetatable({}, parser_mt)
	local cbs = {}
	function cbs.on_headers_complete()
		return parser:on_headers_complete()
	end
	parser.cbs = cbs
	parser.lhp = lhp.request(cbs)
	return parser
end

local function http_parse(sock)
	local data, err = sock:recv(1024)
	if data then
		local parsed = sock.parser:execute(data)
		if parsed ~= #data then
			print('http parse error:', sock.parser:error())
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
	sock.parser = create_parser()
	sock.parser.sock = sock
	-- register callback for read events.
	sock.on_io_event = http_parse
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
	new_server("1080")
end

poll:start()

