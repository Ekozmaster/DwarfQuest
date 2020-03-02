#pragma once

#include <src/EntitiesBehaviourModel/GameObject.h>


namespace DwarfQuest {
    namespace Core {

        class Scene {
        private:
            bool m_destroyed;

        public:

            Scene();
            ~Scene();

            void Destroy();
        };

    }
}