-- Copyright (c) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

export_definitions {
-- address families
"AF_UNIX",
"AF_INET",
"AF_INET6",
"AF_IPX",
"AF_NETLINK",
"AF_PACKET",

-- socket types
"SOCK_STREAM",
"SOCK_DGRAM",
"SOCK_SEQPACKET",
"SOCK_RAW",
"SOCK_RDM",
-- flags
"SOCK_NONBLOCK",
"SOCK_CLOEXEC",

-- Shutdown how types.
"SHUT_RD",
"SHUT_WR",
"SHUT_RDWR",

-- IP_MTU_DISCOVER arguments.
"IP_PMTUDISC_DONT",   -- Never send DF frames.
"IP_PMTUDISC_WANT",   -- Use per route hints.
"IP_PMTUDISC_DO",     -- Always DF.
"IP_PMTUDISC_PROBE",  -- Ignore dst pmtu.

-- IPV6_MTU_DISCOVER arguments.
"IPV6_PMTUDISC_DONT",   -- Never send DF frames.
"IPV6_PMTUDISC_WANT",   -- Use per route hints.
"IPV6_PMTUDISC_DO",     -- Always DF.
"IPV6_PMTUDISC_PROBE",  -- Ignore dst pmtu.

-- Routing header options for IPv6.
"IPV6_RTHDR_LOOSE",  -- Hop doesn't need to be neighbour.
"IPV6_RTHDR_STRICT", -- Hop must be a neighbour.

"IPV6_RTHDR_TYPE_0", -- IPv6 Routing header type 0.

}

object "LSocketFD" {
	userdata_type = 'simple',
	sys_include "sys/socket.h",
	sys_include "netinet/in.h",
	sys_include "netinet/ip.h",
	sys_include "netinet/tcp.h",
	sys_include "netinet/udp.h",
	include "lsocket.h",
	ffi_type = "int",
	ffi_cdef[[
int l_socket_recv(LSocketFD sock, void *buf, size_t len, int flags);
]],
	constructor {
		c_call "LSocketFD" "l_socket_open"
			{ "int", "domain", "int", "type", "int", "protocol", "int", "flags" },
	},
	constructor "fd" {
		var_in{"int", "fd"},
		c_source"  ${this} = ${fd};",
		ffi_source"  ${this} = ${fd}",
	},
	destructor "close" {
		c_method_call "void" "l_socket_close_internal" {},
	},
	method "__tostring" {
		var_out{ "const char *", "str", },
		c_source "pre" [[
	char tmp[64];
]],
		c_source[[
	${str} = tmp;
	snprintf(tmp, 64, "LSocketFD: fd=%d\n", ${this});
]],
		ffi_source[[
	${str} = string.format("LSocketFD: fd=%i\n", ${this})
]],
	},
	method "shutdown" {
		c_method_call "int" "l_socket_shutdown" { "int", "how" },
	},
	method "fileno" {
		var_out{"int", "fd"},
		c_source"  ${fd} = ${this};",
		ffi_source"  ${fd} = ${this}",
	},
	method "set_nonblock" {
		c_method_call "errno_rc" "l_socket_set_nonblock" { "bool", "nonblock" },
	},
	method "connect" {
		c_method_call "errno_rc" "l_socket_connect" { "LSockAddr *", "addr" },
	},
	method "bind" {
		c_method_call "errno_rc" "l_socket_bind" { "LSockAddr *", "addr" },
	},
	method "listen" {
		c_method_call "errno_rc" "l_socket_listen" { "int", "backlog" },
	},
	method "accept" {
		var_out{"LSocketFD", "client"},
		c_method_call { "errno_rc", "rc"} "l_socket_accept" { "LSockAddr *", "peer?", "int", "flags?" },
		c_source[[
	${client} = ${rc};
]],
		ffi_source[[
	${client} = ${rc};
]],
	},
	method "send" {
		c_method_call "errno_rc" "l_socket_send"
			{ "const char *", "data", "size_t", "#data", "int", "flags?" },
	},
	ffi_source[[
local tmp_buf_len = 8192
local tmp_buf = ffi.new("char[?]", tmp_buf_len)
]],
	method "recv" {
		var_in{"size_t", "len"},
		var_in{"int", "flags?"},
		var_out{"char *", "data", has_length = true},
		var_out{"errno_rc", "rc"},
		c_source "pre" [[
#define BUF_LEN 8192
	char buf[BUF_LEN];
	int buf_len = BUF_LEN;
]],
		c_source[[
	if(buf_len > ${len}) { buf_len = ${len}; }
	${rc} = l_socket_recv(${this}, buf, buf_len, ${flags});
	/* ${rc} == 0, then socket is closed. */
	if(${rc} == 0) {
		lua_pushnil(L);
		lua_pushliteral(L, "CLOSED");
		return 2;
	}
	${data} = buf;
	${data_len} = ${rc};
]],
		ffi_source[[
	local buf_len = (tmp_buf_len < ${len}) and tmp_buf_len or ${len}
	${rc} = C.l_socket_recv(${this}, tmp_buf, buf_len, ${flags})
	-- ${rc} == 0, then socket is closed.
	if ${rc} == 0 then return nil, "CLOSED" end
	${data} = tmp_buf;
	${data_len} = ${rc};
]],
	},

	method "send_buf" {
		c_method_call "errno_rc" "l_socket_send"
			{ "const void *", "data", "size_t", "len", "int", "flags?" },
	},
	method "recv_buf" {
		var_in{"void *", "data"},
		var_in{"size_t", "len"},
		var_in{"int", "flags?"},
		var_out{"int", "data_len"},
		var_out{"errno_rc", "rc"},
		c_source[[
	${rc} = l_socket_recv(${this}, ${data}, ${len}, ${flags});
	/* ${rc} == 0, then socket is closed. */
	if(${rc} == 0) {
		lua_pushnil(L);
		lua_pushliteral(L, "CLOSED");
		return 2;
	}
	${data_len} = ${rc};
]],
		ffi_source[[
	${rc} = C.l_socket_recv(${this}, ${data}, ${len}, ${flags});
	-- ${rc} == 0, then socket is closed.
	if ${rc} == 0 then return nil, "CLOSED" end
	${data_len} = ${rc};
]],
	},
}

