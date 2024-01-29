#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "vcu_camera.hpp"
#include "simple_render_system.hpp"
#define MAX_FRAME_TIME 0.5f

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

	FirstApp::FirstApp() {
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run() {
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
				vcuRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				vcuRenderer.endSwapChainRenderPass(commandBuffer);
				vcuRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(vcuDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr<VcuModel> vcuModel = VcuModel::createModelFromFile(vcuDevice, "models/smooth_vase.obj");	

        auto gameObject = VcuGameObject::createGameObject();
        gameObject.model = vcuModel;
		gameObject.transform.translation = { .0f, 0.0f, 2.5f };
		gameObject.transform.scale = glm::vec3{ 3.f };
        gameObjects.push_back(std::move(gameObject));
	}
}