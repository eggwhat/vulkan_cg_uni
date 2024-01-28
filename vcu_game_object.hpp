#pragma once

#include "vcu_model.hpp"

// std
#include <memory>

namespace vcu {

    struct Transform2dComponent {
        glm::vec2 translation{};  // (position offset)
        glm::vec2 scale{ 1.f, 1.f };
        float rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMatrix{ {c, s}, {-s, c} };

            glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
            return rotMatrix * scaleMat;
        }
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
        Transform2dComponent transform2d{};

    private:
        VcuGameObject(id_t objId) : id{ objId } {}

        id_t id;
    };
}