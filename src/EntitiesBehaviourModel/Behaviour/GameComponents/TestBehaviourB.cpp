#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/TestBehaviourB.h>
#include <src/Utils/Logger.h>

namespace DwarfQuest {
    namespace GameComponents {

        void TestBehaviourB::Init() {
            DwarfQuest::Core::Logger::Info(std::string("TestBehaviourB.Init()") + name);
        }

        void TestBehaviourB::Update() {
            //Logger::Info("TestBehaviourB.Update()");
        }

        void TestBehaviourB::Destroy() {
            DwarfQuest::Core::Logger::Info("TestBehaviourB.Destroy()");
        }

    }
}