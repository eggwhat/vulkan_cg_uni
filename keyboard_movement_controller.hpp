#pragma once

#include "vcu_game_object.hpp"
#include "vcu_window.hpp"

namespace vcu {
	
	class KeyboardMovementController {
	public:
		struct KeyMappings {
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveUp = GLFW_KEY_E;
			int moveDown = GLFW_KEY_Q;
			int lookLeft = GLFW_KEY_LEFT;
			int lookRight = GLFW_KEY_RIGHT;
			int lookUp = GLFW_KEY_UP;
			int lookDown = GLFW_KEY_DOWN;
			int cameraModeChange = GLFW_KEY_C;
			int shaderModeChange = GLFW_KEY_SPACE;
		};

		void moveInPlaneXZ(GLFWwindow* window, float dt, VcuGameObject &gameObject, int cameraMode);

		KeyMappings keys{};
		float moveSpeed{ 5.f };
		float lookSpeed{ 1.5f };
	};
}