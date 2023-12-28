#include "vcu_window.hpp"

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
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}

}