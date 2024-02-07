#pragma once

#include "vcu_model.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace vcu {

    struct TransformComponent {
        glm::vec3 translation{};  // (position offset)
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::vec3 rotation;

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    struct PointLightComponent {
        float lightIntensity = 1.0f;
	};

    class VcuGameObject {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, VcuGameObject>;

        static VcuGameObject createGameObject() {
            static id_t currentId = 0;
            return VcuGameObject{ currentId++ };
        }

        static VcuGameObject makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3{ 1.f });

        VcuGameObject(const VcuGameObject&) = delete;
        VcuGameObject& operator=(const VcuGameObject&) = delete;
        VcuGameObject(VcuGameObject&&) = default;
        VcuGameObject& operator=(VcuGameObject&&) = default;

        id_t getId() { return id; }

        glm::vec3 color{};
        TransformComponent transform{};

        // Optional pointer components
        std::shared_ptr<VcuModel> model{};
        std::unique_ptr<PointLightComponent> pointLight = nullptr;

    private:
        VcuGameObject(id_t objId) : id{ objId } {}

        id_t id;
    };
}