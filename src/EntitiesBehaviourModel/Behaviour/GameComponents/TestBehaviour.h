#pragma once

#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>
#include <src/Graphics/OpenGL/Mesh.h>
#include <src/Graphics/OpenGL/Shader.h>
#include <src/Graphics/OpenGL/Texture.h>
#include <src/Graphics/OpenGL/Camera.h>

namespace DwarfQuest {
    namespace GameComponents {

        class TestBehaviour : public DwarfQuest::Core::Behaviour {
        private:
            char name = 'A';
            Core::Mesh* mesh;
            Core::Shader* shader;
            Core::Texture* texture;
        public:
            void Init() override;
            void Update() override;
            void Render() override;
            void Destroy() override;
        };
    }
}
