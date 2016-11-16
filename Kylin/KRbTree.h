
#ifndef __VRBTree__
#define __VRBTree__

#include "KTools.h"

#define RBNODE_COLOR_RED 0
#define RBNODE_COLOR_BLACK 1
#define RBNODE_FLAG_MASK 3

typedef struct KRbNode{
    intptr_t __parent_color; //do not use it directly
    struct KRbNode *left;
    struct KRbNode *right;
} KRbNode;

typedef int (*KRbCompareFun)(const KRbNode *a,const KRbNode *b);

typedef int (*KRbCompareKeyFun)(const KRbNode *Node,const void *key);

typedef void (*KRbSearchHit)(const KRbNode *hitNode);

typedef void (*KRbInserted)(KRbNode *insertedNode);

typedef struct {
    KRbNode *node;
    KRbCompareFun cmp;
    KRbCompareKeyFun cmpKey;
    KRbSearchHit hit;
    KRbInserted inserted;
} KRbTree;

extern KRbNode *RBTreeNil;

#define KRbEntry(ptr,type,member) vcontainer_of(ptr,type,member)

#define KRbColor(rb) ((rb)->__parent_color & 1)

#define KRbIsBlack(rb) KRbColor(rb)

#define KRbIsRed(rb) (!KRbIsBlack(rb))

#define KRbParent(n) ((KRbNode *)((n)->__parent_color & ~RBNODE_FLAG_MASK))

#define KRbIsNil(n) ((n) == RBTreeNil)

#define KRbNotNil(n) (!KRbIsNil(n))

static inline KRbNode *KRbRedParent(KRbNode *rb) {
    return (KRbNode *)rb->__parent_color;
}

static inline intptr_t KRbMakeParentColor(KRbNode *parent,int color) {
    return ((intptr_t)parent | color);
}

static inline void KRbSetParent(KRbNode *rb,KRbNode *parent) {
    rb->__parent_color = KRbColor(rb) | (intptr_t)parent;
}

static inline void KRbSetParentColor(KRbNode *rb,KRbNode *parent,int color) {
    rb->__parent_color = KRbMakeParentColor(parent,color);
}

static inline void KRbSetBlack(KRbNode *rb) {
    rb->__parent_color |= RBNODE_COLOR_BLACK;
}

static inline void KRbSetRed(KRbNode *rb) {
    rb->__parent_color &= ~RBNODE_COLOR_BLACK;
}

static inline void KRbSetColor(KRbNode *rb,int color) {
    rb->__parent_color &= ~RBNODE_COLOR_BLACK;
    rb->__parent_color |= color;
}

static inline void KRbInitTree(KRbTree *root) {
    root->node = RBTreeNil;
}

typedef int (*KRbTreeIterativeFun)(KRbNode *node,void *param);

#ifdef __cplusplus
extern "C" {
#endif

KRbNode *KRbMinimum(KRbNode *node);
KRbNode *KRbMaximum(KRbNode *node);
KRbNode *KRbSuccessor(KRbNode *node);
KRbNode *KRbPredecessor(KRbNode *node);
KRbNode *KRbSearch(KRbTree *root,const void *key);
int KRbInsertSearched(KRbTree *root,KRbNode *node,KRbNode *parent);
int KRbInsert(KRbTree *root,KRbNode *node);
void KRbDelete(KRbTree *root,KRbNode *node);
int KRbClearTree(KRbTree *root,KRbTreeIterativeFun fun,void *param);
int KRbPreOrderTraverse(KRbNode *node,KRbTreeIterativeFun fun,void *param);

#ifdef __cplusplus
} //extern "C"
#endif

static inline int KRbPreOrderTraverseTree(KRbTree *root,KRbTreeIterativeFun fun,void *param) {
    CheckNULLPtr(root);
    CheckNULLPtr(root->node);
    return KRbPreOrderTraverse(root->node,fun,param);
}

#endif