#pragma once

#include "rt_window.hpp"

namespace rt {
	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		RtWindow rtWindow{ WIDTH,HEIGHT,"Hello ray trace!" };


	};
}