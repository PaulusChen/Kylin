
#include <gtest/gtest.h>
#include <iostream>

#include "KHashTable.h"
#include "KTools.h"

using namespace std;

typedef struct {
    uint64_t span;
    double data1;
    uint64_t data2;
    KHListNode node;
    uint32_t Key;
} KHTTest;

static inline int nodeCmp(const uint64_t *keyA,const uint64_t *keyB) {
    return *keyA - *keyB;
}

HashTemplateDeclar(KHTTest,KHTTest,data2,node,nodeCmp) {
    return *key;
}

HashTemplateImpl(KHTTest,data2,node)

TEST(KHASH_TABLE,AddAndBasic) {

    KHTable htable;
    KHashTable(KHTTest,Init)(&htable,1024,0);
    KHTTest test1;
    test1.data1 = 19.22;
    test1.data2 = 0x12345678;
    KHashTable(KHTTest,Insert)(&htable,&test1);

    KHTTest test2;
    test2.data1 = 20.11;
    test2.data2 = 0x87654321;
    KHashTable(KHTTest,Insert)(&htable,&test2);

    KHTTest test3;
    test3.data1 = 0.11;
    test3.data2 = 0x43218765;
    KHashTable(KHTTest,Insert)(&htable,&test3);

    uint64_t key = 0x87654321;
    KHTTest *p = KHashTable(KHTTest,Search)(&htable,&key);
    ASSERT_DOUBLE_EQ(p->data1,20.11);

    KHashTable(KHTTest,Delete)(&htable,&test2);
    key = 0x87654321;
    p = KHashTable(KHTTest,Search)(&htable,&key);
    ASSERT_EQ(NULL,p);
}

void *gdslGetKey(void *test) {
    return &((KHTTest *)test)->data2;
}

ulong gdslHash(void *key) {
    return *(ulong *)key;
}



TEST(KHASH_TABLE,AddAndRemoveCorrectness) {

    const uint64_t testSize = GetMB(1);
    KHTable htable;
    KHashTable(KHTTest,Init)(&htable,testSize,0);

    uint64_t *keyPool = new uint64_t[testSize];
    for (int i = 0; i != testSize; i++) {
        uint64_t key = rand();
        keyPool[i] = key;
    }
    KHTTest *testArray = new KHTTest[testSize];
    srand(0x1234);

    PerfToolsTimer timer;
    PerfToolsTimerReset(&timer);
    int i = 0;
    for (i = 0; i != testSize; i++) {
        KHTTest *cur = &testArray[i];
        uint64_t key = keyPool[i];
        cur->data1 = 19.22;
        cur->data2 = key;
        if(KHashTable(KHTTest,Insert)(&htable,cur) != KE_OK) {
            FAIL();
            cout<<"error!!!!!!!"<<endl;
            break;
        }
    }

    uint64_t testTime = PerfToolsTimerSpan(&timer);
    cout<<"Insertion Lasted : "<<testTime<<" milliseconds"<<endl;

    ASSERT_EQ(i,testSize);

    PerfToolsTimerReset(&timer);
    uint64_t sum = 0;
    KHTTest *p = NULL;
    for (i = 0; i != testSize; i++) {
        uint64_t key = keyPool[i];
        p = KHashTable(KHTTest,Search)(&htable,&key);
        if (p == NULL) {
            p = KHashTable(KHTTest,Search)(&htable,&key);
            break;
        }
        sum += p->data2;
        if (p->data2 != key) {
            break;
        }

        if (p->data1 != 19.22) {
            break;
        }
    }
    testTime = PerfToolsTimerSpan(&timer);
    cout<<"searching Lasted : "<<testTime<<" milliseconds."<<endl;
    ASSERT_EQ(KHashTableDebugCount(&htable),testSize);
    ASSERT_EQ(KHashTableDebugCount(&htable),KHashTableGetTotalCount(&htable));

    PerfToolsTimerReset(&timer);
    p = NULL;
    for (i = 0; i != testSize; i++) {
        uint64_t key = keyPool[i];
        p = KHashTable(KHTTest,DeleteByKey)(&htable,&key);
        if (p == NULL) {
            FAIL();
        }
        sum += p->data2;
        if (p->data2 != key) {
            FAIL();
        }

        if (p->data1 != 19.22) {
            FAIL();
        }
    }

    testTime = PerfToolsTimerSpan(&timer);
    cout<<"Deleting Lasted : "<<testTime<<" milliseconds."<<endl;

    cout<<"Into-> longest list : "<<KHashTableGetMaxSlotLen(&htable)<<" slot : "<<KHashTableGetSlotCount(&htable)<<" test size : "<<testSize<<endl;
    ASSERT_EQ(i,testSize);
    ASSERT_EQ(KHashTableDebugCount(&htable),0);
    ASSERT_EQ(KHashTableDebugCount(&htable),KHashTableGetTotalCount(&htable));

    delete []testArray;
    delete []keyPool;
}