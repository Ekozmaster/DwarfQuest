#pragma once

#ifdef DWARFQUEST_TESTING
#include <map>
#include <typeinfo>
#include <iostream>
#include <string>
#include <vector>
#endif

#ifdef DWARFQUEST_TESTING

namespace DwarfQuest {
    namespace Testing {

        typedef typename void (*UnitSetupPointer)();
        typedef typename void (*UnitTearDownPointer)();
        typedef typename void (*TestUnitPointer)();

        // UNIT TESTING
        class TestContext {
            std::string m_contextName;

            std::vector<std::pair<std::string, UnitSetupPointer>> m_unitSetups;
            std::vector<std::pair<std::string, UnitTearDownPointer>> m_unitTearDowns;
            std::vector<std::pair<std::string, TestUnitPointer>> m_testUnits;

            std::string m_currentTestUnitName;
            bool m_currentUnitHadErrors;
        public:
            TestContext(const char* contextName) : m_contextName(contextName) {
                m_currentTestUnitName = "";
                m_currentUnitHadErrors = false;
            }
            ~TestContext() {}

            std::string GetContextName();
            void RegisterUnitSetup(const char* name, UnitSetupPointer unitSetupPointer);
            void RegisterUnitTearDown(const char* name, UnitTearDownPointer unitTearDownPointer);
            void RegisterTestUnit(const char* name, TestUnitPointer testUnitPointer);
            void PushErrorToCurrentUnit(const char* errorString);
            void Run();
        };

        extern std::vector<std::string> testingErrorsMessages;
        extern std::vector<TestContext*> testContexts;
        extern unsigned int systemTotalOfUnitTests;
        extern TestContext* activeTestContext;

        // ### INTERNAL FUNCTIONS
        void SetTestContext(const char* contextName);
        void PushTestUnitSetupToActiveContext(const char* unitName, UnitSetupPointer unitPtr);
        void PushTestUnitTearDownToActiveContext(const char* unitName, UnitTearDownPointer unitPtr);
        void PushTestUnitToActiveContext(const char* unitName, TestUnitPointer unitPtr);
        void RunAllTestContexts();

        template<typename T>
        void AssertEquals(T a, T b, unsigned int line) {
            if (a != b) {
                std::string error = "Expected '" + std::to_string(a) + "' to be equals to '" + std::to_string(b) + "' at line: " + std::to_string(line);
                activeTestContext->PushErrorToCurrentUnit(error.c_str());
            }
        }

        
#define SETUP_ACTIVE_TEST_CONTEXT(testModuleName) DwarfQuest::Testing::SetTestContext(testModuleName);
#define REGISTER_TEST_UNIT_SETUP(unit) DwarfQuest::Testing::PushTestUnitSetupToActiveContext(#unit, unit);
#define REGISTER_TEST_UNIT_TEAR_DOWN(unit) DwarfQuest::Testing::PushTestUnitTearDownToActiveContext(#unit, unit);
#define REGISTER_TEST_UNIT(unit) DwarfQuest::Testing::PushTestUnitToActiveContext(#unit, unit);

// Assertions
#define ASSERT_EQUALS(x, y) DwarfQuest::Testing::AssertEquals(x, y, __LINE__);

        // MEMORY DIAGNOSTICS
        typedef struct {
            size_t heapAllocatedMemory = 0;
            std::map<std::string, int> allocatedObjectsCount;
            std::map<std::string, int> allocatedObjectsHeapUsage;
        } MemoryDiagnostics;
        static MemoryDiagnostics memoryDiagnostics;

        void PushObjectAllocationCount(const char* objTypeName, size_t sizeofObj);
        void PopObjectAllocationCount(const char* objTypeName, size_t sizeofObj);
        size_t GetHeapAllocatedMemory();
        void PrintHeapUsage();
        void PrintObjectsAllocationStatus();
        void PrintMemoryDiagnostics();

#define COUNT_CONSTRUCTOR_CALL DwarfQuest::Testing::PushObjectAllocationCount(typeid(*this).name(), sizeof(*this)); // memoryDiagnostics.allocatedObjectsCount[typeid(*this).name()]++;
#define COUNT_DESTRUCTOR_CALL DwarfQuest::Testing::PopObjectAllocationCount(typeid(*this).name(), sizeof(*this)); //memoryDiagnostics.allocatedObjectsCount[typeid(*this).name()]--;
    }
}



void* operator new(size_t size);
void operator delete(void* address, size_t size);


#else


namespace DwarfQuest {
    namespace Testing {
#define SETUP_ACTIVE_TEST_CONTEXT
#define REGISTER_TEST_UNIT_SETUP(unit)
#define REGISTER_TEST_UNIT_TEAR_DOWN(unit)
#define REGISTER_TEST_UNIT(unit)

// Assertions
#define ASSERT_EQUALS(x, y)

#define COUNT_CONSTRUCTOR_CALL(x)
#define COUNT_DESTRUCTOR_CALL(x)
    }
}


#endif
