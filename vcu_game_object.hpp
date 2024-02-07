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
            return VcuGameObject{ currentId++, 0};
        }

        static VcuGameObject makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3{ 1.f }, int type = 0);
        static VcuGameObject makeMovingObject(glm::vec3 color = glm::vec3{ 0.5f, 0.2f, 0.7f });

        VcuGameObject(const VcuGameObject&) = delete;
        VcuGameObject& operator=(const VcuGameObject&) = delete;
        VcuGameObject(VcuGameObject&&) = default;
        VcuGameObject& operator=(VcuGameObject&&) = default;

        id_t getId() { return id; }
        int type;

        glm::vec3 color{};
        TransformComponent transform{};

        // Optional pointer components
        std::shared_ptr<VcuModel> model{};
        std::unique_ptr<PointLightComponent> pointLight = nullptr;

    private:
        VcuGameObject(id_t objId, int type) : id{ objId }, type{type} {}

        id_t id;
    };
}