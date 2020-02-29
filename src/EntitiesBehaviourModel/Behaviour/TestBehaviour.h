#pragma once

#include "Behaviour.h"

namespace DwarfQuest {
    namespace Core {

        class TestBehaviour : public Behaviour {
        private:
            char name = 'A';
        public:
            void Init() override;
            void Update() override;
            void Destroy() override;
        };
    }
}