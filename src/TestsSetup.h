#pragma once

#ifdef DWARFQUEST_TESTING
#include <src/DataStructures/Test.h>
#include <src/SceneManagement/Test.h>

void SetupSystemTests() {
    Setup_Tree_Tests();
    Setup_Scene_Tests();
}

#endif