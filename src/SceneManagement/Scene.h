#pragma once

#include <src/EntitiesBehaviourModel/GameObject.h>
#include <src/DataStructures/Tree.h>

namespace DwarfQuest {
    namespace Core {

        class Scene {
        private:
            bool m_destroyed;
            DataStructures::Tree<GameObject*> m_gameObjects;

        public:

            Scene();
            ~Scene();

            GameObject& CreateGameObject();
            GameObject& CreateGameObject(DataStructures::Tree<GameObject*>::Iterator parent);
            void DestroyGameObject(DataStructures::Tree<GameObject*>::Iterator gameObject);
            void DestroyGameObject(GameObject& gameObject);

        private:
            void DestroyRecursive(DataStructures::Tree<GameObject*>::Iterator currentIt);

        public:

            void Destroy();
        };

    }
}