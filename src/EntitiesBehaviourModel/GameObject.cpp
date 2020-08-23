#include <src/EntitiesBehaviourModel/GameObject.h>
#include <src/Utils/Testing.h>

namespace DwarfQuest {
    namespace Core {

        GameObject::GameObject() {
            name = "GameObject";
            enabled = true;
            initialized = false;
            m_destroyed = false;
            COUNT_CONSTRUCTOR_CALL("GameObject");
        }

        GameObject::GameObject(const GameObject& other) 
            : name(other.name), enabled(other.enabled), initialized(other.initialized), m_destroyed(other.m_destroyed)  {
            COUNT_CONSTRUCTOR_CALL("GameObject");
        }

        GameObject::GameObject(GameObject&& other)
            : name(other.name), enabled(other.enabled), initialized(other.initialized), m_destroyed(other.m_destroyed) {

            other.enabled = false;
            other.m_destroyed = true;
            COUNT_CONSTRUCTOR_CALL("GameObject");
        }

        GameObject::GameObject(std::string objName) : name(objName) {
            enabled = true;
            initialized = false;
            m_destroyed = false;
            COUNT_CONSTRUCTOR_CALL("GameObject");
        }

        GameObject::~GameObject() {
            // Destroy all components.
            Destroy();
            COUNT_DESTRUCTOR_CALL("GameObject");
        }

        void GameObject::Init() {
            if (!enabled) return;
            for (auto it = m_components.begin(); it != m_components.end(); ++it) {
                Behaviour* component = *it;
                if (component->enabled) {
                    if (!component->IsInitialized()) {
                        component->Init();
                        component->MarkInitialized();
                    }
                }
            }
            initialized = true;
        }

        void GameObject::Update() {
            if (!enabled || !initialized) return;
            for (auto it = m_components.begin(); it != m_components.end(); ++it) {
                Behaviour* component = *it;
                if (component->enabled) {
                    if (!component->IsInitialized()) {
                        component->Init();
                        component->MarkInitialized();
                    }
                    component->Update();
                }
            }
        }

        void GameObject::Render() {
            if (!enabled || !initialized) return;
            for (auto it = m_components.begin(); it != m_components.end(); ++it) {
                Behaviour* component = *it;
                if (component->enabled) {
                    if (!component->IsInitialized()) {
                        component->Init();
                        component->MarkInitialized();
                    }
                    component->Render();
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
