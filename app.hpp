#pragma once

#include "rt_window.hpp"
#include "rt_pipeline.hpp"
#include "rt_device.hpp"


namespace rt {
	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		RtWindow rtWindow{ WIDTH,HEIGHT,"Hello ray trace!" };
		RtDevice rtDevice{rtWindow};
		RtPipeline rtPipeline{ rtDevice,"simp_shader.vert.spv", "simp_shader.frag.spv", RtPipeline::defaultPipelineConfigInfo (WIDTH,HEIGHT)};

	};
}