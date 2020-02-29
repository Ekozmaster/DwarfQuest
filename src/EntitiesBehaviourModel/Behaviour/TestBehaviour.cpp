#include "TestBehaviour.h"
#include "../../Utils/Logger.h"

namespace DwarfQuest {
    namespace Core {

        void TestBehaviour::Init() {
            Logger::Info("TestBehaviour.Init()");
        }

        void TestBehaviour::Update() {
            //Logger::Info("TestBehaviour.Update()");
        }

        void TestBehaviour::Destroy() {
            Logger::Info("TestBehaviour.Destroy()");
        }

    }
}