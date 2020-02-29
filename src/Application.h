#pragma once

#include "Window.h"
#include "Graphics/OpenGL/Mesh.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/Texture.h"
#include "EntitiesBehaviourModel/GameObject.h"

namespace DwarfQuest {
    namespace Core {

        class Application {
        private:
            Window* window;
            SDL_Event event;
            bool running = false;

            Mesh* mesh;
            Texture* texture;
            Shader* shader;

            GameObject* gameObject;

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