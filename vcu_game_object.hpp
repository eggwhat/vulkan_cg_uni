#pragma once

#include "vcu_model.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>

namespace vcu {

    struct TransformComponent {
        glm::vec3 translation{};  // (position offset)
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::vec3 rotation;

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    class VcuGameObject {
    public:
        using id_t = unsigned int;

        static VcuGameObject createGameObject() {
            static id_t currentId = 0;
            return VcuGameObject{ currentId++ };
        }

        VcuGameObject(const VcuGameObject&) = delete;
        VcuGameObject& operator=(const VcuGameObject&) = delete;
        VcuGameObject(VcuGameObject&&) = default;
        VcuGameObject& operator=(VcuGameObject&&) = default;

        id_t getId() { return id; }

        std::shared_ptr<VcuModel> model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        VcuGameObject(id_t objId) : id{ objId } {}

        id_t id;
    };
}