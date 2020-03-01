#pragma once

#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>

namespace DwarfQuest {
    namespace GameComponents {

        class TestBehaviour : public DwarfQuest::Core::Behaviour {
        private:
            char name = 'A';
        public:
            void Init() override;
            void Update() override;
            void Destroy() override;
        };
    }
}
