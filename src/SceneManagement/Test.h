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
    ASSERT_TRUTHY((*it).name == "C");
}

void Test_Scene_Find_By_GameObject() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    auto it = unittest_scene->NewGameObject("C");

    auto find = unittest_scene->Find(*it);
    ASSERT_NOT_EQUALS(find, unittest_scene->End());
    ASSERT_EQUALS(find, it);
    ASSERT_TRUTHY((*find).name == "C");
}

void Test_Scene_Find_By_Name_Not_Found() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    unittest_scene->NewGameObject("C");

    auto notFound = unittest_scene->Find("D");
    ASSERT_EQUALS(notFound, unittest_scene->End());
}

void Test_Scene_Find_By_Name_Beginning() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    unittest_scene->NewGameObject("C");

    auto find = unittest_scene->Find("A");
    ASSERT_NOT_EQUALS(find, unittest_scene->End());
    ASSERT_EQUALS(find, unittest_scene->Begin());
    ASSERT_TRUTHY((*find).name == "A");
}

void Test_Scene_Find_By_Name_Middle() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    unittest_scene->NewGameObject("C");

    auto find = unittest_scene->Find("B");
    ASSERT_NOT_EQUALS(find, unittest_scene->End());
    ASSERT_NOT_EQUALS(find, unittest_scene->Begin());
    ASSERT_TRUTHY((*find).name == "B");
}

void Test_Scene_Find_By_Name_End() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    unittest_scene->NewGameObject("C");
    
    auto find = unittest_scene->Find("C");
    ASSERT_NOT_EQUALS(find, unittest_scene->End());
    ASSERT_NOT_EQUALS(find, unittest_scene->Begin());
    ASSERT_EQUALS(find, unittest_scene->Back());
    ASSERT_TRUTHY((*find).name == "C");
}

// Find in Children
void Test_Scene_Find_By_Name_First_Child() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    auto parent = unittest_scene->NewGameObject("C");
    unittest_scene->NewGameObject("D", parent);
    unittest_scene->NewGameObject("E", parent);
    unittest_scene->NewGameObject("F", parent);

    auto find = unittest_scene->Find("D");
    ASSERT_NOT_EQUALS(find, unittest_scene->End());
    ASSERT_TRUTHY((*find).name == "D");
    ASSERT_TRUTHY(find.IsDepthEnd());
    ASSERT_FALSY(find.IsDepthBegin());
    ASSERT_TRUTHY(find.IsBreadthBegin());
    ASSERT_FALSY(find.IsBreadthEnd());
    find.StepUp();
    ASSERT_TRUTHY((*find).name == (*parent).name);
}

void Test_Scene_Find_By_Name_Middle_Child() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    auto parent = unittest_scene->NewGameObject("C");
    unittest_scene->NewGameObject("D", parent);
    unittest_scene->NewGameObject("E", parent);
    unittest_scene->NewGameObject("F", parent);

    auto find = unittest_scene->Find("E");
    ASSERT_NOT_EQUALS(find, unittest_scene->End());
    ASSERT_TRUTHY((*find).name == "E");
    ASSERT_TRUTHY(find.IsDepthEnd());
    ASSERT_FALSY(find.IsDepthBegin());
    ASSERT_FALSY(find.IsBreadthBegin());
    ASSERT_FALSY(find.IsBreadthEnd());
    find.StepUp();
    ASSERT_TRUTHY((*find).name == (*parent).name);
}

void Test_Scene_Find_By_Name_Last_Child() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    auto parent = unittest_scene->NewGameObject("C");
    unittest_scene->NewGameObject("D", parent);
    unittest_scene->NewGameObject("E", parent);
    unittest_scene->NewGameObject("F", parent);

    auto find = unittest_scene->Find("F");
    ASSERT_NOT_EQUALS(find, unittest_scene->End());
    ASSERT_TRUTHY((*find).name == "F");
    ASSERT_TRUTHY(find.IsDepthEnd());
    ASSERT_FALSY(find.IsDepthBegin());
    ASSERT_FALSY(find.IsBreadthBegin());
    ++find;
    ASSERT_TRUTHY(find.IsBreadthEnd());
    --find;
    find.StepUp();
    ASSERT_TRUTHY((*find).name == (*parent).name);
}

// Scene.DestroyGameObject(Tree::Iterator gameObject);
void Test_Scene_DestroyGameObject_By_Iterator_Beginning() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    unittest_scene->NewGameObject("C");
    auto it = unittest_scene->Begin();
    
    auto parentResult = unittest_scene->DestroyGameObject(it);
    ASSERT_EQUALS(parentResult, unittest_scene->End());
    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("GameObject"), 2);
    ASSERT_TRUTHY((*unittest_scene->Begin()).name == "B");
}

void Test_Scene_DestroyGameObject_By_Iterator_Middle() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    unittest_scene->NewGameObject("C");
    auto it = unittest_scene->Find("B");

    auto parentResult = unittest_scene->DestroyGameObject(it);
    ASSERT_EQUALS(parentResult, unittest_scene->End());
    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("GameObject"), 2);
    ASSERT_TRUTHY((*unittest_scene->Begin()).name == "A");
    ASSERT_TRUTHY((*unittest_scene->Back()).name == "C");
}

void Test_Scene_DestroyGameObject_By_Iterator_End() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    unittest_scene->NewGameObject("C");
    auto it = unittest_scene->Back();

    auto parentResult = unittest_scene->DestroyGameObject(it);
    ASSERT_EQUALS(parentResult, unittest_scene->End());
    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("GameObject"), 2);
    ASSERT_TRUTHY((*unittest_scene->Begin()).name == "A");
    ASSERT_TRUTHY((*unittest_scene->Back()).name == "B");
}

void Test_Scene_DestroyGameObject_By_Iterator_Not_Found() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    unittest_scene->NewGameObject("C");
    auto it = unittest_scene->End();

    auto parentResult = unittest_scene->DestroyGameObject(it);
    ASSERT_EQUALS(parentResult, unittest_scene->End());
    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("GameObject"), 3);
    ASSERT_TRUTHY((*unittest_scene->Begin()).name == "A");
    ASSERT_TRUTHY((*unittest_scene->Back()).name == "C");
}

void Test_Scene_DestroyGameObject_By_Iterator_Children_Beginning() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    unittest_scene->NewGameObject("C");
    auto parent = unittest_scene->Back();
    unittest_scene->NewGameObject("D", parent);
    unittest_scene->NewGameObject("E", parent);
    unittest_scene->NewGameObject("F", parent);
    ASSERT_EQUALS(unittest_scene->Back().ChildCount(), 3);

    auto it = unittest_scene->Find("D");
    auto parentResult = unittest_scene->DestroyGameObject(it);
    ASSERT_EQUALS(parentResult, unittest_scene->Back());
    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("GameObject"), 5);
    ASSERT_TRUTHY((*unittest_scene->Begin()).name == "A");
    ASSERT_TRUTHY((*unittest_scene->Back()).name == "C");
    ASSERT_EQUALS(unittest_scene->Back().ChildCount(), 2);
}

void Test_Scene_DestroyGameObject_By_Iterator_Children_Middle() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    unittest_scene->NewGameObject("C");
    auto parent = unittest_scene->Back();
    unittest_scene->NewGameObject("D", parent);
    unittest_scene->NewGameObject("E", parent);
    unittest_scene->NewGameObject("F", parent);
    ASSERT_EQUALS(unittest_scene->Back().ChildCount(), 3);

    auto it = unittest_scene->Find("E");
    auto parentResult = unittest_scene->DestroyGameObject(it);
    ASSERT_EQUALS(parentResult, unittest_scene->Back());
    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("GameObject"), 5);
    ASSERT_TRUTHY((*unittest_scene->Begin()).name == "A");
    ASSERT_TRUTHY((*unittest_scene->Back()).name == "C");
    ASSERT_EQUALS(unittest_scene->Back().ChildCount(), 2);
}

void Test_Scene_DestroyGameObject_By_Iterator_Children_End() {
    unittest_scene->NewGameObject("A");
    unittest_scene->NewGameObject("B");
    unittest_scene->NewGameObject("C");
    auto parent = unittest_scene->Back();
    unittest_scene->NewGameObject("D", parent);
    unittest_scene->NewGameObject("E", parent);
    unittest_scene->NewGameObject("F", parent);
    ASSERT_EQUALS(unittest_scene->Back().ChildCount(), 3);

    auto it = unittest_scene->Find("F");
    auto parentResult = unittest_scene->DestroyGameObject(it);
    ASSERT_EQUALS(parentResult, unittest_scene->Back());
    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("GameObject"), 5);
    ASSERT_TRUTHY((*unittest_scene->Begin()).name == "A");
    ASSERT_TRUTHY((*unittest_scene->Back()).name == "C");
    ASSERT_EQUALS(unittest_scene->Back().ChildCount(), 2);
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
    REGISTER_TEST_UNIT(Test_Scene_Back);
    REGISTER_TEST_UNIT(Test_Scene_Find_By_GameObject);
    REGISTER_TEST_UNIT(Test_Scene_Find_By_Name_Not_Found);
    REGISTER_TEST_UNIT(Test_Scene_Find_By_Name_Beginning);
    REGISTER_TEST_UNIT(Test_Scene_Find_By_Name_Middle);
    REGISTER_TEST_UNIT(Test_Scene_Find_By_Name_End);
    REGISTER_TEST_UNIT(Test_Scene_Find_By_Name_First_Child);
    REGISTER_TEST_UNIT(Test_Scene_Find_By_Name_Middle_Child);
    REGISTER_TEST_UNIT(Test_Scene_Find_By_Name_Last_Child);
    REGISTER_TEST_UNIT(Test_Scene_DestroyGameObject_By_Iterator_Beginning);
    REGISTER_TEST_UNIT(Test_Scene_DestroyGameObject_By_Iterator_Middle);
    REGISTER_TEST_UNIT(Test_Scene_DestroyGameObject_By_Iterator_End);
    REGISTER_TEST_UNIT(Test_Scene_DestroyGameObject_By_Iterator_Children_Beginning);
    REGISTER_TEST_UNIT(Test_Scene_DestroyGameObject_By_Iterator_Children_Middle);
    REGISTER_TEST_UNIT(Test_Scene_DestroyGameObject_By_Iterator_Children_End);
    // TODO: Implement intensive scene graph manipulation and integrity tests.
}

#endif
