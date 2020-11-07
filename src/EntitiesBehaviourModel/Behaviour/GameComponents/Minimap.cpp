#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/Minimap.h>
#include <src/Input.h>
#include <src/EntitiesBehaviourModel/GameObject.h>
#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/Renderer.h>
#include <src/ResourceManagement/ResourceManager.h>
#include <glm/gtc/type_ptr.hpp>
#include <src/Graphics/OpenGL/Camera.h>

namespace DwarfQuest {
    namespace GameComponents {
        
        void Minimap::Init() {
            Core::GameObject* go = (Core::GameObject*)gameObject;
            m_renderer = go->AddComponent<GameComponents::Renderer>();
            Core::Material* material = Core::ResourceManager::GetOrLoadMaterialAsset("Assets/Materials/MinimapMaterial.mat");
            Core::Mesh* mesh = Core::ResourceManager::GetOrLoadMeshAsset("Assets/Models/Primitives/Plane.obj");
            ((Renderer*)m_renderer)->material = material;
            ((Renderer*)m_renderer)->mesh = mesh;
            ((Renderer*)m_renderer)->enabled = false;
            
            go->transform.position = glm::vec3(0, 0, -0.99f);
            go->transform.scale = glm::vec3(0.7f);
        }

        void Minimap::Update() {
            Renderer* renderer = (Renderer*)m_renderer;
            if (Core::Input::GetKeyDown("M")) {
                renderer->enabled = !renderer->enabled;
            }
            glm::vec3 cameraPos = ((Core::Camera::Camera*)cameraRef)->position;
            renderer->material->SetData("mapWorldPosition", glm::value_ptr(glm::vec2(cameraPos.x, cameraPos.z)), sizeof(GLfloat) * 2);
        }

        void Minimap::Render() {}
        void Minimap::Destroy() {}

    }
}