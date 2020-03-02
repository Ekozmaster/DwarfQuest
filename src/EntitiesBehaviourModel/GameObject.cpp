#include <src/EntitiesBehaviourModel/GameObject.h>

namespace DwarfQuest {
    namespace Core {

        GameObject::GameObject() {
            enabled = true;
            m_initialized = false;
            m_destroyed = false;
        }

        GameObject::~GameObject() {
            // Destroy all components.
            Destroy();
        }

        void GameObject::Init() {
            if (m_initialized) return;
            for (auto it = m_components.begin(); it != m_components.end(); ++it) {
                Behaviour* component = *it;
                if (component->enabled) {
                    if (!component->IsInitialized()) {
                        component->Init();
                        component->MarkInitilized();
                    }
                }
            }
            m_initialized = true;
        }

        void GameObject::Update() {
            if (!enabled) return;
            for (auto it = m_components.begin(); it != m_components.end(); ++it) {
                Behaviour* component = *it;
                if (component->enabled) {
                    if (!component->IsInitialized()) {
                        component->Init();
                        component->MarkInitilized();
                    }
                    component->Update();
                }
            }
        }

        void GameObject::Destroy() {
            if (m_destroyed) return;

            for (auto it = m_components.begin(); it != m_components.end(); ++it) {
                (*it)->Destroy();
                delete (*it);
            }
            m_components.clear();
            m_destroyed = true;
        }
    }
}
