
#include <gtest/gtest.h>
#include <iostream>

#include "KList.h"

using namespace std;

typedef struct {
    double data1;
    KListNode node;
    int data2;
} KListTest;

TEST(KList,AddAndRemove) {
    KListNode headNode;
    KListInitHead(&headNode);

    KListTest testList1;
    testList1.data1 = 19.22;
    testList1.data2 = 0x12345678;
    KListAddTail(&headNode,&testList1.node);

    KListTest testList2;
    testList2.data1 = 20.11;
    testList2.data2 = 0x87654321;
    KListAddHead(&testList1.node,&testList2.node);

    KListTest testList3;
    testList3.data1 = 0.11;
    testList3.data2 = 0x43218765;
    KListAddTail(&testList1.node,&testList3.node);

    KListNode *pCurrent(NULL);
    int count(0);
    KListForEach(pCurrent,&headNode) {

        KListTest *pKListTest = KListEntry(pCurrent,KListTest,node);
        ++count;
        switch(count) {
        case 3:
            ASSERT_EQ(pKListTest->data1,20.11);
            ASSERT_EQ(pKListTest->data2,0x87654321);
            break;
        case 2:
            ASSERT_EQ(pKListTest->data1,19.22);
            ASSERT_EQ(pKListTest->data2,0x12345678);
            break;
        case 1:
            ASSERT_EQ(pKListTest->data1,0.11);
            ASSERT_EQ(pKListTest->data2,0x43218765);
            break;
        default:
            FAIL();
            break;
        }
    }

    KListDel(&testList2.node);
    count = 0;
    KListForEach(pCurrent,&headNode) {
        KListTest *pKListTest = KListEntry(pCurrent,KListTest,node);
        ++count;
        switch(count) {
        case 2:
            ASSERT_EQ(pKListTest->data1,19.22);
            ASSERT_EQ(pKListTest->data2,0x12345678);
            break;
        case 1:
            ASSERT_EQ(pKListTest->data1,0.11);
            ASSERT_EQ(pKListTest->data2,0x43218765);
            break;
        default:
            FAIL();
            break;
        }
    }
}


TEST(VSTACK,VSTACKPopPush) {

    KStack testStackObj;
    KStack *testStack = &testStackObj;
    KStackInit(testStack);
    int a = 1;
    int b = 2;
    int c = 5;
    int d = 10;

    ASSERT_TRUE(KStackIsEmpty(testStack));
    KStackPush(testStack,&a);
    ASSERT_FALSE(KStackIsEmpty(testStack));
    KStackPush(testStack,&b);
    KStackPush(testStack,&c);
    ASSERT_FALSE(KStackIsEmpty(testStack));
    int *pVal = (int *)KStackTop(testStack);
    KStackPop(testStack);
    ASSERT_FALSE(KStackIsEmpty(testStack));
    ASSERT_EQ(*pVal,5);
    KStackPush(testStack,&d);
    pVal = (int *)KStackTop(testStack);
    KStackPop(testStack);
    ASSERT_FALSE(KStackIsEmpty(testStack));
    ASSERT_EQ(*pVal,10);

    pVal = (int *)KStackTop(testStack);
    KStackPop(testStack);
    ASSERT_EQ(*pVal,2);

    pVal = (int *)KStackTop(testStack);
    KStackPop(testStack);
    ASSERT_EQ(*pVal,1);
    ASSERT_TRUE(KStackIsEmpty(testStack));
}


TEST(KOList,Common) {
    KOListNode head;
    KOListInit(&head);
    int testval1 = 1;
    int testval2 = 2;
    int testval3 = 3;
    int testval4 = 4;
    int testval5 = 5;

    KOListAddHead(&head,&testval1);
    KOListAddHead(&head,&testval2);
    KOListAddHead(&head,&testval3);
    KOListAddHead(&head,&testval4);
    KOListAddHead(&head,&testval5);

    KListNode *current;
    KListForEach(current,&head.node) {
        KOListNode *currentOList = KListEntry(current,KOListNode,node);
        int val = *(int *)KOListData(currentOList);
        cout<<val<<endl;
    }

    KOListNode *pNode = &head;
    pNode = KOListNext(pNode);
    ASSERT_EQ(*(int *)KOListData(pNode),5);
    pNode = KOListNext(pNode);
    ASSERT_EQ(*(int *)KOListData(pNode),4);
    pNode = KOListNext(pNode);
    ASSERT_EQ(*(int *)KOListData(pNode),3);
    pNode = KOListNext(pNode);
    ASSERT_EQ(*(int *)KOListData(pNode),2);
    pNode = KOListNext(pNode);
    ASSERT_EQ(*(int *)KOListData(pNode),1);

    KOListClean(&head);

    KOListAddTail(&head,&testval1);
    KOListAddTail(&head,&testval2);
    KOListAddTail(&head,&testval3);
    KOListAddTail(&head,&testval4);
    KOListAddTail(&head,&testval5);

    KListForEach(current,&head.node) {
        KOListNode *currentOList = KListEntry(current,KOListNode,node);
        int val = *(int *)KOListData(currentOList);
        cout<<val<<endl;
    }

    pNode = &head;
    pNode = KOListNext(pNode);
    ASSERT_EQ(*(int *)KOListData(pNode),1);
    pNode = KOListNext(pNode);
    ASSERT_EQ(*(int *)KOListData(pNode),2);
    pNode = KOListNext(pNode);
    ASSERT_EQ(*(int *)KOListData(pNode),3);
    pNode = KOListNext(pNode);
    ASSERT_EQ(*(int *)KOListData(pNode),4);
    pNode = KOListNext(pNode);
    ASSERT_EQ(*(int *)KOListData(pNode),5);
    KOListClean(&head);

}