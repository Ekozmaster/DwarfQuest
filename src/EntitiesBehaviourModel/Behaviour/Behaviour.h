#pragma once

namespace DwarfQuest {
    namespace Core {
        class Behaviour {
        private:
            bool m_initialized;

        public:
            bool enabled;

            Behaviour();
            bool IsInitialized();
            void MarkInitilized();

            virtual void Init();
            virtual void Update();
            virtual void Destroy();
        };
    }
}