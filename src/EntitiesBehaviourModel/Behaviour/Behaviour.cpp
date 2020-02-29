#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>

namespace DwarfQuest {
    namespace Core {

        Behaviour::Behaviour() {
            enabled = true;
            m_initialized = false;
        }

        bool Behaviour::IsInitialized() {
            return m_initialized;
        }

        void Behaviour::MarkInitilized() {
            m_initialized = true;
        }

        void Behaviour::Init() {
            m_initialized = true;
        }

        void Behaviour::Update() {}
        void Behaviour::Destroy() {}
    }
}