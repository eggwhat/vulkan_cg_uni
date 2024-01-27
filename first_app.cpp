
#include "first_app.hpp"

// std
#include <stdexcept>
#include <array>

namespace vcu {

	FirstApp::FirstApp() {
		loadModel();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	FirstApp::~FirstApp() {
		vkDestroyPipelineLayout(vcuDevice.device(), pipelineLayout, nullptr);
	}

	void FirstApp::run() {
		while (!vcuWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(vcuDevice.device());
	}

	void FirstApp::loadModel() {
		std::vector<VcuModel::Vertex> vertices {
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};
		vcuModel = std::make_unique<VcuModel>(vcuDevice, vertices);
	}

	void FirstApp::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(vcuDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void FirstApp::createPipeline() {
		assert(vcuSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		VcuPipeline::defaultPipelineConfigInfo(pipelineConfig); 
		pipelineConfig.renderPass = vcuSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		vcuPipeline = std::make_unique<VcuPipeline>(
			vcuDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
		);
	}

	void FirstApp::recreateSwapChain() {
		auto extent = vcuWindow.getExtent();
		while(extent.width || extent.height == 0){
			extent = vcuWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(vcuDevice.device());


		if (vcuSwapChain == nullptr) {
			vcuSwapChain = std::make_unique<VcuSwapChain>(vcuDevice, extent);
		}
		else {
			vcuSwapChain = std::make_unique<VcuSwapChain>(vcuDevice, extent, std::move(vcuSwapChain));
			if (vcuSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		
		createPipeline(); 
	}
	 
	void FirstApp::createCommandBuffers() {
	
		commandBuffers.resize(vcuSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vcuDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(vcuDevice.device(), &allocInfo, commandBuffers.data()) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");
		}
	}

	void FirstApp::freeCommandBuffers() {
		vkFreeCommandBuffers(vcuDevice.device(), vcuDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), 
			commandBuffers.data());
		commandBuffers.clear();
	}

	void FirstApp::recordCommandBuffer(int imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vcuSwapChain->getRenderPass();
		renderPassInfo.framebuffer = vcuSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };

		renderPassInfo.renderArea.extent = vcuSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(vcuSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(vcuSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0}, vcuSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);


		vcuPipeline->bind(commandBuffers[imageIndex]);
		vcuModel->bind(commandBuffers[imageIndex]);
		vcuModel->draw(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}
	}

	void FirstApp::drawFrame() {
		uint32_t imageIndex;
		auto result = vcuSwapChain->acquireNextImage(&imageIndex);
	
		if(result == VK_ERROR_OUT_OF_DATE_KHR){
			recreateSwapChain();
			return;
		}

		if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
			throw std::runtime_error("failed to acquire swap chain image");
		}

		recordCommandBuffer(imageIndex);
		result = vcuSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vcuWindow.wasWindowResized()) {
			vcuWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		if(result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image");
		}
	}

}