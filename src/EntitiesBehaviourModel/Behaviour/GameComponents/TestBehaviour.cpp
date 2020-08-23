#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/TestBehaviour.h>
#include <src/Utils/Logger.h>

namespace DwarfQuest {
    namespace GameComponents {

        void TestBehaviour::Init() {
            DwarfQuest::Core::Logger::Info("TestBehaviour.Init()");
        }

        void TestBehaviour::Update() {}
        void TestBehaviour::Render() {}

        void TestBehaviour::Destroy() {
            DwarfQuest::Core::Logger::Info("TestBehaviour.Destroy()");
        }

    }
}
