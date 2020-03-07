#include <src/Utils/Testing.h>


#ifdef DWARFQUEST_TESTING
namespace DwarfQuest {
    namespace Testing {

        // ### TestContext
        std::string TestContext::GetContextName() {
            return m_contextName;
        }

        void TestContext::RegisterUnitSetup(const char* name, UnitSetupPointer unitSetupPointer) {
            m_unitSetups.push_back(std::pair<std::string, UnitSetupPointer>(name, unitSetupPointer));
        }

        void TestContext::RegisterUnitTearDown(const char* name, UnitTearDownPointer unitTearDownPointer) {
            m_unitTearDowns.push_back(std::pair<std::string, UnitSetupPointer>(name, unitTearDownPointer));
        }

        void TestContext::RegisterTestUnit(const char* name, TestUnitPointer testUnitPointer) {
            m_testUnits.push_back(std::pair<std::string, UnitSetupPointer>(name, testUnitPointer));
        }

        void TestContext::PushErrorToCurrentUnit(const char* errorString) {
            auto foo = this;
            testingErrorsMessages.push_back("[FAILED]: " + m_contextName + " > "  + m_currentTestUnitName + " - " + errorString);
            m_currentUnitHadErrors = true;
        }

        void TestContext::Run() {
            for (auto unitIt = m_testUnits.begin(); unitIt != m_testUnits.end(); ++unitIt) {
                m_currentUnitHadErrors = false;
                // Setups
                for (auto it = m_unitSetups.begin(); it != m_unitSetups.end(); ++it) it->second();

                // Unit
                m_currentTestUnitName = unitIt->first;
                unitIt->second();

                // Tear Downs
                for (auto it = m_unitTearDowns.begin(); it != m_unitTearDowns.end(); ++it) it->second();

                if (m_currentUnitHadErrors) std::cout << "x";
                else std::cout << ".";
            }
        }

        std::vector<std::string> testingErrorsMessages;
        std::vector<TestContext*> testContexts;
        unsigned int systemTotalOfUnitTests = 0;
        TestContext* activeTestContext = NULL;

        // ### INTERNAL FUNCTIONS
        void SetTestContext(const char* contextName) {
            for (auto it = testContexts.begin(); it != testContexts.end(); ++it) {
                if ((*it)->GetContextName() == contextName) {
                    activeTestContext = *it;
                    return;
                }
            }
            TestContext* testContext = new TestContext(contextName);
            DwarfQuest::Testing::testContexts.push_back(testContext);
            activeTestContext = testContext;
        }

        void PushTestUnitSetupToActiveContext(const char* unitName, UnitSetupPointer unitPtr) {
            if (!activeTestContext) throw std::exception("No active TestContext to register test units's setup.");
            activeTestContext->RegisterUnitSetup(unitName, unitPtr);
        }

        void PushTestUnitTearDownToActiveContext(const char* unitName, UnitTearDownPointer unitPtr) {
            if (!activeTestContext) throw std::exception("No active TestContext to register test units's tear down.");
            activeTestContext->RegisterUnitTearDown(unitName, unitPtr);
        }

        void PushTestUnitToActiveContext(const char* unitName, TestUnitPointer unitPtr) {
            if (!activeTestContext) throw std::exception("No active TestContext to register test units.");
            activeTestContext->RegisterTestUnit(unitName, unitPtr);
            systemTotalOfUnitTests++;
        }

        void RunAllTestContexts() {
            for (auto it = testContexts.begin(); it != testContexts.end(); ++it) {
                activeTestContext = *it;
                (*it)->Run();
            }

            std::cout << std::endl;
            std::cout << "Finished with a total of " << std::to_string(systemTotalOfUnitTests) << " unit tests performed." << std::endl;
            if (testingErrorsMessages.size()) {
                std::cout << "Errors: " << testingErrorsMessages.size() << std::endl;
            }

            std::cout << std::endl << "Error Messages: " << std::endl;
            for (auto it = testingErrorsMessages.begin(); it != testingErrorsMessages.end(); ++it) {
                std::cout << *it << std::endl;
            }
        }


        // MEMORY DIAGNOSTICS
        void PushObjectAllocationCount(const char* objTypeName, size_t sizeofObj) {
            memoryDiagnostics.allocatedObjectsCount[objTypeName]++;
            memoryDiagnostics.allocatedObjectsHeapUsage[objTypeName] += sizeofObj;
        }

        void PopObjectAllocationCount(const char* objTypeName, size_t sizeofObj) {
            memoryDiagnostics.allocatedObjectsCount[objTypeName]--;
            memoryDiagnostics.allocatedObjectsHeapUsage[objTypeName] -= sizeofObj;
        }

        size_t GetHeapAllocatedMemory() {
            return memoryDiagnostics.heapAllocatedMemory;
        }

        void PrintHeapUsage() {
            std::cout << "Heap memory usage: " << GetHeapAllocatedMemory() << std::endl;
        }

        void PrintObjectsAllocationStatus() {
            std::cout << "Allocations status:" << std::endl;
            if (memoryDiagnostics.allocatedObjectsCount.size() > 0) {
                for (auto it = memoryDiagnostics.allocatedObjectsCount.begin(); it != memoryDiagnostics.allocatedObjectsCount.end(); ++it) {
                    size_t objHeapUsage = memoryDiagnostics.allocatedObjectsHeapUsage[it->first];
                    std::cout << " - " << it->first << " has " << it->second << " allocated instances (" << objHeapUsage << " bytes)" << std::endl;
                }
            } else {
                std::cout << "No Allocations tracked." << std::endl;
            }
        }

        void PrintMemoryDiagnostics() {
            std::cout << "# Memory Diagnostics" << std::endl;
            PrintHeapUsage();
            PrintObjectsAllocationStatus();
            std::cout << std::endl;
        }
    }
}

void* operator new(size_t size) {
    DwarfQuest::Testing::memoryDiagnostics.heapAllocatedMemory += size;
    return malloc(size);
}

void operator delete(void* address, size_t size) {
    DwarfQuest::Testing::memoryDiagnostics.heapAllocatedMemory -= size;
    free(address);
}

#endif