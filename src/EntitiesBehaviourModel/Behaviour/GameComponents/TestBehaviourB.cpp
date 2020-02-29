#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/TestBehaviourB.h>
#include <src/Utils/Logger.h>

namespace DwarfQuest {
    namespace Core {

        void TestBehaviourB::Init() {
            Logger::Info(std::string("TestBehaviourB.Init()") + name);
        }

        void TestBehaviourB::Update() {
            //Logger::Info("TestBehaviourB.Update()");
        }

        void TestBehaviourB::Destroy() {
            Logger::Info("TestBehaviourB.Destroy()");
        }

    }
}