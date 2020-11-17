#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/TestBehaviour.h>
#include <src/Utils/Logger.h>

namespace GameComponents {

    void TestBehaviour::Init() {
        Core::Logger::Info("TestBehaviour.Init()");
    }

    void TestBehaviour::Update() {}
    void TestBehaviour::Render() {}

    void TestBehaviour::Destroy() {
        Core::Logger::Info("TestBehaviour.Destroy()");
    }

}

