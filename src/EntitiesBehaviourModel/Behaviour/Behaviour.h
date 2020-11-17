#pragma once

namespace Core {
    class Behaviour {
    protected:
        bool m_initialized;

    public:
        void* gameObject;
        bool enabled;

        Behaviour();
        bool IsInitialized();
        void MarkInitialized();

        virtual void Init();
        virtual void Update();
        virtual void Destroy();
        virtual void Render();
    };
}
