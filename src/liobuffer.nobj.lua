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

object "LIOBuffer" {
	-- store the `LIOBuffer` structure in the Lua userdata object.
	userdata_type = "embed",
	implements "Buffer" {
		implement_method "const_data" {
			get_field = "buf"
		},
		implement_method "get_size" {
			get_field = "size"
		},
	},
	implements "MutableBuffer" {
		implement_method "data" {
			get_field = "buf"
		},
		implement_method "get_size" {
			get_field = "capacity"
		},
	},

	include "liobuffer.h",
	ffi_cdef[[
typedef struct LIOBuffer LIOBuffer;

struct LIOBuffer {
	uint8_t *buf;     /**< buffer. */
	size_t  size;     /**< size of data in buffer. */
	size_t  capacity; /**< memory size of buffer. */
};

void l_iobuffer_init(LIOBuffer *buf, const uint8_t *data, size_t size);

void l_iobuffer_init_cap(LIOBuffer *buf, size_t capacity);

bool l_iobuffer_set_capacity(LIOBuffer *buf, size_t capacity);

]],
	ffi_source[[
local LIOBuffer_tmp = ffi.new("LIOBuffer")
]],
	constructor {
		var_in{ "<any>", "size_or_data"},
		c_source[[
	LIOBuffer buf;
	const uint8_t *data = NULL;
	size_t len = 0;
	int ltype = lua_type(L, ${size_or_data::idx});

	${this} = &buf;
	if(ltype == LUA_TSTRING) {
		data = lua_tolstring(L, ${size_or_data::idx}, &len);
		l_iobuffer_init(${this}, data, len);
	} else if(ltype == LUA_TNUMBER) {
		len = lua_tointeger(L, ${size_or_data::idx});
		l_iobuffer_init_cap(${this}, len);
	}
]],
		ffi_source[[
	local buf
	local data
	local len
	local ltype = type(${size_or_data})

	${this} = LIOBuffer_tmp
	if ltype == 'string' then
		data = ${size_or_data}
		len = #data
		C.l_iobuffer_init(${this}, data, len)
	elseif ltype == 'number' then
		len = ${size_or_data}
		C.l_iobuffer_init_cap(${this}, len)
	end

]],
	},
	destructor "free" {
		c_method_call "void" "l_iobuffer_cleanup" {}
	},
	method "__tostring" {
		var_out{ "const char *", "data", has_length = true},
		c_source[[ ${data} = ${this}->buf; ${data_len} = ${this}->size; ]],
		ffi_source[[ ${data} = ${this}.buf; ${data_len} = ${this}.size; ]],
	},
	method "tostring" {
		var_out{ "const char *", "data", has_length = true},
		c_source[[ ${data} = ${this}->buf; ${data_len} = ${this}->size; ]],
		ffi_source[[ ${data} = ${this}.buf; ${data_len} = ${this}.size; ]],
	},

	method "get_byte" {
		var_in{ "size_t", "offset"},
		var_out{ "unsigned int", "val"},
		c_source[[
	/* check offset. */
	if(${offset} >= ${this}->size) {
		return luaL_argerror(L, ${offset::idx}, "Offset out-of-bounds.");
	}
	${val} = ${this}->buf[${offset}];
]],
		ffi_source[[
	-- check offset.
	if(${offset} >= ${this}.size) then
		return error("Offset out-of-bounds.")
	end
	${val} = tonumber(${this}.buf[${offset}])
]],
	},

	method "set_byte" {
		var_in{ "size_t", "offset"},
		var_in{ "unsigned int", "val"},
		c_source[[
	/* check offset. */
	if(${offset} >= ${this}->size) {
		return luaL_argerror(L, ${offset::idx}, "Offset out-of-bounds.");
	}
	${this}->buf[${offset}] = ${val};
]],
		ffi_source[[
	-- check offset.
	if(${offset} >= ${this}.size) then
		return error("Offset out-of-bounds.")
	end
	${this}.buf[${offset}] = ${val}
]],
	},

	ffi_source[[
local function next_byte(data, i, j)
	if i < j then
		return data[i], next_byte(data, i+1, j)
	end
	return data[i]
end
]],
	method "get_bytes" {
		var_in{ "size_t", "offset?"},
		var_in{ "size_t", "length?"},
		-- temp vars.
		c_source[[
	size_t i;
	size_t data_len;
	const uint8_t *data;
]],
		c_source[[
	data_len = ${this}->size;
	data = ${this}->buf;
	/* apply offset. */
	if(${offset} > 0) {
		if(${offset} >= data_len) {
			return luaL_argerror(L, ${offset::idx}, "Offset out-of-bounds.");
		}
		data += ${offset};
		data_len -= ${offset};
	}
	/* apply length. */
	if(${length} > 0) {
		if(${length} > data_len) {
			return luaL_argerror(L, ${length::idx}, "Length out-of-bounds.");
		}
		data_len = ${length};
	}
	luaL_checkstack(L, data_len, "No enough space on Lua stack for bytes.");
	for(i=0; i < data_len; i++) {
		lua_pushinteger(L, data[i]);
	}
	return data_len;
]],
		ffi_source[[
	local data_len = ${this}.size
	local data = ${this}.buf
	-- apply offset.
	if(${offset} ~= 0) then
		if(${offset} >= data_len) then
			return error("Offset out-of-bounds.")
		end
		data = data + ${offset}
		data_len = data_len - ${offset}
	end
	-- apply length.
	if(${length} ~= 0) then
		if(${length} > data_len) then
			return error("Length out-of-bounds.")
		end
		data_len = ${length}
	end
	if data_len > 0 then
		return next_byte(data, 0, data_len - 1)
	end
]],
	},

	method "get_data" {
		var_in{ "size_t", "offset?"},
		var_in{ "size_t", "length?"},
		var_out{ "const char *", "data", has_length = true},
		c_source[[
	${data_len} = ${this}->size;
	${data} = ${this}->buf;
	/* apply offset. */
	if(${offset} > 0) {
		if(${offset} >= ${data_len}) {
			return luaL_argerror(L, ${offset::idx}, "Offset out-of-bounds.");
		}
		${data} += ${offset};
		${data_len} -= ${offset};
	}
	/* apply length. */
	if(${length} > 0) {
		if(${length} > ${data_len}) {
			return luaL_argerror(L, ${length::idx}, "Length out-of-bounds.");
		}
		${data_len} = ${length};
	}
]],
		ffi_source[[
	${data_len} = ${this}.size
	${data} = ${this}.buf
	-- apply offset.
	if(${offset} ~= 0) then
		if(${offset} >= ${data_len}) then
			return error("Offset out-of-bounds.")
		end
		${data} = data + ${offset}
		${data_len} = ${data_len} - ${offset}
	end
	-- apply length.
	if(${length} ~= 0) then
		if(${length} > ${data_len}) then
			return error("Length out-of-bounds.")
		end
		${data_len} = ${length}
	end
]],
	},

	method "set_data" {
		var_in{ "const char *", "data"},
		c_source[[
	/* check capacity */
	if(${data_len} > ${this}->capacity) {
		if(!l_iobuffer_set_capacity(${this}, ${data_len})) {
			return luaL_argerror(L, ${data::idx}, "Can't grow buffer, not enough space.");
		}
	}
	memcpy(${this}->buf, ${data}, ${data_len});
	${this}->size = ${data_len};
]],
		ffi_source[[
	-- check capacity
	if(${data_len} > ${this}.capacity) then
		if(C.l_iobuffer_set_capacity(${this}, ${data_len}) == 0) then
			return error("Can't grow buffer, not enough space.");
		end
	end
	ffi.copy(${this}.buf, ${data}, ${data_len});
	${this}.size = ${data_len};
]],
	},

	method "reset" {
		c_method_call "void" "l_iobuffer_reset" {}
	},
	method "__len" {
		var_out{ "size_t", "size", ffi_wrap = "tonumber"},
		c_source[[${size} = ${this}->size; ]],
		ffi_source[[${size} = ${this}.size; ]],
	},
	method "size" {
		var_out{ "size_t", "size", ffi_wrap = "tonumber"},
		c_source[[${size} = ${this}->size; ]],
		ffi_source[[${size} = ${this}.size; ]],
	},
	method "set_size" {
		c_method_call "bool" "l_iobuffer_set_size" { "size_t", "size" }
	},
	method "capacity" {
		var_out{ "size_t", "capacity", ffi_wrap = "tonumber"},
		c_source[[${capacity} = ${this}->capacity; ]],
		ffi_source[[${capacity} = ${this}.capacity; ]],
	},
	method "set_capacity" {
		c_method_call "bool" "l_iobuffer_set_capacity" { "size_t", "capacity" }
	},
}
