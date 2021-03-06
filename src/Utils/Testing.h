#pragma once

#ifdef DWARFQUEST_TESTING
#include <map>
#include <typeinfo>
#include <iostream>
#include <string>
#include <vector>
#endif

#ifdef DWARFQUEST_TESTING

namespace std {
    template<typename T>
    std::string to_string(const T& s) {
        return typeid(s).name();
    }
}

namespace Testing {

    typedef void (*UnitSetupPointer)();
    typedef void (*UnitTearDownPointer)();
    typedef void (*TestUnitPointer)();

    // UNIT TESTING
    class TestContext {
        std::string m_contextName;

        std::vector<std::pair<std::string, UnitSetupPointer>> m_unitSetups;
        std::vector<std::pair<std::string, UnitTearDownPointer>> m_unitTearDowns;
        std::vector<std::pair<std::string, TestUnitPointer>> m_testUnits;
        std::vector<std::string> m_testingErrorsMessages;

        unsigned int m_currentTestUnitStep; // 0 = Setup, 1 = Run, 2 = Tear Down.
        std::string m_currentTestUnitName;
        bool m_currentUnitHadErrors;
    public:
        TestContext(const char* contextName);
        ~TestContext();

        std::string GetContextName();
        void RegisterUnitSetup(const char* name, UnitSetupPointer unitSetupPointer);
        void RegisterUnitTearDown(const char* name, UnitTearDownPointer unitTearDownPointer);
        void RegisterTestUnit(const char* name, TestUnitPointer testUnitPointer);
        void PushErrorToCurrentUnit(const char* errorString);
        void Run();
        void PrintErrorsIfAny();
    };

    extern std::vector<TestContext*> testContexts;
    extern unsigned int systemTotalOfUnitTests;
    extern TestContext* activeTestContext;
    extern int totalErrorsCount;

    // ### INTERNAL FUNCTIONS
    void SetTestContext(const char* contextName);
    void PushTestUnitSetupToActiveContext(const char* unitName, UnitSetupPointer unitPtr);
    void PushTestUnitTearDownToActiveContext(const char* unitName, UnitTearDownPointer unitPtr);
    void PushTestUnitToActiveContext(const char* unitName, TestUnitPointer unitPtr);
    void RunAllTestContexts();

    template<typename T, typename U>
    void AssertEquals(T a, U b, unsigned int line) {
        if (a != b) {
            std::string error = "Expected '" + std::to_string(a) + "' to be equals to '" + std::to_string(b) + "' at line: " + std::to_string(line);
            activeTestContext->PushErrorToCurrentUnit(error.c_str());
        }
    }

    template<typename T, typename U>
    void AssertNotEquals(T a, U b, unsigned int line) {
        if (a == b) {
            std::string error = "Expected '" + std::to_string(a) + "' to be different than '" + std::to_string(b) + "' at line: " + std::to_string(line);
            activeTestContext->PushErrorToCurrentUnit(error.c_str());
        }
    }

    template<typename T>
    void AssertTruthy(T a, unsigned int line) {
        if (!a) {
            std::string error = "Expected 'falsy' to be 'truthy' at line: " + std::to_string(line);
            activeTestContext->PushErrorToCurrentUnit(error.c_str());
        }
    }

    template<typename T>
    void AssertFalsy(T a, unsigned int line) {
        if (a) {
            std::string error = "Expected 'truthy' to be 'falsy' at line: " + std::to_string(line);
            activeTestContext->PushErrorToCurrentUnit(error.c_str());
        }
    }

    
#define SETUP_ACTIVE_TEST_CONTEXT(testModuleName) Testing::SetTestContext(testModuleName);
#define REGISTER_TEST_UNIT_SETUP(unit) Testing::PushTestUnitSetupToActiveContext(#unit, unit);
#define REGISTER_TEST_UNIT_TEAR_DOWN(unit) Testing::PushTestUnitTearDownToActiveContext(#unit, unit);
#define REGISTER_TEST_UNIT(unit) Testing::PushTestUnitToActiveContext(#unit, unit);

// Assertions
#define ASSERT_EQUALS(x, y) Testing::AssertEquals(x, y, __LINE__);
#define ASSERT_NOT_EQUALS(x, y) Testing::AssertNotEquals(x, y, __LINE__);
#define ASSERT_TRUTHY(x) Testing::AssertTruthy(x, __LINE__);
#define ASSERT_FALSY(x) Testing::AssertFalsy(x, __LINE__);

    // MEMORY DIAGNOSTICS
    typedef struct {
        size_t heapAllocatedMemory = 0;
        std::map<std::string, int> allocatedObjectsCount;
        std::map<std::string, int> allocatedObjectsHeapUsage;
    } MemoryDiagnostics;
    static MemoryDiagnostics memoryDiagnostics;

    void PushObjectAllocationCount(const char* objTypeName, size_t sizeofObj);
    void PopObjectAllocationCount(const char* objTypeName, size_t sizeofObj);
    int GetAllocatedObjectCount(const char* objName);
    size_t GetHeapAllocatedMemory();
    void PrintHeapUsage();
    void PrintObjectsAllocationStatus();
    void PrintMemoryDiagnostics();

#define COUNT_CONSTRUCTOR_CALL(objName) Testing::PushObjectAllocationCount(objName, sizeof(*this));
#define COUNT_DESTRUCTOR_CALL(objName) Testing::PopObjectAllocationCount(objName, sizeof(*this));
}




void* operator new(size_t size);
void operator delete(void* address, size_t size);


#else


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



#endif
