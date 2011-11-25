
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

local keep_alive = false
local requests = -1
local concurrent = 1
local profile = false
local url
local family = 'inet'

local i=1
while i <= #arg do
	local p = arg[i]
	if p == '-k' then
		keep_alive = true
	elseif p == '-p' then
		profile = true
	elseif p == '-n' then
		i = i + 1
		requests = tonumber(arg[i])
	elseif p == '-c' then
		i = i + 1
		concurrent = tonumber(arg[i])
	elseif p == '-b' then
		i = i + 1
		p = arg[i]
		if backends[p] then
			backend = p
		else
			error("Invalid backend:" .. p)
		end
	else
		url = p
	end
	i = i + 1
end

assert(url, "missing <url>")
assert(requests > 0, "missing '-n num'")
assert(concurrent <= requests, "insane arguments")

--
-- Parse URL
--
local uri = require"handler.uri"
url = uri.parse(url)
local port = url.port or 80
local http_port = ''
if port ~= 80 then
	http_port = ':' .. tostring(port)
end
local host = url.host

--
-- Pre-make HTTP request.
--
local REQUEST =
  "GET " .. url.path .." HTTP/1.1\r\n" ..
	"Host: " .. url.host .. http_port .. "\r\n" ..
  "User-Agent: fake_http_client/0.1\r\n" ..
  "Connection: keep-alive\r\n\r\n"

print("using backend:", backend)
print(string.format("%d concurrent requests, %d total requests", concurrent, requests))

local sformat = string.format
local stdout = io.stdout
local sock = require("examples.sock_" .. backend)
local new_sock = sock.new

local epoller = require"examples.epoller"

local poll = epoller.new()

local READ_LEN = 2 * 1024

local started = 0
local connections = 0
local done = 0
local succeeded = 0
local failed = 0
local errored = 0
local clients = 0
local parsed = 0

local lhp = require 'http.parser'
local resp_parsed
local http_parser
local function create_parser()
	local parser
	parser = lhp.response({
	on_body = function(data)
		resp_parsed.body = data
	end,
	on_message_complete = function()
		resp_parsed.status = parser:status_code()
	end,
	})
	return parser
end
http_parser = create_parser()
local parsed_resps = setmetatable({},{
__index = function(tab, resp)
	resp_parsed = {}
	parsed = parsed + 1
	local parsed = http_parser:execute(resp)
	if parsed ~= #resp then
		local errno, err, errmsg = http_parser:error()
		resp_parsed.errno = errno
		resp_parsed.errmsg = errmsg
		-- need to re-create parser.
		http_parser = create_parser()
	end
	-- get keep alive flag.
	local keep_alive = http_parser:should_keep_alive()
	resp_parsed.keep_alive = keep_alive
	if not keep_alive then
		-- need to re-create parser.
		http_parser = create_parser()
	end
	rawset(tab, resp, resp_parsed)
	return resp_parsed
end
})

local new_client

local function sock_close(sock)
	clients = clients - 1
	assert(clients >= 0, "Can't close more clients then we create.")
	poll:del(sock)
	sock:close()
	if clients == 0 then
		-- we should be finished.
		poll:stop()
	end
end

local function next_request(sock)
	if started >= requests then
		sock.request_active = false
		sock_close(sock)
		return
	end
	started = started + 1
	sock.request_active = true
	sock:send(REQUEST)
end

local progress_units = 10
local checkpoint = math.floor(requests / progress_units)
local percent = 0
local function print_progress()
	percent = percent + progress_units
	stdout:write(sformat("progress: %i%% done\n", percent))
end

local function http_parse(sock)
	local data, err = sock:recv(1024)
	if data then
		done = done + 1
		if (done % checkpoint) == 0 then
			print_progress()
		end
		-- check resp.
		local resp = parsed_resps[data]
		if resp.status == 200 then
			succeeded = succeeded + 1
		else
			failed = failed + 1
		end
		-- check if we should close the connection.
		if not resp_parsed.keep_alive then
			sock_close(sock)
			-- create a new client if we are not done.
			if clients < concurrent then
				local need = requests - started
				if need > clients then
					new_client()
				end
			end
			return
		end
		-- send a new request.
		if sock.request_active then
			sock.request_active = false
		end
		next_request(sock)
	else
		if err ~= 'EAGAIN' then
			if sock.request_active then
				errored = errored + 1
				if err == 'CLOSED' then
					started = started - 1
					new_client()
				else
					print("socket read error with active request:", err)
				end
			end
			sock_close(sock)
		end
	end
end

local function connected_cb(sock)
	sock.on_io_event = http_parse
	poll:mod(sock, epoller.EPOLLIN)
	-- send first request
	return next_request(sock)
end

new_client = function ()
	local sock = assert(new_sock(family, 'stream'))
	connections = connections + 1
	sock:setblocking(false)
	assert(sock:setopt('socket', 'reuseaddr', 1))
	local stat, err = sock:connect(host, port)
	if not stat then
		if err == 'EINPROGRESS' then
			-- need to wait for connect.
			sock.on_io_event = connected_cb
		else
			error("Failed to connect to server: " .. err)
		end
		poll:add(sock, epoller.EPOLLOUT)
	else
		-- socket is connect
		sock.on_io_event = http_parse
		poll:add(sock, epoller.EPOLLIN)
		-- send first request
		next_request(sock)
	end
	-- register callback for read events.
	clients = clients + 1
	return sock
end

--
-- Create clients.
--

local zmq = require"zmq"

local timer = zmq.stopwatch_start()

for i=1,concurrent do
	new_client()
end

print()
if profile then
	local luatrace = require"luatrace"
	luatrace.tron()
	print("poll:start():", pcall(function()
		poll:start()
	end))
	luatrace.troff()
else
	poll:start()
end

local elapsed = timer:stop()
if elapsed == 0 then elapsed = 1 end

local throughput = done / (elapsed / 1000000)

print(sformat([[

finished in %i sec, %i millisec and %i microsec, %i req/s
requests: %i total, %i started, %i done, %i succeeded, %i failed, %i errored, %i parsed
connections: %i total, %i concurrent
]],
(elapsed / 1000000), (elapsed / 1000) % 1000, (elapsed % 1000), throughput,
requests, started, done, succeeded, failed, errored, parsed,
connections, concurrent
))

