#include <src/Utils/Testing.h>


#ifdef DWARFQUEST_TESTING
namespace DwarfQuest {
    namespace Testing {

        // ### TestContext
        TestContext::TestContext(const char* contextName) : m_contextName(contextName) {
            m_currentTestUnitName = "";
            m_currentUnitHadErrors = false;
            m_currentTestUnitStep = 0;
        }

        TestContext::~TestContext() {}

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
            m_testingErrorsMessages.push_back("[FAILED]: " + m_currentTestUnitName + " - " + errorString);
            m_currentUnitHadErrors = true;
            hadAnyErrors = true;
        }

        void TestContext::Run() {
            for (auto unitIt = m_testUnits.begin(); unitIt != m_testUnits.end(); ++unitIt) {
                m_currentTestUnitName = unitIt->first;
                m_currentUnitHadErrors = false;

                // Setups
                m_currentTestUnitStep = 0;
                for (auto it = m_unitSetups.begin(); it != m_unitSetups.end(); ++it) it->second();

                // Run
                m_currentTestUnitStep = 1;
                unitIt->second();

                // Tear Downs
                m_currentTestUnitStep = 2;
                for (auto it = m_unitTearDowns.begin(); it != m_unitTearDowns.end(); ++it) it->second();

                if (m_currentUnitHadErrors) std::cout << "x";
                else std::cout << ".";
            }
        }

        void TestContext::PrintErrorsIfAny() {
            if (m_testingErrorsMessages.size() == 0) return;
            std::cout << std::endl << "=== " << m_contextName << std::endl;
            for (auto it = m_testingErrorsMessages.begin(); it != m_testingErrorsMessages.end(); ++it) {
                std::cout << *it << std::endl;
            }
        }

        std::vector<std::string> testingErrorsMessages;
        std::vector<TestContext*> testContexts;
        unsigned int systemTotalOfUnitTests = 0;
        TestContext* activeTestContext = NULL;
        bool hadAnyErrors = false;

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
                std::cout << "Total Errors: " << testingErrorsMessages.size() << std::endl;
            }

            if (hadAnyErrors) {
                std::cout << std::endl << "Error Messages: " << std::endl;
                for (auto it = testContexts.begin(); it != testContexts.end(); ++it) (*it)->PrintErrorsIfAny();
            } else {
                std::cout << "All tests passed." << std::endl;
            }

            std::cout << std::endl;
            PrintMemoryDiagnostics();

            std::cout << std::endl;
            char answer[10];
            std::cout << "Type 'exit' to finish." << std::endl;
            do {
                std::cin.getline(answer, 10);
            } while (std::strcmp(answer, "exit") != 0);

            for (auto it = testContexts.begin(); it != testContexts.end(); ++it) delete (*it);
            testContexts.clear();
        }


        // MEMORY DIAGNOSTICS
        void PushObjectAllocationCount(const char* objName, size_t sizeofObj) {
            memoryDiagnostics.allocatedObjectsCount[objName]++;
            memoryDiagnostics.allocatedObjectsHeapUsage[objName] += sizeofObj;
        }

        void PopObjectAllocationCount(const char* objName, size_t sizeofObj) {
            memoryDiagnostics.allocatedObjectsCount[objName]--;
            memoryDiagnostics.allocatedObjectsHeapUsage[objName] -= sizeofObj;
        }

        int GetAllocatedObjectCount(const char* objName) {
            return memoryDiagnostics.allocatedObjectsCount[objName];
        }

        size_t GetHeapAllocatedMemory() {
            return memoryDiagnostics.heapAllocatedMemory;
        }

        void PrintHeapUsage() {
            std::cout << "Heap memory usage: " << GetHeapAllocatedMemory() << " bytes" << std::endl;
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