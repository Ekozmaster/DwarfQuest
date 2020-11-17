#include <src/SceneManagement/Scene.h>

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

    DataStructures::Tree<GameObject>::Iterator Scene::NewGameObject(const std::string& name, int childIndex) {
        if (childIndex > -1) return m_gameObjects.Push(GameObject(name), childIndex);
        return m_gameObjects.Push(GameObject(name));
    }

    DataStructures::Tree<GameObject>::Iterator Scene::NewGameObject(DataStructures::Tree<GameObject>::Iterator parent, int childIndex) {
        if (childIndex > -1) return m_gameObjects.Push(GameObject(), parent, childIndex);
        return m_gameObjects.Push(GameObject(), parent);
    }

    DataStructures::Tree<GameObject>::Iterator Scene::NewGameObject(const std::string& name, DataStructures::Tree<GameObject>::Iterator parent, int childIndex) {
        if (childIndex > -1) return m_gameObjects.Push(GameObject(name), parent, childIndex);
        return m_gameObjects.Push(GameObject(name), parent);
    }

    DataStructures::Tree<GameObject>::Iterator Scene::DestroyGameObject(DataStructures::Tree<GameObject>::Iterator gameObject) {
        if (gameObject == End()) return End();
        return m_gameObjects.Erase(gameObject);
    }

    DataStructures::Tree<GameObject>::Iterator Scene::DestroyGameObject(GameObject& gameObject) {
        auto it = m_gameObjects.Find(gameObject);
        return DestroyGameObject(it);
    }


    // ### FIND
    DataStructures::Tree<GameObject>::Iterator Scene::FindRecursive(const std::string& name, DataStructures::Tree<GameObject>::Iterator current) {
        if ((*current).name == name) return current;

        if (!current.IsDepthEnd()) {
            current.StepDown();
            for (; !current.IsBreadthEnd(); ++current) {
                auto find = FindRecursive(name, current);
                if (find != End()) return find;
            }
        }
        return End();
    }

    DataStructures::Tree<GameObject>::Iterator Scene::FindRecursive(const GameObject& gameObject, DataStructures::Tree<GameObject>::Iterator current) {
        if ((*current) == gameObject) return current;

        if (!current.IsDepthEnd()) {
            current.StepDown();
            for (; !current.IsBreadthEnd(); ++current) {
                auto find = FindRecursive(gameObject, current);
                if (find != End()) return find;
            }
        }
        return End();
    }

    DataStructures::Tree<GameObject>::Iterator Scene::Find(const std::string& name) {
        for (auto it = Begin(); !it.IsBreadthEnd(); ++it) {
            auto find = FindRecursive(name, it);
            if (find != End()) return find;
        }
        return End();
    }

    DataStructures::Tree<GameObject>::Iterator Scene::Find(const GameObject& gameObject) {
        for (auto it = Begin(); !it.IsBreadthEnd(); ++it) {
            auto find = FindRecursive(gameObject, it);
            if (find != End()) return find;
        }
        return End();
    }


    // ### BEGIN, END, BACK.
    DataStructures::Tree<GameObject>::Iterator Scene::Begin() {
        return m_gameObjects.Begin();
    }

    DataStructures::Tree<GameObject>::Iterator Scene::End() {
        return m_gameObjects.End();
    }

    DataStructures::Tree<GameObject>::Iterator Scene::Back() {
        return m_gameObjects.Back();
    }


    // ### LIFECYCLE METHODS
    void Scene::Init() {
        for (auto it = Begin(); !it.IsBreadthEnd(); ++it) {
            GameObject& gm = *it;
            if (!gm.initialized) gm.Init();
        }
    }

    void Scene::Update() {
        for (auto it = Begin(); !it.IsBreadthEnd(); ++it) {
            GameObject& gm = *it;
            if (!gm.initialized) gm.Init();
            gm.Update();
        }
    }

    void Scene::Render() {
        for (auto it = Begin(); !it.IsBreadthEnd(); ++it) {
            GameObject& gm = *it;
            if (!gm.initialized) gm.Init();
            gm.Render();
        }
    }


    // ### DESTRUCTION.
    void Scene::DestroyAllGameObjects() {
        m_gameObjects.Clear();
    }

    void Scene::Destroy() {
        if (m_destroyed) return;
        DestroyAllGameObjects();
        m_destroyed = true;
    }
}
