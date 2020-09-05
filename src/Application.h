#pragma once

#include <src/Window.h>
#include <src/SceneManagement/Scene.h>
#include <src/Graphics/OpenGL/Camera.h>
#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/VoxelTerrain.h>

namespace DwarfQuest {
    namespace Core {

        class Application {
        private:
            Window* window;
            SDL_Event event;
            bool running = false;
            GameComponents::VoxelTerrain* terrain;

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