#pragma once

namespace DwarfQuest {
    namespace Core {
        class Behaviour {
        private:
            bool m_initialized;

        public:
            void* gameObject;
            bool enabled;

            Behaviour();
            bool IsInitialized();

            virtual void Init();
            virtual void Update();
            virtual void Destroy();
            virtual void Render();
        };
    }
}