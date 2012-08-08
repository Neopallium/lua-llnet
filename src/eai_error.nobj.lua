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

-- EAI_* error values.
meta_object "EAI_Errors" {
	map_constants_bidirectional = true,
	export_definitions {
		-- Error values for `getaddrinfo' function.  */
		"EAI_BADFLAGS",    -- Invalid value for `ai_flags' field.  */
		"EAI_NONAME",      -- NAME or SERVICE is unknown.  */
		"EAI_AGAIN",       -- Temporary failure in name resolution.  */
		"EAI_FAIL",        -- Non-recoverable failure in name res.  */
		"EAI_FAMILY",      -- `ai_family' not supported.  */
		"EAI_SOCKTYPE",    -- `ai_socktype' not supported.  */
		"EAI_SERVICE",     -- SERVICE not supported for `ai_socktype'.  */
		"EAI_MEMORY",      -- Memory allocation failure.  */
		"EAI_SYSTEM",      -- System error returned in `errno'.  */
		"EAI_OVERFLOW",    -- Argument buffer overflow.  */
		-- __USE_GNU errors
		"EAI_NODATA",      -- No address associated with NAME.  */
		"EAI_ADDRFAMILY",  -- Address family for NAME not supported.  */
		"EAI_INPROGRESS",  -- Processing request in progress.  */
		"EAI_CANCELED",    -- Request canceled.  */
		"EAI_NOTCANCELED", -- Request not canceled.  */
		"EAI_ALLDONE",     -- All requests done.  */
		"EAI_INTR",        -- Interrupted by a signal.  */
		"EAI_IDN_ENCODE",  -- IDN encoding failed.  */
	},

	method "description" {
		var_in{ "<any>", "err" },
		var_out{ "const char *", "msg" },
		c_source "pre" [[
	int err_type;
	int err_num = -1;
]],
		c_source[[
	err_type = lua_type(L, ${err::idx});
	if(err_type == LUA_TSTRING) {
		lua_pushvalue(L, ${err::idx});
		lua_rawget(L, ${this::idx});
		if(lua_isnumber(L, -1)) {
			err_num = lua_tointeger(L, -1);
		}
		lua_pop(L, 1);
	} else if(err_type == LUA_TNUMBER) {
		err_num = lua_tointeger(L, ${err::idx});
	} else {
		return luaL_argerror(L, ${err::idx}, "expected string/number");
	}
	${msg} = gai_strerror(err_num);
]],
	},
}

-- Convert 'errno' codes into strings.
--
-- This is an error code wrapper object, it converts C-style 'int' return error code
-- into Lua-style 'nil, "Error message"' return values.
--
ffi_source "ffi_src" [[
-- get EAI_Errors table to map errno to error name.
local EAI_Error_names = _M.EAI_Errors
]]

c_source "extra_code" [[
static char llnet_EAI_Errors_key[] = "llnet_EAI_Errors_key";
]]

c_source "module_init_src" [[
	/* Cache reference to llnet.EAI_Errors table for errno->string convertion. */
	lua_pushlightuserdata(L, llnet_EAI_Errors_key);
	lua_getfield(L, -2, "EAI_Errors");
	lua_rawset(L, LUA_REGISTRYINDEX);
]]

error_code "eai_rc" "int" {
	ffi_type = "int",
	is_error_check = function(rec) return "(0 != ${" .. rec.name .. "})" end,
	ffi_is_error_check = function(rec) return "(0 ~= ${" .. rec.name .. "})" end,
	default = "0",
	c_source [[
	if(0 != err) {
		/* get EAI_Errors table. */
		lua_pushlightuserdata(L, llnet_EAI_Errors_key);
		lua_rawget(L, LUA_REGISTRYINDEX);
		/* convert error to string. */
		lua_rawgeti(L, -1, err);
		/* remove EAI_Errors table. */
		lua_remove(L, -2);
		if(!lua_isnil(L, -1)) {
			/* found error. */
			return;
		}
		/* Unknown error. */
		lua_pop(L, 1);
		err_str = "UNKNOWN ERROR";
	}
]],
	ffi_source [[
	if(0 ~= err) then
		err_str = EAI_Error_names[err]
	end
]],
}

