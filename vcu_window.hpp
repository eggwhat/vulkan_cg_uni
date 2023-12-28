#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace vcu {

	class VcuWindow {

	public:
		VcuWindow(int width, int height, std::string windowName);
		~VcuWindow();

		VcuWindow(const VcuWindow&) = delete;
		VcuWindow& operator=(const VcuWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }

	private:
		void initWindow();

		const int width;
		const int height;

		std::string windowName;
		GLFWwindow* window;
};

}