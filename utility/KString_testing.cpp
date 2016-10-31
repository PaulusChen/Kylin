
#include <gtest/gtest.h>
#include <iostream>

#include "KString.h"

#define TEST_BUF_LEN GetKB(10)

TEST(KString,basic) {
    KStrRebuilder_t *rebuilder = CreateKStrRebuilder("(^http://).+\\.gnu.org/.+(/.+$)","We got substring [${0}] and [${1}] and [${2}]");

    const char *testStr = "http://www.gnu.org/software/findutils/manual/html_node/find_html/findutils_002ddefault-regular-expression-syntax.html#findutils_002ddefault-regular-expression-syntax";

    char result[TEST_BUF_LEN];
    KStrRebuilderBuild(rebuilder,testStr,result,TEST_BUF_LEN);
    DestoryKStrRebuilder(rebuilder);

    printf("%s",result);
}

TEST(KString,basic2) {
    KStrRebuilder_t *rebuilder = CreateKStrRebuilder("(^http://).+\\.gnu.org/.+(/.+$)","${0}-----${1}-----${2}");

    const char *testStr = "http://www.gnu.org/software/findutils/manual/html_node/find_html/findutils_002ddefault-regular-expression-syntax.html#findutils_002ddefault-regular-expression-syntax";

    char result[TEST_BUF_LEN];
    KStrRebuilderBuild(rebuilder,testStr,result,TEST_BUF_LEN);
    DestoryKStrRebuilder(rebuilder);

    printf("%s",result);

}