
#include <gtest/gtest.h>
#include <iostream>

#include "KHttpHelper.h"

using namespace std;

TEST(KHttpHelper,base) {
    KHttpHelper_t *helper = KCreateHttpHelper("http://localhost/testfile");
    int64_t contentLen = KHttpHelperGetContentLen(helper);
    ASSERT_GT(contentLen,0);

    uint8_t buf[GetMB(1)];
    uint64_t len = GetKB(512);
    KHttpHelperDownloadRange(helper,buf,0,&len);

    char contentType[1024];
    KHttpHelperGetContentType(helper,contentType,1024);
    cout<<contentType<<endl;

    ASSERT_NE(0,len);
}

