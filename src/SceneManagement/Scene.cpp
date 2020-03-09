#include <src/SceneManagement/Scene.h>

namespace DwarfQuest {
    namespace Core {

        Scene::Scene() {
            m_destroyed = false;
        }

        Scene::~Scene() {
            DestroyAllGameObjects();
        }

        DataStructures::Tree<GameObject>::Iterator Scene::NewGameObject() {
            auto it = m_gameObjects.Push(GameObject());
            return it;
        }

        DataStructures::Tree<GameObject>::Iterator Scene::NewGameObject(std::string name) {
            auto it = m_gameObjects.Push(GameObject(name));
            return it;
        }

        DataStructures::Tree<GameObject>::Iterator Scene::NewGameObject(DataStructures::Tree<GameObject>::Iterator parent) {
            auto it = m_gameObjects.Push(GameObject(), parent);
            return it;
        }

        DataStructures::Tree<GameObject>::Iterator Scene::NewGameObject(std::string name, DataStructures::Tree<GameObject>::Iterator parent) {
            auto it = m_gameObjects.Push(GameObject(name), parent);
            return it;
        }

        bool Scene::DestroyGameObject(DataStructures::Tree<GameObject>::Iterator gameObject) {
            m_gameObjects.Erase(gameObject);
            return true;
        }

        bool Scene::DestroyGameObject(GameObject& gameObject) {
            auto it = m_gameObjects.Find(gameObject);
            if (it != m_gameObjects.End()) return DestroyGameObject(it);
            return false;
        }

        void Scene::DestroyAllGameObjects() {
            m_gameObjects.Clear();
        }

        void Scene::Destroy() {
            if (m_destroyed) return;
            DestroyAllGameObjects();
            m_destroyed = true;
        }
    }
}