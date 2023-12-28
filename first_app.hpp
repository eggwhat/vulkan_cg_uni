#pragma once

#include "vcu_window.hpp"

namespace vcu {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		VcuWindow window{ WIDTH, HEIGHT, "Vulkan" };
};


}

