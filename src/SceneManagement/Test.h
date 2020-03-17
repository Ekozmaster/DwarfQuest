#pragma once

#ifdef DWARFQUEST_TESTING
#include <src/SceneManagement/Scene.h>
#include <src/EntitiesBehaviourModel/GameObject.h>

namespace DQCore = DwarfQuest::Core;
namespace DQTesting = DwarfQuest::Testing;

DQCore::Scene* unittest_scene;

void Test_Scene_Setup() {
    unittest_scene = new DQCore::Scene();
}

void Test_Scene_TearDown() {
    delete unittest_scene;
    unittest_scene = NULL;

    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("Tree::Node"), 0);
    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("GameObject"), 0);
}

// Scene.NewGameObject();
void Test_Scene_NewGameObject() {
    auto it = unittest_scene->NewGameObject();
    ASSERT_TRUTHY(it.IsBreadthBegin());
    ASSERT_FALSY(it.IsBreadthEnd());
    ASSERT_TRUTHY(it.IsDepthBegin());
    ASSERT_TRUTHY(it.IsDepthEnd());
}

// Scene.NewGameObject(std::string name);
void Test_Scene_NewGameObject_With_Name() {
    auto it = unittest_scene->NewGameObject("Test GameObject");
    ASSERT_TRUTHY(it.IsBreadthBegin());
    ASSERT_FALSY(it.IsBreadthEnd());
    ASSERT_TRUTHY(it.IsDepthBegin());
    ASSERT_TRUTHY(it.IsDepthEnd());
    ASSERT_EQUALS((*it).name, std::string("Test GameObject"));
}

// Scene.NewGameObject(Tree::Iterator parent);
void Test_Scene_NewGameObject_With_Parent() {
    auto parentIt = unittest_scene->NewGameObject();
    auto it = unittest_scene->NewGameObject(parentIt);

    // Parent
    ASSERT_TRUTHY(parentIt.IsBreadthBegin());
    ASSERT_FALSY(parentIt.IsBreadthEnd());
    ASSERT_TRUTHY(parentIt.IsDepthBegin());
    ASSERT_FALSY(parentIt.IsDepthEnd());

    // Child
    ASSERT_TRUTHY(it.IsBreadthBegin());
    ASSERT_FALSY(it.IsBreadthEnd());
    ASSERT_FALSY(it.IsDepthBegin());
    ASSERT_TRUTHY(it.IsDepthEnd());
}


// Scene.NewGameObject(std::string name, Tree::Iterator parent);
void Test_Scene_NewGameObject_With_Name_And_Parent() {
    auto parentIt = unittest_scene->NewGameObject("Test Parent GameObject");
    auto it = unittest_scene->NewGameObject("Test Child GameObject", parentIt);

    // Parent
    ASSERT_TRUTHY(parentIt.IsBreadthBegin());
    ASSERT_FALSY(parentIt.IsBreadthEnd());
    ASSERT_TRUTHY(parentIt.IsDepthBegin());
    ASSERT_FALSY(parentIt.IsDepthEnd());

    // Child
    ASSERT_TRUTHY(it.IsBreadthBegin());
    ASSERT_FALSY(it.IsBreadthEnd());
    ASSERT_FALSY(it.IsDepthBegin());
    ASSERT_TRUTHY(it.IsDepthEnd());
    ASSERT_EQUALS((*it).name, std::string("Test Child GameObject"));
    it.StepUp();
    ASSERT_EQUALS((*it).name, std::string("Test Parent GameObject"));
}


// Testing insertion at specific positions.
void Test_Scene_NewGameObject_At_Beginning() {
    auto it = unittest_scene->NewGameObject();
    ASSERT_TRUTHY(it.IsBreadthBegin());
    ASSERT_FALSY(it.IsBreadthEnd());
    ASSERT_TRUTHY(it.IsDepthBegin());
    ASSERT_TRUTHY(it.IsDepthEnd());

    it = unittest_scene->NewGameObject(0);
    ASSERT_TRUTHY(it.IsDepthBegin());
    ASSERT_TRUTHY(it.IsDepthEnd());
    ASSERT_TRUTHY(it.IsBreadthBegin());
    ASSERT_FALSY(it.IsBreadthEnd());
    ++it;
    ASSERT_FALSY(it.IsBreadthEnd());
}

void Test_Scene_NewGameObject_At_Middle() {
    unittest_scene->NewGameObject();
    unittest_scene->NewGameObject();
    auto it = unittest_scene->NewGameObject(1);

    ASSERT_TRUTHY(it.IsDepthBegin());
    ASSERT_TRUTHY(it.IsDepthEnd());
    ASSERT_FALSY(it.IsBreadthBegin());
    ++it;
    ASSERT_FALSY(it.IsBreadthEnd());
    --it; --it;
    ASSERT_TRUTHY(it.IsBreadthBegin());
}

void Test_Scene_NewGameObject_At_End() {
    unittest_scene->NewGameObject();
    unittest_scene->NewGameObject();
    auto it = unittest_scene->NewGameObject();

    ASSERT_TRUTHY(it.IsDepthBegin());
    ASSERT_TRUTHY(it.IsDepthEnd());
    ++it;
    ASSERT_TRUTHY(it.IsBreadthEnd());
    --it; --it;
    ASSERT_FALSY(it.IsBreadthBegin());
    --it;
    ASSERT_TRUTHY(it.IsBreadthBegin());
}

// Test insertion at specific position in parent.
void Test_Scene_NewGameObject_With_Parent_At_Beginning() {
    auto parentIt = unittest_scene->NewGameObject();
    unittest_scene->NewGameObject(parentIt);
    auto it = unittest_scene->NewGameObject(parentIt, 0);

    ASSERT_TRUTHY(it.IsBreadthBegin());
    ASSERT_FALSY(it.IsBreadthEnd());
    ASSERT_FALSY(it.IsDepthBegin());
    ASSERT_TRUTHY(it.IsDepthEnd());

    it = unittest_scene->NewGameObject(parentIt, 0);
    ASSERT_FALSY(it.IsDepthBegin());
    ASSERT_TRUTHY(it.IsDepthEnd());
    ASSERT_TRUTHY(it.IsBreadthBegin());
    ASSERT_FALSY(it.IsBreadthEnd());
    ++it;
    ASSERT_FALSY(it.IsBreadthEnd());
}

void Test_Scene_NewGameObject_With_Parent_At_Middle() {
    auto parentIt = unittest_scene->NewGameObject();
    unittest_scene->NewGameObject(parentIt);
    unittest_scene->NewGameObject(parentIt);
    auto it = unittest_scene->NewGameObject(parentIt, 1);

    ASSERT_FALSY(it.IsDepthBegin());
    ASSERT_TRUTHY(it.IsDepthEnd());
    ASSERT_FALSY(it.IsBreadthBegin());
    ++it;
    ASSERT_FALSY(it.IsBreadthEnd());
    --it; --it;
    ASSERT_TRUTHY(it.IsBreadthBegin());
}

void Test_Scene_NewGameObject_With_Parent_At_End() {
    auto parentIt = unittest_scene->NewGameObject();
    unittest_scene->NewGameObject(parentIt);
    unittest_scene->NewGameObject(parentIt);
    auto it = unittest_scene->NewGameObject(parentIt);

    ASSERT_FALSY(it.IsDepthBegin());
    ASSERT_TRUTHY(it.IsDepthEnd());
    ++it;
    ASSERT_TRUTHY(it.IsBreadthEnd());
    --it; --it;
    ASSERT_FALSY(it.IsBreadthBegin());
    --it;
    ASSERT_TRUTHY(it.IsBreadthBegin());
}

void Test_Scene_Begin() {
    ASSERT_EQUALS(unittest_scene->Begin(), unittest_scene->End());

    unittest_scene->NewGameObject();
    unittest_scene->NewGameObject();
    unittest_scene->NewGameObject();

    auto it = unittest_scene->Begin();
    ASSERT_NOT_EQUALS(it, unittest_scene->End());
    ASSERT_TRUTHY(it.IsBreadthBegin());
    ++it;
    ASSERT_FALSY(it.IsBreadthEnd());
    ++it; ++it;
    ASSERT_TRUTHY(it.IsBreadthEnd());
}

void Test_Scene_End() {
    unittest_scene->NewGameObject();
    unittest_scene->NewGameObject();
    unittest_scene->NewGameObject();

    auto it = unittest_scene->End();
    ASSERT_NOT_EQUALS(it, unittest_scene->Begin());
    ASSERT_TRUTHY(it.IsBreadthEnd());
    --it;
    ASSERT_FALSY(it.IsBreadthBegin());
    --it; --it;
    ASSERT_TRUTHY(it.IsBreadthBegin());
}

void Test_Scene_Back() {
    ASSERT_EQUALS(unittest_scene->Back(), unittest_scene->End());
    ASSERT_EQUALS(unittest_scene->Back(), unittest_scene->Begin());
    unittest_scene->NewGameObject("A");
    ASSERT_EQUALS(unittest_scene->Back(), unittest_scene->Begin());
    unittest_scene->NewGameObject("B");
    ASSERT_NOT_EQUALS(unittest_scene->Back(), unittest_scene->Begin());
    unittest_scene->NewGameObject("C");
    ASSERT_NOT_EQUALS(unittest_scene->Back(), unittest_scene->Begin());

    auto it = unittest_scene->Back();
    ASSERT_NOT_EQUALS(it, unittest_scene->End());
    ASSERT_EQUALS((*it).name, "C");
}

// Scene.DestroyGameObject(Tree::Iterator gameObject);
void Test_Scene_DestroyGameObject_By_Iterator_Beginning() {
    // TODO: Implement Find() on Scene using TDD.
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    unittest_scene->NewGameObject("C");
    auto it = unittest_scene->Begin();
    
    bool result = unittest_scene->DestroyGameObject(it);
    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("GameObject"), 2);
    ASSERT_EQUALS((*unittest_scene->Begin()).name, "B");
}


void Setup_Scene_Tests() {
    SETUP_ACTIVE_TEST_CONTEXT("Testing Scene's GameObjects management.");
    REGISTER_TEST_UNIT_SETUP(Test_Scene_Setup);
    REGISTER_TEST_UNIT_TEAR_DOWN(Test_Scene_TearDown);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject_With_Name);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject_With_Parent);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject_With_Name_And_Parent);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject_At_Beginning);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject_At_Middle);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject_At_End);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject_With_Parent_At_Beginning);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject_With_Parent_At_Middle);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject_With_Parent_At_End);
    REGISTER_TEST_UNIT(Test_Scene_Begin);
    REGISTER_TEST_UNIT(Test_Scene_End);
    REGISTER_TEST_UNIT(Test_Scene_DestroyGameObject_By_Iterator_Beginning);
}

#endif
