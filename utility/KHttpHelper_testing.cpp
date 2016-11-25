
#include <gtest/gtest.h>
#include <iostream>

#include "KHttpHelper.h"
#include "KLog.h"

using namespace std;


void testKHttpHelperGetDataHandler(KHttpHelperRequestTask_t *task,void *param) {

    int status = KHttpHelperTaskGetStatus(task);
    if (status != KE_OK) {
        KLogErr("Http Get Failed.");
        return ;
    }

    const char *contentType = KHttpHelperTaskGetContentType(task);
    cout<<"Content Type : "<<contentType;

    uint64_t contentLen = KHttpHelperTaskGetContentLen(task);
    cout<<" Content Length : "<<contentLen;

    int *testval = (int *)param;
    __sync_fetch_and_sub(testval,1);
    cout<<" testval :"<<*testval<<endl;
}

TEST(KHttpHelper,base) {
    KHttpHelper_t *helper = KCreateHttpHelper(1);
    int *testval = new int;
    *testval = 0;

    for(int i = 0 ; i != 1; ++i) {
        int curTestVal = __sync_fetch_and_add(testval,1);
        if (curTestVal > 200) {
            cout<<".";
            sched_yield();
            continue;
        }

        KHttpHelperRequestRange(helper,
                                "http://localhost/testfile",
                                i * 80,
                                GetKB(10),
                                testKHttpHelperGetDataHandler,
                                testval);
    }

    sleep(1000);

    KDestoryHttpHelper(helper);
}

