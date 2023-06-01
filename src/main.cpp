#include "lua_co_await.h"
#include "iris/iris_common.inl"
using namespace iris;

extern "C"
#if defined(_MSC_VER)
__declspec(dllexport)
#else
__attribute__((visibility("default")))
#endif
int luaopen_lua_co_await(lua_State * L) {
	iris_lua_t lua(L);
	auto ref = lua.make_type<lua_co_await_t>("lua_co_await");
	lua_rawgeti(L, LUA_REGISTRYINDEX, ref.get());
	lua.deref(ref);

	return 1;
}
