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
            DataStructures::Tree<GameObject>::Iterator NewGameObject(std::string name, int childIndex = -1);
            DataStructures::Tree<GameObject>::Iterator NewGameObject(DataStructures::Tree<GameObject>::Iterator parent, int childIndex = -1);
            DataStructures::Tree<GameObject>::Iterator NewGameObject(std::string name, DataStructures::Tree<GameObject>::Iterator parent, int childIndex = -1);

            bool DestroyGameObject(DataStructures::Tree<GameObject>::Iterator gameObject);
            bool DestroyGameObject(GameObject& gameObject);

            DataStructures::Tree<GameObject>::Iterator Begin();
            DataStructures::Tree<GameObject>::Iterator End();
            DataStructures::Tree<GameObject>::Iterator Back();

            void DestroyAllGameObjects();
            void Destroy();
        };

    }
}