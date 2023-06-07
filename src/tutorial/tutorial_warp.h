// tutorial_warp.h
// PaintDream (paintdream@paintdream.com)
// 2023-06-01

#pragma once
#include "../common.h"

namespace iris {
	class tutorial_warp_t : enable_read_write_fence_t<> {
	public:
		static void lua_registar(lua_t&& lua);

		tutorial_warp_t();
		~tutorial_warp_t() noexcept;
	};
}