
#include <sys/time.h>
#include <gtest/gtest.h>
#include <iostream>

#include "GTestingExt.h"
#include "KList.h"
#include "KRbTree.h"

using namespace std;

typedef struct {
    uint64_t span;
    double TestVal1;
    uint64_t span2;
    KRbNode node;
    uint32_t Key;
} KRbNodeTest;

int TestKRbCompareKeyFun(const KRbNode *pNode,const void *key) {
    KRbNodeTest *TestNode = KRbEntry(pNode,KRbNodeTest,node);
    return TestNode->Key- *(int *)key;
}

int TestKRbCompareFun(const KRbNode *a,const KRbNode *b) {
    KRbNodeTest *NodeB = KRbEntry(b,KRbNodeTest,node);
    return TestKRbCompareKeyFun(a,&NodeB->Key);
}

int TestKRbTreeIterativeFun(KRbNode *node,void *param) {
    KRbNodeTest *pNode = KRbEntry(node,KRbNodeTest,node);
    ObjRelease(pNode);
    return VE_OK;
}

int DisplayKRbTreeRecursion(KRbNode *node,uint32_t indent) {
    if (node->right != RBTreeNil) {
        DisplayKRbTreeRecursion(node->right,indent + 3);
    }
    KRbNodeTest *pNode = KRbEntry(node,KRbNodeTest,node);
    string space(indent,' ');
    cout<<space;
    if (KRbIsRed(&pNode->node)) {
        cout<<"R";
    } else {
        cout<<"B";
    }
    cout<<pNode->Key<<" : "<<pNode->TestVal1<<endl;

    if (node->left != RBTreeNil) {
        DisplayKRbTreeRecursion(node->left,indent + 3);
    }

    return VE_OK;
}

int DisplayKRbTree(KRbNode *node) {
    //cout<<"Recursion version ================================="<<endl;
    //DisplayKRbTreeRecursion(node,0);

    // cout<<"NonRecursion version ================================="<<endl;
    // int indent(0);
    // VStack stack;
    // VStackInit(&stack);
    // while (node != RBTreeNil || !VStackIsEmpty(&stack)) {
    //     if (node != RBTreeNil) {
    //         VStackPush(&stack,node);
    //         KRbNode *pParent = KRbParent(node);
    //         if (node == pParent->left) {
    //             indent += 6;
    //         } else {
    //             indent+=3;
    //         }

    //         node = node->right;
    //     } else {
    //         node = (KRbNode *)VStackTop(&stack);
    //         KRbNodeTest *pNode = KRbEntry(node,KRbNodeTest,node);
    //         string space(indent,' ');
    //         cout<<space;
    //         if (KRbIsRed(&pNode->node)) {
    //             cout<<"R";
    //         } else {
    //             cout<<"B";
    //         }
    //         cout<<pNode->Key<<endl;
    //         VStackPop(&stack);
    //         indent -= 3;
    //         node = node->left;
    //     }
    // }
    //cout<<endl<<endl;

    return 0;
}

class KRbTreeTest: public testing::Test {
protected:
    virtual void SetUp() {
        KRbInitTree(&root);
        root.cmp = TestKRbCompareFun;
        root.cmpKey = TestKRbCompareKeyFun;
        root.hit = NULL;
        root.inserted = NULL;
    }

    virtual void TearDown() {
        KRbClearTree(&root,TestKRbTreeIterativeFun,NULL);
    }

    KRbRoot root;
};


TEST_F(KRbTreeTest,KRbNodeBasicInsertTest) {
    KRbNodeTest *test = ObjAlloc(KRbNodeTest);
    test->Key= 199;
    test->TestVal1 = 100.1;
    KRbInsert(&root,&test->node);
    KRbNodeTest *pNode = KRbEntry(root.node,KRbNodeTest,node);
    EXPECT_TRUE(pNode);
    EXPECT_EQ(pNode->Key,199);
    EXPECT_EQ(pNode->TestVal1,100.1);
}

int traverseFun(KRbNode *node,void *param) {
    KRbNodeTest *pTest = KRbEntry(node,KRbNodeTest,node);
    cout<<"key : "<<pTest->Key<<" val : "<<pTest->TestVal1<<endl;

    return VE_OK;
}

TEST_F(KRbTreeTest,KRbNodeSameKeyTest) {

    KRbNodeTest *test = ObjAlloc(KRbNodeTest);
    test->Key = 199;
    test->TestVal1 = 1.1;
    KRbInsert(&root,&test->node);

    KRbNodeTest *test2 = ObjAlloc(KRbNodeTest);
    test2->Key = 199;
    test2->TestVal1 = 2.1;
    KRbInsert(&root,&test2->node);

    KRbNodeTest *test3 = ObjAlloc(KRbNodeTest);
    test3->Key = 199;
    test3->TestVal1 = 1.2;
    KRbInsert(&root,&test3->node);

    KRbNodeTest *test4 = ObjAlloc(KRbNodeTest);
    test4->Key = 199;
    test4->TestVal1 = 1.5;
    KRbInsert(&root,&test4->node);

    KRbPreOrderTraverseTree(&root,traverseFun,NULL);
}

TEST_F(KRbTreeTest,KRbNodeSeqInsertTest) {
    KRbNodeTest *test = ObjAlloc(KRbNodeTest);
    test->Key = 199;
    test->TestVal1 = 1.1;
    KRbInsert(&root,&test->node);
    KRbNodeTest *test2 = ObjAlloc(KRbNodeTest);
    test2->Key = 200;
    test2->TestVal1 = 2.1;
    KRbInsert(&root,&test2->node);

    int searchKey(199);
    KRbNode *findNode = KRbSearch(&root,&searchKey);
    EXPECT_TRUE(findNode != NULL);
    KRbNodeTest *node1 = KRbEntry(findNode,KRbNodeTest,node);
    EXPECT_TRUE(node1 != NULL);
    EXPECT_EQ(node1->Key,199);
    EXPECT_DOUBLE_EQ(node1->TestVal1,1.1);

    KRbNode *successor = KRbSuccessor(findNode);
    EXPECT_TRUE(successor != NULL);
    KRbNodeTest *node2 = KRbEntry(successor,KRbNodeTest,node);
    EXPECT_TRUE(node2 != NULL);
    EXPECT_EQ(node2->Key,200);
    EXPECT_DOUBLE_EQ(node2->TestVal1,2.1);

//    DisplayKRbTree(root.node);
    KRbNode *predcessor = KRbPredecessor(successor);
    EXPECT_EQ(predcessor,findNode);
}

void KRbTreeTestInsert(KRbRoot *root,int key,double val) {
    KRbNodeTest *test = ObjAlloc(KRbNodeTest);
    test->Key = key;
    test->TestVal1 = val;
    KRbInsert(root,&test->node);
}

TEST_F(KRbTreeTest,KRbNodeSeqLargerInsertTest) {
    KRbTreeTestInsert(&root,3,0);
    DisplayKRbTree(root.node);
    KRbTreeTestInsert(&root,2,0);
    DisplayKRbTree(root.node);
    KRbTreeTestInsert(&root,1,0);
    DisplayKRbTree(root.node);
    KRbTreeTestInsert(&root,5,0);
    KRbTreeTestInsert(&root,4,0);
    KRbTreeTestInsert(&root,6,0);
    DisplayKRbTree(root.node);
    KRbTreeTestInsert(&root,9,0);
    KRbTreeTestInsert(&root,7,0);
    DisplayKRbTree(root.node);
    KRbTreeTestInsert(&root,3,1);
    KRbTreeTestInsert(&root,8,0);
    KRbTreeTestInsert(&root,10,0);

    DisplayKRbTree(root.node);
}

KRbNode *KRbSearchID(KRbRoot *root,int id) {
    return KRbSearch(root,&id);
}

TEST_F(KRbTreeTest,KRbNodeInsertRemoveSearchTest) {
    KRbTreeTestInsert(&root,3,0);
    DisplayKRbTree(root.node);
    KRbTreeTestInsert(&root,2,0);
    DisplayKRbTree(root.node);
    KRbTreeTestInsert(&root,1,0);
    DisplayKRbTree(root.node);
    KRbTreeTestInsert(&root,5,6.6);
    KRbTreeTestInsert(&root,4,0);
    KRbTreeTestInsert(&root,6,0);
    DisplayKRbTree(root.node);
    KRbTreeTestInsert(&root,9,0);
    KRbTreeTestInsert(&root,7,0);
    DisplayKRbTree(root.node);
    KRbTreeTestInsert(&root,3,1);
    KRbTreeTestInsert(&root,8,0);
    KRbTreeTestInsert(&root,10,0);

    KRbNode *searchNode = KRbSearchID(&root,5);
    KRbNodeTest *searchNodeTest = KRbEntry(searchNode,KRbNodeTest,node);
    ASSERT_EQ(searchNodeTest->Key,5);
    ASSERT_DOUBLE_EQ(searchNodeTest->TestVal1,6.6);
    KRbDelete(&root,searchNode);

    DisplayKRbTree(root.node);
}

#define RBTREE_RANDOM_TEST_TIMES (1<<24)
//#define RBTREE_RANDOM_TEST_TIMES (2<<4)
#define RBTREE_RANDOM_TEST_TIMES_MASK (RBTREE_RANDOM_TEST_TIMES - 1)


void PrintTimeReset(timeval *begtime) {
    gettimeofday(begtime,NULL);
}
void PrintTimeDiff(const char *tag,timeval *begtime) {
    timeval endtime;
    gettimeofday(&endtime,NULL);
    uint64_t timeDiff = endtime.tv_sec - begtime->tv_sec;
    timeDiff *=(1000*1000);
    timeDiff += (endtime.tv_usec - begtime->tv_usec);
    cout<<tag<<" time : "<<timeDiff<<endl;
    *begtime = endtime;
}

DIS_TEST_F(KRbTreeTest,KRbNodeMassRandomInsertTest) {
    uint32_t *randList = new uint32_t[RBTREE_RANDOM_TEST_TIMES];
    srand((int)time(NULL));
    for (uint32_t i = 0; i != RBTREE_RANDOM_TEST_TIMES; i++) {
        uint32_t randVal = rand();
        srand(randVal);
        randList[i] = randVal;

        KRbNodeTest *test = ObjAlloc(KRbNodeTest);
        test->Key = randVal;
        test->TestVal1 = randVal/1000.0;
        KRbInsert(&root,&test->node);
    }

    DisplayKRbTree(root.node);

    timeval begtime;
    PrintTimeReset(&begtime);
    KRbNode *currentNode = KRbMinimum(root.node);
    for (uint32_t i = 0; i != RBTREE_RANDOM_TEST_TIMES - 1U; i++) {
        KRbNode *successor = KRbSuccessor(currentNode);
        EXPECT_TRUE(successor != NULL);
        KRbNodeTest *successorNodeTest = KRbEntry(successor,KRbNodeTest,node);
        KRbNodeTest *currentNodeTest = KRbEntry(currentNode,KRbNodeTest,node);
        EXPECT_GE(successorNodeTest->Key,currentNodeTest->Key);
        currentNode = successor;
    }

    PrintTimeDiff("RBTREE_RANDOM_TEST_TIMES KRbSuccessor",&begtime);

    KRbNode *maxNode = KRbMaximum(root.node);
    ASSERT_EQ(currentNode,maxNode);
    for (uint32_t i = 0; i != RBTREE_RANDOM_TEST_TIMES - 1U; i++) {
        KRbNode *predecessor = KRbPredecessor(currentNode);
        EXPECT_TRUE(predecessor != NULL);
        KRbNodeTest *predecessorNodeTest = KRbEntry(predecessor,KRbNodeTest,node);
        KRbNodeTest *currentNodeTest = KRbEntry(currentNode,KRbNodeTest,node);
        EXPECT_LE(predecessorNodeTest->Key,currentNodeTest->Key);
        currentNode = predecessor;
    }

    PrintTimeDiff("RBTREE_RANDOM_TEST_TIMES KRbPredecessor",&begtime);

    for (uint32_t i = 0; i != RBTREE_RANDOM_TEST_TIMES; i++) {
        uint32_t testKey = randList[i];
        KRbNode *findNode = KRbSearch(&root,&testKey);
        EXPECT_TRUE(findNode != NULL);
        KRbNodeTest *findKRbnode = KRbEntry(findNode,KRbNodeTest,node);
        EXPECT_TRUE(findKRbnode != NULL);
        EXPECT_EQ(findKRbnode->Key,testKey);
        EXPECT_DOUBLE_EQ(findKRbnode->TestVal1,testKey/1000.0)<<" index : "<<i<<" randval : "<<randList[i]<<" key : "<<findKRbnode->Key;
        if(testKey%17 == 0) {
            KRbDelete(&root,findNode);
            ObjRelease(findKRbnode);
        }
        else if(testKey%13 == 0) {
            KRbDelete(&root,findNode);
            ObjRelease(findKRbnode);
        }
    }

    PrintTimeDiff("RBTREE_RANDOM_TEST_TIMES KRbSearch",&begtime);
    cout<<"RBTREE_RANDOM_TEST_TIMES = "<<RBTREE_RANDOM_TEST_TIMES<<endl;
    delete[] randList;
}


DIS_TEST_F(KRbTreeTest,KRbNodeMassRandomInsertTest2) {
    uint32_t *randList = new uint32_t[RBTREE_RANDOM_TEST_TIMES];
    srand((int)time(NULL));
    for (uint32_t i = 0; i != RBTREE_RANDOM_TEST_TIMES; i++) {
        uint32_t randVal = rand();
        srand(randVal);
        randList[i] = randVal;
        KRbNodeTest *test = ObjAlloc(KRbNodeTest);
        test->Key = randVal;
        test->TestVal1 = randVal/1000.0;
        KRbInsert(&root,&test->node);
    }

    DisplayKRbTree(root.node);

    for (uint32_t i = 0; i != RBTREE_RANDOM_TEST_TIMES; i++) {
        uint32_t testKey = randList[i];
        KRbNode *findNode = KRbSearch(&root,&testKey);
        EXPECT_TRUE(findNode != NULL);
        KRbNodeTest *findKRbnode = KRbEntry(findNode,KRbNodeTest,node);
        EXPECT_TRUE(findKRbnode != NULL);
        EXPECT_EQ(findKRbnode->Key,testKey);
        EXPECT_DOUBLE_EQ(findKRbnode->TestVal1,testKey/1000.0);

        if(testKey%17 == 0) {
            KRbDelete(&root,findNode);
            ObjRelease(findKRbnode);
        }
        else if(testKey%13 == 0) {
            KRbDelete(&root,findNode);
            ObjRelease(findKRbnode);
        }

    }

    DisplayKRbTree(root.node);

    KRbNode *maxNode = KRbMaximum(root.node);
    KRbNode *minNode = KRbMinimum(root.node);

    KRbNode *currentNode = minNode;
    while(currentNode != maxNode) {
        KRbNode *successor = KRbSuccessor(currentNode);
        EXPECT_TRUE(successor != NULL);
        KRbNodeTest *successorNodeTest = KRbEntry(successor,KRbNodeTest,node);
        KRbNodeTest *currentNodeTest = KRbEntry(currentNode,KRbNodeTest,node);
        EXPECT_GE(successorNodeTest->Key,currentNodeTest->Key);
        currentNode = successor;
    }

    ASSERT_EQ(currentNode,maxNode);

    while(currentNode != minNode) {
        KRbNode *predecessor = KRbPredecessor(currentNode);
        EXPECT_TRUE(predecessor != NULL);
        KRbNodeTest *predecessorNodeTest = KRbEntry(predecessor,KRbNodeTest,node);
        KRbNodeTest *currentNodeTest = KRbEntry(currentNode,KRbNodeTest,node);
        EXPECT_LE(predecessorNodeTest->Key,currentNodeTest->Key);
        currentNode = predecessor;
    }

    delete[] randList;
}