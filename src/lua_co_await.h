// lua_co_await.h
// PaintDream (paintdream@paintdream.com)
// 2023-06-01

#pragma once
#include "common.h"

namespace iris {
	class lua_co_await_t : enable_read_write_fence_t<> {
	public:
		enum status_t : size_t {
			status_idle,
			status_running,
		};

		static void lua_registar(lua_t&& lua);

		lua_co_await_t();
		~lua_co_await_t() noexcept;
		std::string_view get_version() const noexcept;
		std::string_view get_status() const noexcept;
		lua_coroutine_t<void> yield() noexcept;
		bool start(size_t thread_count);
		bool terminate() noexcept;
		bool poll(size_t delayInMilliseonds);

	private:
		lua_async_worker_t async_worker;
		std::unique_ptr<lua_warp_t> main_warp;
		std::atomic<status_t> status = status_idle;
	};
}