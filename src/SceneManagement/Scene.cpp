#include <src/SceneManagement/Scene.h>

namespace DwarfQuest {
    namespace Core {

        Scene::Scene() {
            m_destroyed = false;
        }

        Scene::~Scene() {
            Destroy();
        }

        void Scene::DestroyRecursive(DataStructures::Tree<GameObject*>::Iterator currentIt) {
            delete (*currentIt);
            // Step down into currentNode's childs.
            if (!currentIt.IsDepthEnd()) {
                currentIt.StepDown();
                for (DataStructures::Tree<GameObject*>::Iterator it = currentIt; !it.IsBreadthEnd(); ++it) {
                    DestroyRecursive(it);
                }
            }
        }

        void Scene::Destroy() {
            if (m_destroyed) return;
            for (DataStructures::Tree<GameObject*>::Iterator it = gameObjects.Begin(); it != gameObjects.End(); ++it) {
                DestroyRecursive(it);
            }
            m_destroyed = true;
        }
    }
}