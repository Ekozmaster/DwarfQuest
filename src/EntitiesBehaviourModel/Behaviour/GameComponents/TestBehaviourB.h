#pragma once
#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>

namespace GameComponents {

    class TestBehaviourB : public Core::Behaviour {
    private:
    public:
        char name = 'B';
        void Init() override;
        void Update() override;
        void Destroy() override;
    };
}