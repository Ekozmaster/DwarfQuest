#pragma once

#include <string>

#include <src/EntitiesBehaviourModel/GameObject.h>
#include <src/DataStructures/Tree.h>

namespace DwarfQuest {
    namespace Core {

        class Scene {
        private:
            bool m_destroyed;
            DataStructures::Tree<GameObject> m_gameObjects;

        public:

            Scene();
            ~Scene();

            DataStructures::Tree<GameObject>::Iterator NewGameObject(int childIndex = -1);
            DataStructures::Tree<GameObject>::Iterator NewGameObject(const std::string& name, int childIndex = -1);
            DataStructures::Tree<GameObject>::Iterator NewGameObject(DataStructures::Tree<GameObject>::Iterator parent, int childIndex = -1);
            DataStructures::Tree<GameObject>::Iterator NewGameObject(const std::string& name, DataStructures::Tree<GameObject>::Iterator parent, int childIndex = -1);

            DataStructures::Tree<GameObject>::Iterator DestroyGameObject(DataStructures::Tree<GameObject>::Iterator gameObject);
            DataStructures::Tree<GameObject>::Iterator DestroyGameObject(GameObject& gameObject);

        private:
            DataStructures::Tree<GameObject>::Iterator FindRecursive(const std::string& name, DataStructures::Tree<GameObject>::Iterator current);
            DataStructures::Tree<GameObject>::Iterator FindRecursive(const GameObject& gameObject, DataStructures::Tree<GameObject>::Iterator current);

        public:
            DataStructures::Tree<GameObject>::Iterator Find(const std::string& name);
            DataStructures::Tree<GameObject>::Iterator Find(const GameObject& gameObject);

            DataStructures::Tree<GameObject>::Iterator Begin();
            DataStructures::Tree<GameObject>::Iterator End();
            DataStructures::Tree<GameObject>::Iterator Back();

            void Init();
            void Update();
            void Render();
            void DestroyAllGameObjects();
            void Destroy();
        };

    }
}