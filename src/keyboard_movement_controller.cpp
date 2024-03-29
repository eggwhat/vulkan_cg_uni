#include "keyboard_movement_controller.hpp"

namespace vcu {

    void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, VcuGameObject& gameObject, int& cameraMode, glm::vec3 movingObjectTranslation,
        glm::vec3 movingObjectRotation) {

        if (cameraMode == 1) {
            gameObject.transform.translation = glm::vec3(0.f, -3.5f, -15.f);
            gameObject.transform.rotation = glm::vec3(0.f, 0.0f, 0.f);
            return;
        }

        if (cameraMode == 2) {
            gameObject.transform.translation = glm::vec3(0.f, -6.f, -10.f);
            auto direction = movingObjectTranslation - gameObject.transform.translation;
            direction = glm::normalize(direction);
            float angle = glm::atan(direction.x, direction.z);
            gameObject.transform.rotation = glm::vec3(-glm::atan(direction.y, glm::sqrt(direction.x * direction.x + direction.z * direction.z)), angle, 0.f);
            return;
        }

        if (cameraMode == 3) {
            gameObject.transform.translation = movingObjectTranslation + glm::vec3{.0f, -8.0f, 5.0f};
            gameObject.transform.rotation = movingObjectRotation + glm::vec3{ -2.2f, 0.f, -glm::pi<float>()};
            return;
		}

        if (cameraMode == 4) {
            gameObject.transform.translation = glm::vec3(0.f, -2.f, -10.f);
            gameObject.transform.rotation = glm::vec3(0.f, 0.0f, 0.f);
            cameraMode = 0;
            return;
        }

        glm::vec3 rotate{0};
        if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
        if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
        if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
        if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
        }

        gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
        gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

        float yaw = gameObject.transform.rotation.y;
        const glm::vec3 forwardDir{ glm::sin(yaw), 0.f, glm::cos(yaw) };
        const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
        const glm::vec3 upDir{ 0.f, -1.f, 0.f };

        glm::vec3 moveDir{ 0.f };
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
        }

    }

}