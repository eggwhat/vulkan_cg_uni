#pragma once

#include "vcu_window.hpp"
#include "vcu_pipeline.hpp"
#include "vcu_device.hpp"

namespace vcu {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		VcuWindow vcuWindow{ WIDTH, HEIGHT, "Vulkan" };
		VcuDevice vcuDevice{ vcuWindow };
		VcuPipeline vcuPipeline{
			vcuDevice, 
			"shaders/simple_shader.vert.spv", 
			"shaders/simple_shader.frag.spv", 
			VcuPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
}; 


}

