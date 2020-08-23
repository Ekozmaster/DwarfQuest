#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>

namespace DwarfQuest {
    namespace Core {

        Behaviour::Behaviour() {
            enabled = true;
            m_initialized = false;
            gameObject = nullptr;
        }

        bool Behaviour::IsInitialized() {
            return m_initialized;
        }

        void Behaviour::Init() {
            m_initialized = true;
        }

        void Behaviour::Update() {}
        void Behaviour::Destroy() {}
        void Behaviour::Render() {}
    }
}