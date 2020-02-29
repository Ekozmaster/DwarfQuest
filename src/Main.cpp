#include<iostream>
#include "Application.h"

int main(int argc, char* args[]) {
    DwarfQuest::Core::Application app = DwarfQuest::Core::Application();

    int appInitialized = app.Init();
    if (!appInitialized) {
        std::cin.get();
        return 1;
    }

    app.Run();
    app.Destroy();

    return 0;
}