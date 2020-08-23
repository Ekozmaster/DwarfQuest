#pragma once

#include <src/Window.h>
#include <src/SceneManagement/Scene.h>
#include <src/Graphics/OpenGL/Camera.h>

namespace DwarfQuest {
    namespace Core {

        class Application {
        private:
            Window* window;
            SDL_Event event;
            bool running = false;

            Scene* scene;

            void FrameSetupStage();
            void FrameMainStage();
            void FrameFinishStage();
            void FrameRenderStage();
        public:
            bool Init();
            int Run();
            void Destroy();
            void DispatchEvents();
        };
    }
}