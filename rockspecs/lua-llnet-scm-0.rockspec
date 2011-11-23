package = "lua-llnet"
version = "scm-0"
source = {
	url = "git://github.com/Neopallium/lua-llnet.git",
}
description = {
	summary = "Low-Level network for Lua",
	homepage = "http://github.com/Neopallium/lua-llnet",
	license = "MIT/X11",
}
dependencies = {
	"lua >= 5.1",
}
build = {
	type = "builtin",
	modules = {
		llnet = {
			sources = {"src/pre_generated-llnet.nobj.c"},
		}
	}
}
