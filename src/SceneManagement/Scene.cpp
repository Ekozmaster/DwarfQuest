#include <src/SceneManagement/Scene.h>

namespace DwarfQuest {
    namespace Core {

        Scene::Scene() {
            m_destroyed = false;
        }

        Scene::~Scene() {
            Destroy();
        }

        void Scene::Destroy() {
            if (m_destroyed) return;
            m_destroyed = true;
        }
    }
}