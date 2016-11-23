
#include <gtest/gtest.h>
#include <iostream>

#include "KHttpHelper.h"
#include "KLog.h"

using namespace std;


void testKHttpHelperGetDataHandler(KHttpHelperRequestTask_t *task) {

    int status = KHttpHelperTaskGetStatus(task);
    if (status != KE_OK) {
        KLogErr("Http Get Failed.");
        return ;
    }

    const char *contentType = KHttpHelperTaskGetContentType(task);
    cout<<"Content Type : "<<contentType<<endl;

    uint64_t contentLen = KHttpHelperTaskGetContentLen(task);
    cout<<"Content Length : "<<contentLen<<endl;

}

TEST(KHttpHelper,base) {
    KHttpHelper_t *helper = KCreateHttpHelper(10);
    KHttpHelperRequestRange(helper,
                            "http://localhost/testfile",
                            0,
                            GetKB(10),
                            testKHttpHelperGetDataHandler);

    sleep(100);

    KDestoryHttpHelper(helper);
}

