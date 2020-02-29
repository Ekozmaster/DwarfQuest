#pragma once

#include <vector>
#include "Behaviour/Behaviour.h"

namespace DwarfQuest {
    namespace Core {

        class GameObject {
        private:
            std::vector<Behaviour*> m_components;
            bool m_destroyed;
            bool m_initialized;

        public:
            bool enabled;

            GameObject();
            ~GameObject();

            void Init();
            void Update();
            void Destroy();

            // ADD
            template<class T>
            T* AddComponent() {
                T* component = new T();
                m_components.push_back(component);
                return component;
            }

            // REMOVE
            template<class T>
            void RemoveComponent() {
                for (auto it = m_components.begin(); it != m_components.end(); ++it) {
                    if (dynamic_cast<T*>(*it) != NULL) {
                        delete (*it);
                        m_components.erase(it);
                        return;
                    }
                }
            }

            template<class T>
            void RemoveAllComponents() {
                for (auto it = m_components.begin(); it != m_components.end(); ++it) {
                    if (dynamic_cast<T*>(*it) != NULL) {
                        delete (*it);
                        m_components.erase(it--);
                    }
                }
            }

            // GET
            template<class T>
            T* GetComponent() {
                for (auto it = m_components.begin(); it != m_components.end(); ++it) {
                    if (dynamic_cast<T*>(*it) != NULL) return (T*)(*it);
                }
                return NULL;
            }

            template<class T>
            std::vector<T*> GetAllComponents() {
                std::vector<T*> result;
                for (auto it = m_components.begin(); it != m_components.end(); ++it) {
                    if (dynamic_cast<T*>(*it) != NULL) {
                        result.push_back((T*)(*it));
                    }
                }
                return result;
            }
        };
    }
}
