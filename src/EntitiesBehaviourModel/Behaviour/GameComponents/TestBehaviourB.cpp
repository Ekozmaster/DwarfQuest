#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/TestBehaviourB.h>
#include <src/Utils/Logger.h>

namespace GameComponents {

    void TestBehaviourB::Init() {
        Core::Logger::Info(std::string("TestBehaviourB.Init()") + name);
    }

    void TestBehaviourB::Update() {
        //Logger::Info("TestBehaviourB.Update()");
    }

    void TestBehaviourB::Destroy() {
        Core::Logger::Info("TestBehaviourB.Destroy()");
    }

}