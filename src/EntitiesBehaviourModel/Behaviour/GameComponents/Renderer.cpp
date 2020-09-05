#include <glm/gtc/type_ptr.hpp>

#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/Renderer.h>
#include <src/Utils/Logger.h>
#include <src/EntitiesBehaviourModel/GameObject.h>
#include <src/ResourceManagement/ResourceManager.h>
#include <src/Graphics/OpenGL/Graphics.h>
#include <src/Graphics/OpenGL/ShadersDefinitions.h>

namespace DwarfQuest {
    namespace GameComponents {

        void Renderer::Init() {
            DwarfQuest::Core::Logger::Info("Renderer.Init()");
        }

        void Renderer::Update() {}

        void Renderer::Render() {
            material->Use();
            glm::mat4 model = ((Core::GameObject*)gameObject)->transform.GetTRSMatrix();
            Core::Graphics::SetShaderMatrix(SHADERS_MODEL_MATRIX, glm::value_ptr(model));

            // Rendering.
            Core::Graphics::SetMesh(mesh);
            Core::Graphics::RenderMesh();
        }

        void Renderer::Destroy() {
            DwarfQuest::Core::Logger::Info("Renderer.Destroy()");
        }

    }
}
