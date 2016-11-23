
#include <gtest/gtest.h>
#include "KLog.h"

int main(int argc, char *argv[])
{
    KLogInit("Kylin-Testing");
    testing::InitGoogleTest(&argc,argv);

    return RUN_ALL_TESTS();
}
