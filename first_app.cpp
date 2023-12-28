
#include "first_app.hpp"

namespace vcu {

	void FirstApp::run() {
		while (!window.shouldClose()) {
			glfwPollEvents();
		}
	}
}