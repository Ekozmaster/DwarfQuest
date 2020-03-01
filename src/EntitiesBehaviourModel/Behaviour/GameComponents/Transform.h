#pragma once

#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>
#include <glm/glm.hpp>

namespace DwarfQuest {
    namespace GameComponents {

        class Transform : public DwarfQuest::Core::Behaviour {
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