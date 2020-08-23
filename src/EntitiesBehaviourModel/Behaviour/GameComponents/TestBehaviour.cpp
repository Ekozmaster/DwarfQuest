#include <glm/gtc/type_ptr.hpp>

#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/TestBehaviour.h>
#include <src/Utils/Logger.h>
#include <src/EntitiesBehaviourModel/GameObject.h>
#include <src/ResourceManagement/ResourceManager.h>
#include <src/Graphics/OpenGL/Graphics.h>
#include <src/Graphics/OpenGL/Camera.h>
#include <src/Graphics/OpenGL/ShadersDefinitions.h>

namespace DwarfQuest {
    namespace GameComponents {

        void TestBehaviour::Init() {
            DwarfQuest::Core::Logger::Info("TestBehaviour.Init()");
            mesh = Core::ResourceManager::GetOrLoadMeshAsset("Assets/Models/TestCrateModel.obj");
            shader = Core::ResourceManager::GetOrLoadShaderAsset("Assets/Shaders/testShader.glsl");
            texture = Core::ResourceManager::GetOrLoadTextureAsset("Assets/Textures/testCrateAOBake.jpg");
        }

        void TestBehaviour::Update() {
            ((Core::GameObject*)gameObject)->transform.position.x+= 0.001;
            std::string foo = std::to_string(((Core::GameObject*)gameObject)->transform.position.x);
            DwarfQuest::Core::Logger::Info(foo);
        }

        void TestBehaviour::Render() {
            Core::Graphics::SetShader(shader);
            glm::mat4 model = ((Core::GameObject*)gameObject)->transform.GetTRSMatrix();
            Core::Graphics::SetShaderMatrix(SHADERS_MODEL_MATRIX, glm::value_ptr(model));

            // Rendering.
            Core::Graphics::SetTexture(texture, 0);
            Core::Graphics::SetMesh(mesh);
            Core::Graphics::RenderMesh();
        }

        void TestBehaviour::Destroy() {
            DwarfQuest::Core::Logger::Info("TestBehaviour.Destroy()");
        }

    }
}
