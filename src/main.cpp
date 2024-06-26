#include "lua_co_await.h"
#include "../ref/iris/src/iris_common.inl"
using namespace iris;

extern "C"
#if defined(_MSC_VER)
__declspec(dllexport)
#else
__attribute__((visibility("default")))
#endif
int luaopen_lua_co_await(lua_State * L) {
	return lua_t::forward(L, [](lua_t lua) {
		return lua.make_type<lua_co_await_t>("lua_co_await");
	});
}
