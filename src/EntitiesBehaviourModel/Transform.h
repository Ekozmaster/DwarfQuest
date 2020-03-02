#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace DwarfQuest {
    namespace Core {

        class Transform {
        public:
            glm::vec3 position;
            glm::quat rotation;
            glm::vec3 scale;

            Transform();

            void Translate(const glm::vec3& delta);
            void Rotate(const glm::vec3& delta);
            void Scale(const glm::vec3& delta);
            void SetEulerRotation(const glm::vec3& angles);

            glm::mat4 GetTRSMatrix() const;
        };

    }
}