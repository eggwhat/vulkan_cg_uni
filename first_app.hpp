#pragma once

#include "vcu_window.hpp"
#include "vcu_game_object.hpp"
#include "vcu_device.hpp"
#include "vcu_renderer.hpp"
#include "vcu_descriptors.hpp"

// std
#include <memory>
#include <vector>

#define CAMERA_MODES 5
#define SHADERS 3

namespace vcu {
	class FirstApp {
	public:
		static constexpr int WIDTH = 1200;
		static constexpr int HEIGHT = 900;
		int cameraMode{ 0 };
		int shaderMode{ 0 };
		bool fogEnabled{ false };
		bool nightMode{ true };
		const std::vector<const char*> cameraModeNames{ "Free", "Static", "Following", "3rd person", ""};
		const std::vector<const char*> shadingModeNames{ "Phong", "Flat", "Gouraud" };
		const std::vector<const char*> fogModeNames{ "Fog off", "Fog on" };
		const std::vector<const char*> nightModeNames{ "Day", "Night" };

		FirstApp();
		~FirstApp();
		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;
		void run();

	private:
		void loadGameObjects();

		VcuWindow vcuWindow{ WIDTH, HEIGHT, "Little engine" };
		VcuDevice vcuDevice{ vcuWindow };
		VcuRenderer vcuRenderer{ vcuWindow, vcuDevice };

		std::unique_ptr<VcuDescriptorPool> globalPool{};
		VcuGameObject::Map gameObjects; 
}; 
} // namespace vcu

