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

void Test_Scene_DestroyGameObject_By_Iterator_Beginning() {
    // TODO: Implement specific child indexing using TDD.
    auto it = unittest_scene->NewGameObject();
    //unittest_scene->NewGameObject();
    bool result = unittest_scene->DestroyGameObject(it);
    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("GameObject"), 0);
}


void Setup_Scene_Tests() {
    SETUP_ACTIVE_TEST_CONTEXT("Testing Scene's GameObjects management.");
    REGISTER_TEST_UNIT_SETUP(Test_Scene_Setup);
    REGISTER_TEST_UNIT_TEAR_DOWN(Test_Scene_TearDown);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject_With_Name);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject_With_Parent);
    REGISTER_TEST_UNIT(Test_Scene_NewGameObject_With_Name_And_Parent);
    REGISTER_TEST_UNIT(Test_Scene_DestroyGameObject_By_Iterator_Beginning);
}

#endif