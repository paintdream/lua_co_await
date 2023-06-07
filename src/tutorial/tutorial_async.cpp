#include "tutorial_async.h"

namespace iris {
	void tutorial_async_t::lua_registar(lua_t&& lua) {
		lua.define<&tutorial_async_t::wait>("wait");
		lua.define("run", lua.load("local self = ...\n\
print('async wait for 1000 ms ' .. tostring(self))\n\
self:wait(1000)"));
	}

	tutorial_async_t::tutorial_async_t() {}
	tutorial_async_t::~tutorial_async_t() noexcept {
	}

	lua_coroutine_t<void> tutorial_async_t::wait(size_t millseconds) {
		lua_warp_t* current = co_await iris_switch(static_cast<lua_warp_t*>(nullptr));
		std::this_thread::sleep_for(std::chrono::milliseconds(millseconds));
		co_await iris_switch(current);
	}
}