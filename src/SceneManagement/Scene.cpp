#include <src/SceneManagement/Scene.h>

namespace DwarfQuest {
    namespace Core {

        Scene::Scene() {
            m_destroyed = false;
        }

        Scene::~Scene() {
            DestroyAllGameObjects();
        }

        DataStructures::Tree<GameObject>::Iterator Scene::NewGameObject(int childIndex) {
            if (childIndex > -1) return m_gameObjects.Push(GameObject(), childIndex);
            return m_gameObjects.Push(GameObject());
        }

        DataStructures::Tree<GameObject>::Iterator Scene::NewGameObject(std::string name, int childIndex) {
            if (childIndex > -1) return m_gameObjects.Push(GameObject(name), childIndex);
            return m_gameObjects.Push(GameObject(name));
        }

        DataStructures::Tree<GameObject>::Iterator Scene::NewGameObject(DataStructures::Tree<GameObject>::Iterator parent, int childIndex) {
            if (childIndex > -1) return m_gameObjects.Push(GameObject(), parent, childIndex);
            return m_gameObjects.Push(GameObject(), parent);
        }

        DataStructures::Tree<GameObject>::Iterator Scene::NewGameObject(std::string name, DataStructures::Tree<GameObject>::Iterator parent, int childIndex) {
            if (childIndex > -1) return m_gameObjects.Push(GameObject(name), parent, childIndex);
            return m_gameObjects.Push(GameObject(name), parent);
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

        DataStructures::Tree<GameObject>::Iterator Scene::Begin() {
            return m_gameObjects.Begin();
        }

        DataStructures::Tree<GameObject>::Iterator Scene::End() {
            return m_gameObjects.End();
        }

        DataStructures::Tree<GameObject>::Iterator Scene::Back() {
            return m_gameObjects.Back();
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