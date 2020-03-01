#pragma once
#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>

namespace DwarfQuest {
    namespace GameComponents {

        class TestBehaviourB : public DwarfQuest::Core::Behaviour {
        private:
        public:
            char name = 'B';
            void Init() override;
            void Update() override;
            void Destroy() override;
        };
    }
}