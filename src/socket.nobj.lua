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

-- set/get sockopt levels.
"SOL_SOCKET",

-- Socket options.
"SO_DEBUG",
"SO_REUSEADDR",
"SO_TYPE",
"SO_ERROR",
"SO_DONTROUTE",
"SO_BROADCAST",
"SO_SNDBUF",
"SO_RCVBUF",
"SO_SNDBUFFORCE",
"SO_RCVBUFFORCE",
"SO_KEEPALIVE",
"SO_OOBINLINE",
"SO_NO",
"SO_PRIORITY",
"SO_LINGER",
"SO_BSDCOMPAT",

"SO_PASSCRED",
"SO_PEERCRED",
"SO_RCVLOWAT",
"SO_SNDLOWAT",
"SO_RCVTIMEO",
"SO_SNDTIMEO",

-- Security levels - as per NRL IPv6 - don't actually do anything
"SO_SECURITY_AUTHENTICATION",
"SO_SECURITY_ENCRYPTION_TRANSPORT",
"SO_SECURITY_ENCRYPTION_NETWORK",

"SO_BINDTODEVICE",

-- Socket filtering
"SO_ATTACH",
"SO_DETACH",

"SO_PEERNAME",
"SO_TIMESTAMP",

"SO_ACCEPTCONN",

"SO_PEERSEC",
"SO_PASSSEC",
"SO_TIMESTAMPNS",

"SO_MARK",

"SO_TIMESTAMPING",

"SO_PROTOCOL",
"SO_DOMAIN",

"SO_RXQ",

}
object "LSocketFD" {
	userdata_type = 'simple',
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
		c_source[[
	${this} = ${fd};
]],
		ffi_source[[
	${this} = ${fd}
]],
	},
	destructor "close" {
		c_method_call "void" "l_socket_close_internal" {},
	},
	method "shutdown" {
		c_method_call "int" "l_socket_shutdown" { "int", "how" },
	},
	method "fileno" {
		var_out{"int", "fd"},
		c_source[[
	${fd} = ${this};
]],
		ffi_source[[
	${fd} = ${this}
]],
	},
	method "set_nonblock" {
		c_method_call "int" "l_socket_set_nonblock" { "bool", "nonblock" },
	},
	method "set_option" {
		var_in{"int", "level"},
		var_in{"int", "opt"},
		var_in{"<any>", "value"},
		var_out{"int", "rc"},
		c_source "pre" [[
	int val_type;
]],
		c_source[[
	val_type = lua_type(L, ${value::idx});
	if(val_type == LUA_TSTRING) {
		size_t len;
		const char *val = lua_tolstring(L, ${value::idx}, &len);
		${rc} = l_socket_set_option(${this}, ${level}, ${opt}, val, len);
	} else if(val_type == LUA_TNUMBER) {
		int val = lua_tointeger(L, ${value::idx});
		${rc} = l_socket_set_option(${this}, ${level}, ${opt}, &val, sizeof(val));
	} else {
		return luaL_argerror(L, ${value::idx}, "expected string/number");
	}
]],
	},
	method "set_int_option" {
		c_method_call "int" "l_socket_set_int_option" { "int", "level", "int", "opt", "int", "val" },
	},
	method "get_int_option" {
		c_method_call "int" "l_socket_get_int_option" { "int", "level", "int", "opt", "int", "&val>1" },
	},
	method "connect" {
		c_method_call "int" "l_socket_connect" { "LSockAddr *", "addr" },
	},
	method "bind" {
		c_method_call "int" "l_socket_bind" { "LSockAddr *", "addr" },
	},
	method "listen" {
		c_method_call "int" "l_socket_listen" { "int", "backlog" },
	},
	method "accept" {
		c_method_call "LSocketFD" "l_socket_accept" { "LSockAddr *", "peer?", "int", "flags?" },
	},
	method "send" {
		c_method_call "int" "l_socket_send"
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
		c_source "pre" [[
#define BUF_LEN 8192
	char buf[BUF_LEN];
	int buf_len = BUF_LEN;
	int rc;
]],
		c_source[[
	if(buf_len > ${len}) { buf_len = ${len}; }
	rc = l_socket_recv(${this}, buf, buf_len, ${flags});
	if(rc <= 0) {
		/* rc == 0, then socket is closed. */
		if(rc == 0) { lua_pushnil(L); return 1; }
		/* return error. */
		lua_pushinteger(L, rc);
		return 1;
	}
	${data} = buf;
	${data_len} = rc;
]],
		ffi_source[[
	local buf_len = (tmp_buf_len < ${len}) and tmp_buf_len or ${len}
	local rc = C.l_socket_recv(${this}, tmp_buf, buf_len, ${flags})
	if rc <= 0 then
		-- rc == 0, then socket is closed.
		if rc == 0 then return nil end
		-- return error
		return rc
	end
	${data} = tmp_buf;
	${data_len} = rc;
]],
	},
}

