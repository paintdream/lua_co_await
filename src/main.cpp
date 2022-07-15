#include "iris/iris_common.h"
#include "iris/iris_common.inl"
#include "iris/iris_lua.h"
#include <string_view>
using namespace iris;

class lua_co_await {
public:
	static void lua_registar(iris_lua_t&& lua) {
		lua.define<&lua_co_await::get_version>("get_version");
	}

	static std::string_view get_version() {
		return "lua_co_await 0.0.0";
	}
};

extern "C"
#if defined(_MSC_VER) && defined(_WIN32)
__declspec(dllexport)
#else
__attribute__((visibility("default")))
#endif
int luaopen_lua_co_await(lua_State * L) {
	iris_lua_t luaState(L);
	auto ref = luaState.make_type<lua_co_await>("lua_co_await");
	lua_rawgeti(L, LUA_REGISTRYINDEX, ref.get());
	luaState.deref(ref);

	return 1;
}
