#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "vcu_buffer.hpp"
#include "vcu_camera.hpp"
#include "simple_render_system.hpp"

//#define MAX_FRAME_TIME 0.5f

// libs
#define GLFW_FORCE_RADIANS
#define GLFW_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <chrono>
#include <cassert>
#include <array>

namespace vcu {

	struct GlobalUbo {
		glm::mat4 projectionView{1.f};
		glm::vec3 lightDirection = glm::vec3{ glm::vec3{1.f, -3.f, -1.f} };
	};

	FirstApp::FirstApp() {
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run() {
		VcuBuffer globalUboBuffer{ 
			vcuDevice, 
			sizeof(GlobalUbo), 
			VcuSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			vcuDevice.properties.limits.minUniformBufferOffsetAlignment,
		};
		globalUboBuffer.map();


		SimpleRenderSystem simpleRenderSystem{ vcuDevice, vcuRenderer.getSwapChainRenderPass() };
        VcuCamera camera{};
    
        auto viewerObject = VcuGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!vcuWindow.shouldClose()) {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            //frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ(vcuWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = vcuRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

			if (auto commandBuffer = vcuRenderer.beginFrame()) {
				int frameIndex = vcuRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera
				};

				// update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				globalUboBuffer.writeToIndex(&ubo, frameIndex);
				globalUboBuffer.flushIndex(frameIndex);

				// render
				vcuRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
				vcuRenderer.endSwapChainRenderPass(commandBuffer);
				vcuRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(vcuDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr<VcuModel> vcuModel = VcuModel::createModelFromFile(vcuDevice, "models/flat_vase.obj");	

        auto flatVase = VcuGameObject::createGameObject();
		flatVase.model = vcuModel;
		flatVase.transform.translation = { -0.5f, 3.0f, 0.0f };
		flatVase.transform.rotation = { 1.0f, 0.0f, 0.0f };
		flatVase.transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		gameObjects.push_back(std::move(flatVase));

		vcuModel = VcuModel::createModelFromFile(vcuDevice, "models/smooth_vase.obj");
		auto smoothVase = VcuGameObject::createGameObject();
		smoothVase.model = vcuModel;
		smoothVase.transform.translation = { 0.5f, 3.0f, 0.0f };
		smoothVase.transform.rotation = { 1.0f, 0.0f, 0.0f };
		smoothVase.transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		gameObjects.push_back(std::move(smoothVase));
	}
}