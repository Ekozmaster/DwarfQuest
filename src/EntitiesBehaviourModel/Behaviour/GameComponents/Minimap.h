#pragma once

#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>

namespace GameComponents {

    class Minimap : public Core::Behaviour {
    private:
        void* m_renderer = nullptr;

    public:
        void* cameraRef = nullptr;

        void Init() override;
        void Update() override;
        void Render() override;
        void Destroy() override;
    };

}