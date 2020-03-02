#include<iostream>

#include <src/Application.h>
#include <src/DataStructures/Tree.h>


int main(int argc, char* args[]) {
    namespace DQCore = DwarfQuest::Core;
    namespace DQDS = DwarfQuest::DataStructures;
    

    DQDS::Tree<int> myTree = DQDS::Tree<int>();


    myTree.Push(10);
    myTree.Push(0, 0);
    /*
    DQDS::Tree<int>::Iterator zero = myTree.Find(0);
    myTree.Push(3, zero);
    DQDS::Tree<int>::Iterator one = myTree.Push(1, zero, 0);
    DQDS::Tree<int>::Iterator two = myTree.Push(2, zero, 1);
    DQDS::Tree<int>::Iterator three = myTree.Find(3);
    myTree.Push(4, one);
    myTree.Push(5, one);
    myTree.Push(6, two);
    myTree.Push(7, two);
    myTree.Push(8, three);
    myTree.Push(9, three);
    DQDS::Tree<int>::Iterator ten = myTree.Find(10);
    myTree.Push(11, ten);
    myTree.Push(12, ten);
    myTree.Push(13, ten);
    myTree.Push(14, ten);
    myTree.Push(15, ten);
    myTree.Erase(myTree.Find(14));
    myTree.Erase(myTree.Find(16));
    myTree.Erase(myTree.Find(15));
    */
    while (std::cin.get() != '\n');

    /*
    DQCore::Application app = DwarfQuest::Core::Application();

    int appInitialized = app.Init();
    if (!appInitialized) {
        std::cin.get();
        return 1;
    }

    app.Run();
    app.Destroy();
    */
    return 0;
}
