-- Copyright (c) 2012 by Robert G. Jakabosky <bobby@sharedrealm.com>
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

object "LAddrInfo" {
	include "laddrinfo.h",
	-- store the 'LAddrInfo' structure in the Lua userdata object.
	userdata_type = "embed",
	ffi_cdef[[
struct LAddrInfo {
	struct addrinfo  *_res;
	struct addrinfo  *_cur;
};

typedef struct LAddrInfo LAddrInfo;
]],
	constructor {
		c_method_call "eai_rc" "l_addrinfo_init_ip" { "const char *", "host", "const char *", "port?"},
	},
	constructor "ipv4" {
		c_method_call "eai_rc" "l_addrinfo_init_ipv4" { "const char *", "host", "const char *", "port?"},
	},
	constructor "ipv6" {
		c_method_call "eai_rc" "l_addrinfo_init_ipv6" { "const char *", "host", "const char *", "port?"},
	},
	constructor "full" {
		c_method_call "eai_rc" "l_addrinfo_init_full" { "const char *", "host", "const char *", "port?",
			"int", "ai_family?", "int", "ai_socktype?", "int", "ai_protocol?", "int", "ai_flags?" },
	},
	destructor {
		c_method_call "void" "l_addrinfo_cleanup" {},
	},
	method "first" {
		c_method_call "bool" "l_addrinfo_first" {},
	},
	method "get_addr" {
		c_method_call "int" "l_addrinfo_get_addr" { "LSockAddr *", "addr"},
	},
	method "get_canonname" {
		c_method_call "const char *" "l_addrinfo_get_canonname" {},
	},
	method "get_family" {
		c_method_call "int" "l_addrinfo_get_family" {},
	},
	method "get_socktype" {
		c_method_call "int" "l_addrinfo_get_socktype" {},
	},
	method "get_protocol" {
		c_method_call "int" "l_addrinfo_get_protocol" {},
	},
	method "next" {
		c_method_call "bool" "l_addrinfo_next" {},
	},
}

