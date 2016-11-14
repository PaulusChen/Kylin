
#include <gtest/gtest.h>
#include <iostream>

using namespace std;

#include "KThreads.h"

void TestKThreadFun(KThread_t *thread,void *param) {
    *(uint32_t *)param = 100;

}

TEST(KThreads,basic) {

    uint32_t testval = 1;
    KThread_t *thread = CreateKThread(TestKThreadFun,
                                      &testval,
                                      KTHREAD_CREATE_THREAD_FLAG_NULL);
    KThreadDestory(thread);
    ASSERT_EQ(testval,100);

}