#include<iostream>

#include <src/Application.h>
#include <src/DataStructures/Tree.h>
#include <src/Utils/Testing.h>

int main(int argc, char* args[]) {
    namespace DQCore = DwarfQuest::Core;
    namespace DQDS = DwarfQuest::DataStructures;
    

    DQDS::Tree<int> myTree = DQDS::Tree<int>();


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

    using namespace DwarfQuest::Testing;
    std::cout << allocatedObjectsCount.size();
    for (auto it = allocatedObjectsCount.begin(); it != allocatedObjectsCount.end(); ++it) {
        std::cout << it->first << " :: " << it->second << std::endl;
    }
    myTree.Push(11, ten);
    myTree.Push(12, ten);
    myTree.Push(13, ten);
    myTree.Push(14, ten);
    myTree.Push(15, ten);

    
    /*
>>>>>>> b0a2467465c60f3b05101541c1baa34991293828
    myTree.Erase(myTree.Find(14));
    myTree.Erase(myTree.Find(16));
    myTree.Erase(myTree.Find(15));

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
