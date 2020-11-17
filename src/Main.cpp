#include<iostream>

#include <src/Application.h>

#ifdef DWARFQUEST_TESTING
#include <src/DataStructures/Test.h>
#include <src/TestsSetup.h>
#endif


int main(int argc, char* args[]) {
#ifdef DWARFQUEST_TESTING
    SetupSystemTests();
    Testing::RunAllTestContexts();
#else

    Core::Application app = Core::Application();

    int appInitialized = app.Init();
    if (!appInitialized) {
        std::cin.get();
        return 1;
    }

    app.Run();
    app.Destroy();
    
    
#endif
    return 0;
}