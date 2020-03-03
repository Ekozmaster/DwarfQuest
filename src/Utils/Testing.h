#pragma once

#ifdef DWARFQUEST_TESTING
#include<map>
#include<typeinfo>
#endif
namespace DwarfQuest {
    namespace Testing {

#ifdef DWARFQUEST_TESTING
        std::map<std::string, int> allocatedObjectsCount;
        #define COUNT_CONSTRUCTOR_CALL(x) DwarfQuest::Testing::allocatedObjectsCount[typeid(*x).name()]++;
        #define COUNT_DESTRUCTOR_CALL(x) DwarfQuest::Testing::allocatedObjectsCount[typeid(*x).name()]--;
#else
        #define COUNT_CONSTRUCTOR_CALL(x)
        #define COUNT_DESTRUCTOR_CALL(x)
#endif
    }
}