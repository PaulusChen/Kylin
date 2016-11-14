
#include <gtest/gtest.h>
#include <iostream>

using namespace std;

#include "KBuffer.h"

KBufferDeclar(int)
KBufferImpl(int)

TEST(KBuffer,basic) {

    KBuffer(int,buffer) testVec;
    KBuffer(int,Init)(&testVec,GetKB(1));

    for (int i = 0; i != GetKB(3); i++) {
        KBuffer(int,AppendTail)(&testVec,GetKB(5) - i);
    }

    cout<<KBuffer(int,GetLen)(&testVec)<<endl;

    for (int i = 0; i != GetKB(3); i++) {
    }
}
