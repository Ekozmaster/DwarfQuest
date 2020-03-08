#pragma once

#ifdef DWARFQUEST_TESTING
#include <src/Utils/Testing.h>
#include <src/DataStructures/Tree.h>

namespace DQTesting = DwarfQuest::Testing;
namespace DQDS = DwarfQuest::DataStructures;

DQDS::Tree<int>* unittest_Tree = new DQDS::Tree<int>();

void Test_Tree_Setup() {
    unittest_Tree = new DQDS::Tree<int>();
}

void Test_Tree_TearDown() {
    if (unittest_Tree) delete unittest_Tree;
    unittest_Tree = NULL;

    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("Tree::Node"), 0);
}

// TREE PUSH
// Push(int).
void Test_Tree_Push_Int() {
    auto it = unittest_Tree->Push(10);

    ASSERT_NOT_EQUALS(it, unittest_Tree->End());
    ASSERT_EQUALS(*it, 10);
}

// Push(int, Iterator).
void Test_Tree_Push_Int_Parent() {
    auto it = unittest_Tree->Push(10);
    auto it2 = unittest_Tree->Push(0, it);

    ASSERT_NOT_EQUALS(it2, unittest_Tree->End());
    ASSERT_EQUALS(*it2, 0);

    it2.StepUp();
    ASSERT_EQUALS(*it2, 10);
}

// Push(int, Iterator, int).
void Test_Tree_Push_Int_As_First_Child_Of_Parent() {
    auto it = unittest_Tree->Push(10);
    unittest_Tree->Push(1, it);
    unittest_Tree->Push(2, it);

    auto testIt = unittest_Tree->Push(0, it, 0);
    ASSERT_TRUTHY(testIt.IsBreadthBegin());
    ++testIt;
    ASSERT_EQUALS(*testIt, 1);
    testIt.StepUp();
    ASSERT_EQUALS(*testIt, 10);
}

// Push(int, Iterator, int).
void Test_Tree_Push_Int_As_Middle_Child_Of_Parent() {
    auto it = unittest_Tree->Push(10);
    unittest_Tree->Push(1, it);
    unittest_Tree->Push(3, it);

    auto testIt = unittest_Tree->Push(2, it, 1);
    --testIt;
    ASSERT_EQUALS(*testIt, 1);
    ++testIt; ++testIt;
    ASSERT_EQUALS(*testIt, 3);
    testIt.StepUp();
    ASSERT_EQUALS(*testIt, 10);
}

// Push(int, Iterator, int).
void Test_Tree_Push_Int_As_Last_Child_Of_Parent() {
    auto it = unittest_Tree->Push(10);
    unittest_Tree->Push(1, it);
    unittest_Tree->Push(2, it);

    auto testIt = unittest_Tree->Push(3, it);
    --testIt;
    ASSERT_EQUALS(*testIt, 2);
    ++testIt; ++testIt;
    ASSERT_TRUTHY(testIt.IsBreadthEnd());
    --testIt;
    testIt.StepUp();
    ASSERT_EQUALS(*testIt, 10);
}

// TREE ERASE.
void Test_Tree_Erase_From_Beginning() {
    auto it = unittest_Tree->Push(10);
    unittest_Tree->Push(2, it);
    auto testIt = unittest_Tree->Push(1, it, 0);

    // Erase returns parent
    auto erasedParentIt = unittest_Tree->Erase(testIt);
    ASSERT_EQUALS(*erasedParentIt, 10);
    // Stepping down to remaining child.
    erasedParentIt.StepDown();
    ASSERT_EQUALS(*erasedParentIt, 2);
    ASSERT_EQUALS(it.ChildCount(), 1);
}

void Test_Tree_Erase_From_End() {
    auto it = unittest_Tree->Push(10);
    unittest_Tree->Push(1, it);
    auto testIt = unittest_Tree->Push(2, it);

    // Erase returns parent
    auto erasedParentIt = unittest_Tree->Erase(testIt);
    ASSERT_EQUALS(*erasedParentIt, 10);
    // Stepping down to remaining child.
    erasedParentIt.StepDown();
    ASSERT_EQUALS(*erasedParentIt, 1);
    ASSERT_EQUALS(it.ChildCount(), 1);
}

void Test_Tree_Erase_From_Middle() {
    auto it = unittest_Tree->Push(10);
    unittest_Tree->Push(1, it);
    unittest_Tree->Push(3, it);
    auto testIt = unittest_Tree->Push(2, it, 1);

    // Erase returns parent
    auto erasedParentIt = unittest_Tree->Erase(testIt);
    ASSERT_EQUALS(*erasedParentIt, 10);
    // Stepping down to remaining childs.
    erasedParentIt.StepDown();
    ASSERT_EQUALS(*erasedParentIt, 1);
    ++erasedParentIt;
    ASSERT_EQUALS(*erasedParentIt, 3);
    ASSERT_EQUALS(it.ChildCount(), 2);
}

void Test_Tree_Find_At_Beginning() {
    unittest_Tree->Push(0);
    unittest_Tree->Push(5);
    unittest_Tree->Push(10);

    auto it = unittest_Tree->Find(0);
    ASSERT_NOT_EQUALS(it, unittest_Tree->End());
    ASSERT_EQUALS(*it, 0);
    ASSERT_TRUTHY(it.IsBreadthBegin());
}

void Test_Tree_Find_At_End() {
    unittest_Tree->Push(0);
    unittest_Tree->Push(5);
    unittest_Tree->Push(10);

    auto it = unittest_Tree->Find(10);
    ASSERT_NOT_EQUALS(it, unittest_Tree->End());
    ASSERT_EQUALS(*it, 10);
    ++it;
    ASSERT_TRUTHY(it.IsBreadthEnd());
}

void Test_Tree_Find_At_Middle() {
    unittest_Tree->Push(0);
    unittest_Tree->Push(5);
    unittest_Tree->Push(10);

    auto it = unittest_Tree->Find(5);
    ASSERT_NOT_EQUALS(it, unittest_Tree->End());
    ASSERT_EQUALS(*it, 5);
    ++it;
    ASSERT_EQUALS(*it, 10);
    --it; --it;
    ASSERT_EQUALS(*it, 0);
}

void Test_Tree_Destroy() {
    auto ten = unittest_Tree->Push(10);
    unittest_Tree->Push(11, ten);
    unittest_Tree->Push(12, ten);
    unittest_Tree->Push(13, ten);

    auto zero = unittest_Tree->Push(0, 0);
    unittest_Tree->Push(3, zero);
    unittest_Tree->Push(1, zero, 0);
    unittest_Tree->Push(2, zero, 1);

    auto zeroChilds = unittest_Tree->Find(1);
    unittest_Tree->Push(4, zeroChilds);
    unittest_Tree->Push(5, zeroChilds);
    ++zeroChilds;
    unittest_Tree->Push(6, zeroChilds);
    unittest_Tree->Push(7, zeroChilds);
    ++zeroChilds;
    unittest_Tree->Push(8, zeroChilds);
    unittest_Tree->Push(9, zeroChilds);

    auto tenChilds = unittest_Tree->Find(11);
    ++tenChilds;
    auto toEraseIt = unittest_Tree->Push(19, tenChilds);
    auto erasedParentIt = unittest_Tree->Erase(toEraseIt);
    ASSERT_EQUALS(*erasedParentIt, 12);

    unittest_Tree->Destroy();
    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("Tree::Node"), 0);
}


// TREE::Iterator
void Test_Tree_Iterator_Setup() {
    unittest_Tree = new DQDS::Tree<int>();
    //      (0)         (10)
    //    /  |  \      / |  \
    //   1   2   3   11  12  13
    //  / \ / \ / \
    // 4  5 6 7 8 9

    auto ten = unittest_Tree->Push(10);
    unittest_Tree->Push(11, ten);
    unittest_Tree->Push(12, ten);
    unittest_Tree->Push(13, ten);

    auto zero = unittest_Tree->Push(0, 0);
    unittest_Tree->Push(3, zero);
    unittest_Tree->Push(1, zero, 0);
    unittest_Tree->Push(2, zero, 1);

    auto zeroChilds = unittest_Tree->Find(1);
    unittest_Tree->Push(4, zeroChilds);
    unittest_Tree->Push(5, zeroChilds);
    ++zeroChilds;
    unittest_Tree->Push(6, zeroChilds);
    unittest_Tree->Push(7, zeroChilds);
    ++zeroChilds;
    unittest_Tree->Push(8, zeroChilds);
    unittest_Tree->Push(9, zeroChilds);

    auto tenChilds = unittest_Tree->Find(11);
    ++tenChilds;
    auto toEraseIt = unittest_Tree->Push(19, tenChilds);
    auto erasedParentIt = unittest_Tree->Erase(toEraseIt);
}

void Test_Tree_Iterator_TearDown() {
    if (unittest_Tree) delete unittest_Tree;
    unittest_Tree = NULL;

    ASSERT_EQUALS(DQTesting::GetAllocatedObjectCount("Tree::Node"), 0);
}

void Test_Tree_Iterator_Tree_Navigation() {
    auto it = unittest_Tree->Begin();
    ASSERT_NOT_EQUALS(it, unittest_Tree->End());

    // 0
    ASSERT_EQUALS(*it, 0);
    
    ASSERT_EQUALS(*it.StepDown(), 1);
    ASSERT_EQUALS(*++it, 2);
    ASSERT_EQUALS(*++it, 3);
    ASSERT_TRUTHY((++it).IsBreadthEnd());
    ASSERT_EQUALS(*--it, 3);
    ASSERT_EQUALS(*--(--(--it)), 1);
    ASSERT_EQUALS(*it.StepDown(), 4);
    ASSERT_EQUALS(*++it, 5);
    ASSERT_TRUTHY(it.IsDepthEnd());
    ASSERT_EQUALS(*it.StepUp(), 1);
    ASSERT_EQUALS(*++it, 2);
    ASSERT_EQUALS(*it.StepDown(), 6);
    ASSERT_EQUALS(*++it, 7);
    ASSERT_EQUALS(*++(it.StepUp()), 3);
    ASSERT_EQUALS(*it.StepDown(), 8);
    ASSERT_EQUALS(*++it, 9);
    it.StepUp();
    it.StepUp();

    // 10
    ASSERT_EQUALS(*++it, 10);
    ASSERT_EQUALS(*it.StepDown(), 11);
    ASSERT_EQUALS(*++it, 12);
    ASSERT_EQUALS(*++it, 13);

    ASSERT_TRUTHY(it.IsDepthEnd());
    ASSERT_FALSY(it.IsDepthBegin());
    it.StepUp();
    ASSERT_FALSY(it.IsDepthEnd());
    ASSERT_TRUTHY(it.IsDepthBegin());
}


// SETUP
void Setup_Tree_Tests() {
    SETUP_ACTIVE_TEST_CONTEXT("Testing Tree data structure's operations");
    REGISTER_TEST_UNIT_SETUP(Test_Tree_Setup);
    REGISTER_TEST_UNIT_TEAR_DOWN(Test_Tree_TearDown);
    REGISTER_TEST_UNIT(Test_Tree_Push_Int);
    REGISTER_TEST_UNIT(Test_Tree_Push_Int_Parent);
    REGISTER_TEST_UNIT(Test_Tree_Push_Int_As_First_Child_Of_Parent);
    REGISTER_TEST_UNIT(Test_Tree_Push_Int_As_Middle_Child_Of_Parent);
    REGISTER_TEST_UNIT(Test_Tree_Push_Int_As_Last_Child_Of_Parent);
    REGISTER_TEST_UNIT(Test_Tree_Erase_From_Beginning);
    REGISTER_TEST_UNIT(Test_Tree_Erase_From_End);
    REGISTER_TEST_UNIT(Test_Tree_Erase_From_Middle);
    REGISTER_TEST_UNIT(Test_Tree_Find_At_Beginning);
    REGISTER_TEST_UNIT(Test_Tree_Find_At_End);
    REGISTER_TEST_UNIT(Test_Tree_Find_At_Middle);
    REGISTER_TEST_UNIT(Test_Tree_Destroy);

    SETUP_ACTIVE_TEST_CONTEXT("Testing Tree::Iterator operations");
    REGISTER_TEST_UNIT_SETUP(Test_Tree_Iterator_Setup);
    REGISTER_TEST_UNIT_TEAR_DOWN(Test_Tree_Iterator_TearDown);
    REGISTER_TEST_UNIT(Test_Tree_Iterator_Tree_Navigation);
}

#endif
