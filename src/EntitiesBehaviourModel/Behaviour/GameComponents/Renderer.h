#pragma once

#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>
#include <src/Graphics/OpenGL/Mesh.h>
#include <src/Graphics/OpenGL/Shader.h>
#include <src/Graphics/OpenGL/Texture.h>
#include <src/Graphics/OpenGL/Camera.h>
#include <src/Graphics/OpenGL/Material.h>

namespace GameComponents {

    class Renderer : public Core::Behaviour {
    private:
        char name = 'A';
    public:
        Core::Material* material = nullptr;
        Core::Mesh* mesh = nullptr;

        void Init() override;
        void Update() override;
        void Render() override;
        void Destroy() override;
    };
}

