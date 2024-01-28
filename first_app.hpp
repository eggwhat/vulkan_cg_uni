#pragma once

#include "vcu_window.hpp"
#include "vcu_pipeline.hpp"
#include "vcu_game_object.hpp"
#include "vcu_device.hpp"
#include "vcu_swap_chain.hpp"

// std
#include <memory>
#include <vector>

namespace vcu {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();
		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;
		void run();

	private:
		void loadGameObjects();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);

		VcuWindow vcuWindow{ WIDTH, HEIGHT, "Vulkan" };
		VcuDevice vcuDevice{ vcuWindow };
		std::unique_ptr<VcuSwapChain> vcuSwapChain;
		std::unique_ptr<VcuPipeline> vcuPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VcuGameObject> gameObjects; 
}; 
} // namespace vcu

