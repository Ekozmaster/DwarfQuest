#include<iostream>

#include "Application.h"

int main(int argc, char* args[]) {
    Application app = Application();

    int appInitialized = app.Init();
    if (!appInitialized) {
        std::cin.get();
        return 1;
    }

    app.Run();

    return 0;
}