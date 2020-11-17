#pragma once

#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>

namespace GameComponents {

    class TestBehaviour : public Core::Behaviour {
    private:
        char name = 'A';
    public:
        void Init() override;
        void Update() override;
        void Render() override;
        void Destroy() override;
    };
}

