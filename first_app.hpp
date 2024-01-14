#pragma once

#include "vcu_window.hpp"
#include "vcu_pipeline.hpp"

namespace vcu {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		VcuWindow window{ WIDTH, HEIGHT, "Vulkan" };
		VcuPipeline vcuPipeline{ "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv" };
};


}

