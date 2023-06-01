// lua_co_await.h
// PaintDream (paintdream@paintdream.com)
// 2023-06-01

#pragma once
#include "common.h"

namespace iris {
	class lua_co_await_t {
	public:
		enum status_t : size_t {
			status_idle,
			status_running,
			status_stopping,
		};

		static void lua_registar(iris_lua_t&& lua);
		std::string_view get_version() const noexcept;
		std::string_view get_status() const noexcept;
		size_t poll(size_t count);

	private:
		std::atomic<status_t> status = status_idle;
	};
}