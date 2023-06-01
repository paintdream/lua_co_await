#include "lua_co_await.h"

namespace iris {
	void lua_co_await_t::lua_registar(iris_lua_t&& lua) {
		lua.define<&lua_co_await_t::get_version>("get_version");
	}

	std::string_view lua_co_await_t::get_version() const noexcept {
		return "lua_co_await 0.0.0";
	}

	std::string_view lua_co_await_t::get_status() const noexcept {
		switch (status.load(std::memory_order_acquire)) {
			case status_idle:
				return "idle";
			case status_running:
				return "running";
			case status_stopping:
				return "stopping";
			default:
				return "unknown";
		}
	}

	size_t lua_co_await_t::poll(size_t count) {
		// TODO: place code here
		return 0;
	}
}