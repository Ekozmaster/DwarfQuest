#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <src/EntitiesBehaviourModel/Transform.h>


namespace Core {

    Transform::Transform() {
        position = glm::vec3(0);
        rotation = glm::quat(1, 0, 0, 0);
        scale = glm::vec3(1, 1, 1);
    }

    void Transform::Translate(const glm::vec3& delta) {
        position += delta;
    }

    void Transform::Rotate(const glm::vec3& delta) {
        rotation = glm::quat(delta) * rotation;
    }

    void Transform::Scale(const glm::vec3& delta) {
        scale *= delta;
    }

    void Transform::SetEulerRotation(const glm::vec3& angles) {
        rotation = glm::quat(angles);
    }

    glm::mat4 Transform::GetTRSMatrix() const {
        return glm::translate(glm::scale(glm::toMat4(rotation), scale), position);
    }
}

