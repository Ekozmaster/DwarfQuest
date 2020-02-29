#pragma once
#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>

namespace DwarfQuest {
    namespace Core {

        class TestBehaviourB : public Behaviour {
        private:
        public:
            char name = 'B';
            void Init() override;
            void Update() override;
            void Destroy() override;
        };
    }
}