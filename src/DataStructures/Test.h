#pragma once

#ifdef DWARFQUEST_TESTING
#include <src/Utils/Testing.h>

void Test_Tree_Setup() {
}

void Test_Tree_TearDown() {
}

void Test_Tree_TreeAdd() {
    /*
    namespace DQTesting = DwarfQuest::Testing;
    
    DQTesting::PrintMemoryDiagnostics();
    DQDS::Tree<int> myTree = DQDS::Tree<int>();
    DQTesting::PrintMemoryDiagnostics();

    myTree.Push(10);
    myTree.Push(0, 0);
    DQDS::Tree<int>::Iterator zero = myTree.Find(0);
    DQDS::Tree<int>::Iterator test = myTree.Push(3, zero);
    myTree.Push(1, zero, 0);
    myTree.Push(2, zero, 1);
    myTree.Find(3);
    DQDS::Tree<int>::Iterator notfound = myTree.Find(19);
    DQDS::Tree<int>::Iterator one = myTree.Find(1);
    myTree.Push(4, one);
    myTree.Push(5, one);
    DQDS::Tree<int>::Iterator two = myTree.Find(2);
    myTree.Push(6, two);
    myTree.Push(7, two);
    DQDS::Tree<int>::Iterator three = myTree.Find(3);
    myTree.Push(8, three);
    myTree.Push(9, three);
    //myTree.Push(9, notfound);
    DQDS::Tree<int>::Iterator ten = myTree.Find(10);

    myTree.Push(11, ten);
    myTree.Push(12, ten);
    myTree.Push(13, ten);
    myTree.Push(14, ten);
    myTree.Push(15, ten);

    DQTesting::PrintMemoryDiagnostics();

    myTree.Erase(myTree.Find(14));
    myTree.Erase(myTree.Find(16));
    myTree.Erase(myTree.Find(15));

    DQTesting::PrintMemoryDiagnostics();
    DQTesting::PrintMemoryDiagnostics();
    */
    ASSERT_EQUALS(1, 1);
}

void Test_Tree_TreeErase() {
    ASSERT_EQUALS(1, 2);
}

void Setup_Tree_Tests() {
    SETUP_ACTIVE_TEST_CONTEXT("Tree data structure's operations");
    REGISTER_TEST_UNIT(Test_Tree_TreeAdd);
    REGISTER_TEST_UNIT(Test_Tree_TreeErase);
    REGISTER_TEST_UNIT_SETUP(Test_Tree_Setup);
    REGISTER_TEST_UNIT_TEAR_DOWN(Test_Tree_TearDown);

    SETUP_ACTIVE_TEST_CONTEXT("Tree tests again");
    REGISTER_TEST_UNIT(Test_Tree_TreeAdd);
    REGISTER_TEST_UNIT(Test_Tree_TreeErase);
    REGISTER_TEST_UNIT_SETUP(Test_Tree_Setup);
    REGISTER_TEST_UNIT_TEAR_DOWN(Test_Tree_TearDown);
}

#endif
