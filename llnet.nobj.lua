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

-- make generated variable nicer.
set_variable_format "%s%d"

c_module "llnet" {
-- module settings.
use_globals = false,
hide_meta_info = false,
luajit_ffi = true,
luajit_ffi_load_cmodule = true,

c_source [[
#ifdef __WINDOWS__
static void lsock_init_winsock2(lua_State *L) {
	WSADATA wsaData;
	int err;

	err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(err != 0) {
		luaL_error(L, "winsock2 failed with error: %d\n", err);
	}
}
#else
#define lsock_init_winsock2(L)
#endif
]],
c_source "module_init_src" [[
	lsock_init_winsock2(L);
]],

c_function "socketpair" {
	var_in{"int", "type"},
	var_in{"int", "flags?"},
	var_out{"LSocketFD", "sock1" },
	var_out{"LSocketFD", "sock2" },
	var_out{"errno_rc", "rc"},
	c_source "pre" [[
	LSocketFD sv[2];
]],
	c_source[[
	${rc} = l_socket_pair(${type}, ${flags}, sv);
	if(${rc} == 0) {
		${sock1} = sv[0];
		${sock2} = sv[1];
	}
]],
},
subfiles {
"src/error.nobj.lua",
"src/eai_error.nobj.lua",
"src/protocols.nobj.lua",
"src/services.nobj.lua",
"src/lsockaddr.nobj.lua",
"src/laddrinfo.nobj.lua",
"src/socket_options.nobj.lua",
"src/lsocket.nobj.lua",

-- IO Buffer
"src/liobuffer.nobj.lua",
},

}

