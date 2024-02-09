#include "lua_co_await.h"

// register tutorial types, remove it freely
// to run tutorial, just launch the lua console, type:
// require("lua_co_await").new():run_tutorials()
//

#include "tutorial/tutorial_binding.h"
#include "tutorial/tutorial_async.h"
#include "tutorial/tutorial_quota.h"
#include "tutorial/tutorial_warp.h"

namespace iris {
	void lua_co_await_t::lua_registar(lua_t&& lua) {
		lua.set_current<&lua_co_await_t::get_version>("get_version");
		lua.set_current<&lua_co_await_t::start>("start");
		lua.set_current<&lua_co_await_t::terminate>("terminate");
		lua.set_current<&lua_co_await_t::poll>("poll");
		lua.set_current<&lua_co_await_t::tutorial_binding>("tutorial_binding");
		lua.set_current<&lua_co_await_t::tutorial_async>("tutorial_async");
		lua.set_current<&lua_co_await_t::tutorial_warp>("tutorial_warp");
		lua.set_current<&lua_co_await_t::tutorial_quota>("tutorial_quota");
		lua.set_current<&lua_co_await_t::run_tutorials>("run_tutorials");
	}

	lua_co_await_t::lua_co_await_t() {}
	lua_co_await_t::~lua_co_await_t() noexcept {
		// force terminate on destructing
		terminate();
	}

	std::string_view lua_co_await_t::get_version() const noexcept {
		return "lua_co_await 0.0.0";
	}

	bool lua_co_await_t::is_running() const noexcept {
		return async_worker != nullptr;
	}

	bool lua_co_await_t::start(size_t thread_count) {
		if (async_worker == nullptr) {
			async_worker = std::make_unique<lua_async_worker_t>();
			async_worker->resize(thread_count);
			// add current thread as an external worker
			size_t thread_index = async_worker->append(std::thread());
			async_worker->make_current(thread_index);
			async_worker->start();
			// attach current warp
			main_warp = std::make_unique<lua_warp_t>(std::ref(*async_worker));
			bool result = main_warp->preempt();
			assert(result); // must success
			return result;
		} else {
			return false;
		}
	}

	bool lua_co_await_t::terminate() noexcept {
		if (async_worker != nullptr) {
			// cleanup threadlocal worker state
			async_worker->make_current(~(size_t)0);

			// notify terminate and wait all threads to exit
			async_worker->terminate();
			async_worker->join();

			// join with finalize, executing remaining warp tasks (if any)
			while (!main_warp->join<true, true>()) {}

			// cleanup warp data
			main_warp->yield();
			main_warp = nullptr;
			async_worker = nullptr;

			return true;
		} else {
			return false;
		}
	}

	bool lua_co_await_t::poll(size_t delayInMilliseconds) {
		auto guard = write_fence();
		// try to poll tasks of main_warp, also poll other tasks in given time if there is no task in main_warp.

		if (async_worker != nullptr && main_warp != nullptr) {
			// try poll
			if (main_warp->join()) {
				return true;
			} else if (delayInMilliseconds == 0) {
				return false;
			} else {
				async_worker->poll_delay(0, delayInMilliseconds);
				// try poll again
				return main_warp->join();
			}
		} else {
			return false;
		}
	}

	// tutorials
	lua_ref_t lua_co_await_t::tutorial_binding(lua_t&& lua) {
		return lua.make_type<tutorial_binding_t>("tutorial_binding");
	}

	lua_ref_t lua_co_await_t::tutorial_async(lua_t&& lua) {
		return lua.make_type<tutorial_async_t>("tutorial_async");
	}

	lua_ref_t lua_co_await_t::tutorial_warp(lua_t&& lua) {
		assert(async_worker != nullptr);
		return lua.make_type<tutorial_warp_t>("tutorial_warp", std::ref(*async_worker));
	}

	lua_ref_t lua_co_await_t::tutorial_quota(lua_t&& lua, size_t capacity) {
		assert(async_worker != nullptr);
		return lua.make_type<tutorial_quota_t>("tutorial_quota", std::ref(*async_worker), capacity);
	}

	void lua_co_await_t::run_tutorials(lua_refptr_t<lua_co_await_t>&& self, lua_t&& lua) {
		lua.call<void>(lua.load("local co_await = ... \n\
co_await:start(4) \n\
co_await:tutorial_binding().new():run() \n\
local complete_count = 0 \n\
coroutine.wrap(function () \n\
	co_await:tutorial_async().new():run() \n\
	print('complete async') \n\
	complete_count = complete_count + 1 \n\
end)() \n\
coroutine.wrap(function () \n\
	co_await:tutorial_warp().new():run() \n\
	print('complete warp') \n\
	complete_count = complete_count + 1 \n\
end)() \n\
coroutine.wrap(function () \n\
	co_await:tutorial_quota(100).new():run() \n\
	print('complete quota') \n\
	complete_count = complete_count + 1 \n\
end)() \n\
while complete_count < 3 do \n\
	co_await:poll(1000) \n\
end \n\
co_await:terminate() \n\
collectgarbage() \n\
print('all completed')\n\
"), std::move(self));
	}
}