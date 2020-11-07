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
            for (m_curComponentIndex = 0; m_curComponentIndex < m_components.size(); m_curComponentIndex++) {
                Behaviour* component = m_components[m_curComponentIndex];
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

            for (m_curComponentIndex = 0; m_curComponentIndex < m_components.size(); m_curComponentIndex++) {
                Behaviour* component = m_components[m_curComponentIndex];
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
            for (m_curComponentIndex = 0; m_curComponentIndex < m_components.size(); m_curComponentIndex++) {
                Behaviour* component = m_components[m_curComponentIndex];
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

            for (m_curComponentIndex = 0; m_curComponentIndex < m_components.size(); m_curComponentIndex++) {
                Behaviour* component = m_components[m_curComponentIndex];
                component->Destroy();
                delete component;
            }
            m_components.clear();
            m_destroyed = true;
        }
    }
}
