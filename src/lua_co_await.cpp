#include "lua_co_await.h"

// register tutorial types, remove it freely
#include "tutorial/tutorial_async.h"
#include "tutorial/tutorial_quota.h"
#include "tutorial/tutorial_warp.h"

namespace iris {
	void lua_co_await_t::lua_registar(lua_t&& lua) {
		lua.define<&lua_co_await_t::get_version>("get_version");
		lua.define<&lua_co_await_t::start>("start");
		lua.define<&lua_co_await_t::terminate>("terminate");
		lua.define<&lua_co_await_t::poll>("poll");
		lua.define<&lua_co_await_t::yield>("yield");
		lua.define("tutorial_async", lua.make_type<tutorial_async_t>("tutorial_async"));
		lua.define("tutorial_quota", lua.make_type<tutorial_quota_t>("tutorial_quota"));
		lua.define("tutorial_warp", lua.make_type<tutorial_warp_t>("tutorial_warp"));
		lua.define("run_tutorials", lua.load("local co_await = ... \n\
local complete_count = 0 \n\
coroutine.wrap(function () \n\
	co_await.tutorial_async.create():run() \n\
	print('complete async') \n\
	complete_count = complete_count + 1 \n\
end)() \n\
coroutine.wrap(function () \n\
	--co_await.tutorial_quota.create():run() \n\
	print('complete quota') \n\
	complete_count = complete_count + 1 \n\
end)() \n\
coroutine.wrap(function () \n\
	--co_await.tutorial_warp.create():run() \n\
	print('complete warp') \n\
	complete_count = complete_count + 1 \n\
end)() \n\
while complete_count < 3 do \n\
	co_await:poll(1000) \n\
end \n\
print('all completed')\n\
"));
	}

	lua_co_await_t::lua_co_await_t() {}
	lua_co_await_t::~lua_co_await_t() noexcept {
		terminate();
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
			default:
				return "unknown";
		}
	}

	lua_coroutine_t<void> lua_co_await_t::yield() noexcept {
		lua_warp_t* current_warp = co_await iris_switch(static_cast<lua_warp_t*>(nullptr));
		co_await iris_switch(current_warp);
	}
	
	bool lua_co_await_t::start(size_t thread_count) {
		status_t expected = status_idle;
		if (status.compare_exchange_strong(expected, status_running, std::memory_order_release)) {
			async_worker.resize(thread_count);
			async_worker.start();

			// attach current warp
			main_warp = std::make_unique<lua_warp_t>(async_worker);
			bool result = main_warp->preempt();
			assert(result); // must success
			return result;
		} else {
			return false;
		}
	}

	bool lua_co_await_t::terminate() noexcept {
		status_t expected = status_running;
		if (status.compare_exchange_strong(expected, status_idle, std::memory_order_release)) {
			async_worker.terminate();
			async_worker.join();

			// join with finalize
			while (!main_warp->join()) {}
			main_warp->yield();
			main_warp = nullptr;

			return true;
		} else {
			return false;
		}
	}

	bool lua_co_await_t::poll(size_t delayInMilliseconds) {
		auto guard = write_fence();
		if (main_warp) {
			if (main_warp->join()) {
				return true;
			} else if (delayInMilliseconds == 0) {
				return false;
			} else {
				async_worker.poll_delay(0, delayInMilliseconds);
				return main_warp->join();
			}
		} else {
			return false;
		}
	}
}