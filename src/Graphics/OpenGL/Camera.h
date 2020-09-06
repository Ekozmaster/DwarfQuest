#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<src/Window.h>

namespace DwarfQuest {
    namespace Core {

        namespace Camera {

            typedef struct {
                glm::vec3 position;
                glm::vec3 direction;
                glm::vec3 up;

                double fov;
                double aspectRatio;
                double nearPlane;
                double farPlane;
            } Camera;

            inline glm::mat4 CameraPerspectiveMatrix(const Camera& camera) {
                return glm::perspective(glm::radians(camera.fov), camera.aspectRatio, camera.nearPlane, camera.farPlane);
            }

            inline glm::mat4 CameraLookMatrix(const Camera& camera) {
                return glm::lookAt(camera.position, camera.position + camera.direction, camera.up);
            }

            inline Camera InitADefaultCamera(Window* window) {
                Camera camera;
                camera.position = glm::vec3(-3.0, 4.0, -3.0);
                camera.direction = glm::vec3(1.0, -1.0, 1.0);
                camera.up = glm::vec3(0.0, 1.0, 0.0);

                camera.fov = 45.0f;
                camera.aspectRatio = window->Width() / window->Height();
                camera.nearPlane = 0.3;
                camera.farPlane = 1000.0;
                return camera;
            }

            inline glm::vec3 Right(const Camera& camera) {
                return glm::normalize(glm::cross(camera.direction, camera.up));
            }
        }
    }
}