#include "vcu_window.hpp"

//std
#include <stdexcept>

namespace vcu {

	VcuWindow::VcuWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{name} {
		initWindow();
	}

	VcuWindow::~VcuWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void VcuWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void VcuWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		} 
	}

	void VcuWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto vcuWindow = reinterpret_cast<VcuWindow *>(glfwGetWindowUserPointer(window));
		vcuWindow->framebufferResized = true;
		vcuWindow->width = width;
		vcuWindow->height = height;
	}
}