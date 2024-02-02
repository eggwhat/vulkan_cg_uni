#pragma once

#include "vcu_window.hpp"
#include "vcu_device.hpp"
#include "vcu_swap_chain.hpp"

// std
#include <memory>
#include <vector>
#include <cassert>

namespace vcu {
	class VcuRenderer {
	public:
		VcuRenderer(VcuWindow &vcuWindow, VcuDevice &vcuDevice);
		~VcuRenderer();

		VcuRenderer(const VcuRenderer&) = delete;
		VcuRenderer& operator=(const VcuRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return vcuSwapChain->getRenderPass(); }
		float getAspectRatio() const { return vcuSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex]; 
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		VcuWindow& vcuWindow;
		VcuDevice& vcuDevice;
		std::unique_ptr<VcuSwapChain> vcuSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{0};
		bool isFrameStarted{false};
	};
} // namespace vcu

