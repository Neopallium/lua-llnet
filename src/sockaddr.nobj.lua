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

basetype "socklen_t"          "integer" "0"
basetype "sa_family_t"        "integer" "0"
basetype "sockaddr *"         "lightuserdata" "NULL"

object "LSockAddr" {
	include "lsockaddr.h",
	-- store the 'LSockAddr' structure in the Lua userdata object.
	userdata_type = "embed",
	c_source[[
typedef struct sockaddr sockaddr;
]],
	ffi_cdef[[
typedef struct sockaddr sockaddr;
typedef int socklen_t;
typedef int sa_family_t;

struct LSockAddr {
	struct sockaddr  *addr;
	socklen_t        addrlen;
};

typedef struct LSockAddr LSockAddr;

]],
	constructor {
		c_source "pre" [[
	LSockAddr addr;
	${this} = &addr;
]],
		ffi_source "ffi_pre" [[
	${this} = ffi.new("LSockAddr");
]],
		c_method_call "int" "l_sockaddr_init_ip_port" { "const char *", "ip", "int", "port" },
	},
	constructor "unix" {
		c_source "pre" [[
	LSockAddr addr;
	${this} = &addr;
]],
		ffi_source "ffi_pre" [[
	${this} = ffi.new("LSockAddr");
]],
		c_method_call "int" "l_sockaddr_init_unix" { "const char *", "unix" },
	},
	constructor "family" {
		c_source "pre" [[
	LSockAddr addr;
	${this} = &addr;
]],
		ffi_source "ffi_pre" [[
	${this} = ffi.new("LSockAddr");
]],
		c_method_call "int" "l_sockaddr_init_family" { "sa_family_t", "family" },
	},
	constructor "len" {
		c_source "pre" [[
	LSockAddr addr;
	${this} = &addr;
]],
		ffi_source "ffi_pre" [[
	${this} = ffi.new("LSockAddr");
]],
		c_method_call "int" "l_sockaddr_init_len" { "socklen_t", "len" },
	},
	destructor {
		c_method_call "void" "l_sockaddr_cleanup" {},
	},
	method "resize" {
		c_method_call "void" "l_sockaddr_resize" {"socklen_t", "addrlen"},
	},
	method "get_family" {
		c_method_call "sa_family_t" "l_sockaddr_get_family" {},
	},
	method "addr" {
		c_method_call "sockaddr *" "l_sockaddr_get_addr" {},
	},
	method "addrlen" {
		c_method_call "socklen_t" "l_sockaddr_get_addrlen" {},
	},
}

