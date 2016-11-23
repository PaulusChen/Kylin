
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
    cout<<" testval :"<<*testval<<endl;
}

TEST(KHttpHelper,base) {
    KHttpHelper_t *helper = KCreateHttpHelper(10);
    int *testval = new int;
    *testval = 100;

    for(int i = 0 ; i != GetKB(100); ++i) {
        KHttpHelperRequestRange(helper,
                                "http://localhost/testfile",
                                i * 80,
                                GetKB(10),
                                testKHttpHelperGetDataHandler,
                                testval);
    }

    sleep(10000);

    KDestoryHttpHelper(helper);
}

