#pragma once

#include "rt_window.hpp"

#include "rt_device.hpp"
#include "game_object.hpp"
#include "rt_renderer.hpp"


#include <memory>
#include <vector>

namespace rt {
	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		App();
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		void run();

	private:
		void loadObjects();








		RtWindow rtWindow{ WIDTH,HEIGHT,"Hello ray trace!" };
		RtDevice rtDevice{rtWindow};
		RtRenderer rtRenderer{rtWindow, rtDevice};

		std::vector<RtObject> objects;

	};
}