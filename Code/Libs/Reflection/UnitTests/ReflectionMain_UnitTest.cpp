/*
   GameRiff - Framework for creating various video game services
   Reflection unit tests
   Copyright (C) 2011, Shaun Leach.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
  
       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.
  
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <gtest/gtest.h>

#include "Pch.h"

LOG_DEFINE_MODULE(Reflection Unit Tests);

using ::testing::InitGoogleTest;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestResult;
using ::testing::TestPartResult;
using ::testing::UnitTest;
using ::testing::EmptyTestEventListener;

// Provides alternative output mode which produces minimal amount of
// information about tests.
class TestLogger : public EmptyTestEventListener {
private:
    // Called before any test activity starts.
    virtual void OnTestProgramStart(const UnitTest& /* unit_test */) {}

    // Called after all test activities have ended.
    virtual void OnTestProgramEnd(const UnitTest & unitTest) {
        LOG(
            LOG_PRIORITY_INFO, 
            L"%d of %d tests passed.", 
            unitTest.successful_test_count(),
            unitTest.total_test_count()
        );
    }

    // Called before a test starts.
    virtual void OnTestStart(const TestInfo& test_info) {
    }

    // Called after a failed assertion or a SUCCEED() invocation.
    virtual void OnTestPartResult(const TestPartResult & testPartResult) {
        if (testPartResult.failed()) {
            LOGF(
                LOG_PRIORITY_ERROR, 
                testPartResult.file_name(), 
                testPartResult.line_number(), 
                "FAILED: TEST -- %S", 
                testPartResult.summary()
            );
        }
    }

    // Called after a test ends.
    virtual void OnTestEnd(const TestInfo & testInfo) {
        const TestResult & result = *testInfo.result();
        if (result.Failed()) {
            LOG(LOG_PRIORITY_ERROR, L"TEST: %S:%S -- FAILED!!", testInfo.name(), testInfo.test_case_name());
        }
        LOG(
            LOG_PRIORITY_INFO, 
            L"%s : TEST: %S:%S", 
            result.Passed() ? L"PASSED" : L"FAILED",
            testInfo.test_case_name(),
            testInfo.name()
        );
    }
};  // class TestLogger

//====================================================
int main(int argc, char **argv) {
    LogInit();

    bool loop = false;
    unsigned count = 0;
    while(loop) {
        count++;
    }
    ReflInitialize();

    InitGoogleTest(&argc, argv);

    UnitTest            & unit_test = *UnitTest::GetInstance();
    TestEventListeners  & listeners = unit_test.listeners();
    listeners.Append(new TestLogger);

    int ret_val = RUN_ALL_TESTS();

    LogClose();

    return ret_val;
}
