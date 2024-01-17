
#include "first_app.hpp"

namespace vcu {

	void FirstApp::run() {
		while (!vcuWindow.shouldClose()) {
			glfwPollEvents();
		}
	}
}