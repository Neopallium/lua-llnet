
local backends = {
llnet = true,
ffi = true,
nixio = true,
}
local backend = 'llnet'

local full_parse = true

-- check first command line argument for backend type
if backends[arg[1]] then
	backend = arg[1]
	table.remove(arg, 1)
end

print("using backend:", backend)

local sock = require("examples.sock_" .. backend)
local new_sock = sock.new
local sock_flags = sock.NONBLOCK + sock.CLOEXEC

local lhp = require 'http.parser'

local epoller = require"examples.epoller"

local poll = epoller.new()

local function new_acceptor(host, port, family, cb)
	local sock = new_sock(family or 'inet', 'stream', 0, sock_flags)
	assert(sock:setopt('socket', 'reuseaddr', 1))
	assert(sock:setopt('tcp', 'defer_accept', 60))
	assert(sock:bind(host, port))
	assert(sock:listen(30000))
	-- register callback for read events.
	sock.on_io_event = cb
	poll:add(sock, epoller.EPOLLIN)
	return sock
end

local need_stats = false
local clients = 0
local peak_clients = 0
local function sock_close(sock, keep_parser)
	clients = clients - 1
	assert(clients >= 0,"Number of clients can't be below zero.")
	if keep_parser then
		sock.parser:close()
	end
	poll:del(sock)
	sock:close()
	sock.closed = true
	if clients == 0 then
		need_stats = true
	end
end

local READ_LEN = 2 * 1024
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
	-- can't re-use active parser.
	if self.active then return end
	self.sock = nil
	self.body = nil
	self.url = nil
	self.method = nil
	self.major = nil
	self.minor = nil
	parser_cache[#parser_cache + 1] = self
	self.lhp:reset()
end
if full_parse then
	function parser_mt:on_message_begin()
		self.active = true
		self.headers = {}
	end
	function parser_mt:on_url(url)
		self.method = self.lhp:method()
		self.url = url
	end
	function parser_mt:on_header(header, val)
		self.headers[header] = val
	end
	function parser_mt:on_headers_complete()
		local lhp = self.lhp
		self.major, self.minor = lhp:version()
	end
	function parser_mt:on_body(data)
		local body = self.body
		if body then
			self.body = body .. data
		else
			self.body = data
		end
	end
	function parser_mt:on_message_complete()
		self.active = false
		local sock = self.sock
		sock:send(RESPONSE)
		if not self.lhp:should_keep_alive() then
			sock_close(sock, true)
		end
		self.body = nil
		self.url = nil
	end
else
	function parser_mt:on_message_begin()
		self.active = true
	end
	function parser_mt:on_message_complete()
		self.active = false
		local sock = self.sock
		sock:send(RESPONSE)
		if not self.lhp:should_keep_alive() then
			sock_close(sock, true)
		end
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
	function cbs.on_message_begin()
		return parser:on_message_begin()
	end
	if full_parse then
		function cbs.on_url(url)
			return parser:on_url(url)
		end
		function cbs.on_header(header, val)
			return parser:on_header(header, val)
		end
		function cbs.on_headers_complete()
			return parser:on_headers_complete()
		end
		function cbs.on_body(data)
			return parser:on_body(data)
		end
	end
	function cbs.on_message_complete()
		return parser:on_message_complete()
	end
	parser.cbs = cbs
	parser.lhp = lhp.request(cbs)
	return parser
end

local function http_parse(sock)
	local data, err = sock:recv(READ_LEN)
	if data then
		local parsed = sock.parser:execute(data)
		if parsed ~= #data then
			print('http parse error:', sock.parser:error())
			sock_close(sock, false)
			return false
		end
	else
		if err ~= 'EAGAIN' then
			sock_close(sock, true)
			return false
		end
	end
	return true
end

local MAX_ACCEPT = 100
local llnet = require"llnet"
local tmp_addr = llnet.LSockAddr()
local function new_client(server)
	for i=1,MAX_ACCEPT do
		local sock, err = server:accept(tmp_addr, sock_flags)
		if not sock then return end
		clients = clients + 1
		if clients > peak_clients then
			peak_clients = clients
		end
		sock.parser = create_parser()
		sock.parser.sock = sock
		-- register callback for read events.
		sock.on_io_event = http_parse
		-- read first request.
		if http_parse(sock) and not sock.closed then
			-- add socket to poller.
			poll:add(sock, epoller.EPOLLIN)
		end
	end
end

function print_stats()
	print("clients", clients, "peak_clients", peak_clients, "mem", collectgarbage"count", "parsers", #parser_cache)
	collectgarbage"collect"
	collectgarbage"collect"
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

repeat
	local rc = poll:step(1000) -- wait for events.
	if rc == 0 then
		if need_stats then
			print_stats()
			need_stats = false
		end
	end
until false

