/**
 * @file   KList.h
 * @brief  list data struct implementing.
 * list data struct implementing.
 * @author ChenPeng
 * @date   2016-10-12
 */
#ifndef __KLISTH__
#define __KLISTH__

#include "Tools.h"

typedef struct __KListNode {
    struct __KListNode *prev,*next;
} KListNode;

static inline void __KListAddNode(KListNode *newNode,
                             KListNode *prev,
                             KListNode *next)
{
    next->prev = newNode;
    newNode->next = next;
    newNode->prev = prev;
    prev->next = newNode;
}

#define KLIST_HEAD_INIT(name) { &(name), &(name) }

#define KLIST_HEAD(name) \
    KListNode name = KLIST_HEAD_INIT(name)

static inline void KListInitHead(KListNode *list) {
    assert(list);
    list->next = list;
    list->prev = list;
}

static inline void KListAddHead(KListNode *head,KListNode *newNode)
{
    __KListAddNode(newNode, head, head->next);
}

static inline void KListAddTail(KListNode *head,KListNode *newNode)
{
    __KListAddNode(newNode, head->prev, head);
}


static inline void __KListDel(KListNode * prev, KListNode * next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void __KListDelEntry(KListNode *entry)
{
    __KListDel(entry->prev, entry->next);
}

static inline void KListDel(KListNode *entry)
{
    __KListDel(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
}


static inline void KListMove(KListNode *list, KListNode *head)
{
    __KListDelEntry(list);
    KListAddHead(head,list);
}

static inline void KListMoveTail(KListNode *list,
                                  KListNode *head)
{
    __KListDelEntry(list);
    KListAddTail(head,list);
}

static inline int KListIsLast(const KListNode *list,
                               const KListNode *head)
{
    return list->next == head;
}

static inline int KListEmpty(const KListNode *head)
{
    return head->next == head;
}


#define KListForEach(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define KListEntry(ptr,ntype,member) \
    vcontainer_of(ptr,ntype,member)

///////////////////////////////////////////////KHLIST///////////////////////////////////////////

typedef struct __KHListNode {
    struct __KHListNode *next,**pprev;
} KHListNode;

typedef struct {
    KHListNode *first;
} KHListHead;

#define KHLIST_HEAD_INIT() { .first = NULL }

#define KHLIST_HEAD(name) \
    KListNode name = KHLIST_HEAD_INIT()

static inline void INIT_HLIST_NODE(KHListNode *h)
{
	h->next = NULL;
	h->pprev = NULL;
}

static inline int KHListIsUnhashed(const KHListNode *h)
{
	return !h->pprev;
}

static inline int KHListEmpty(const KHListHead *h)
{
	return !h->first;
}

static inline void __KHListDel(KHListNode *n)
{
    if (n == NULL) {
        return ;
    }

	KHListNode *next = n->next;
	KHListNode **pprev = n->pprev;
	*pprev = next;
	if (next)
		next->pprev = pprev;
}

static inline void KHListDel(KHListNode *n)
{
	__KHListDel(n);
	n->next = NULL;
	n->pprev = NULL;
}

static inline void KHLlistDelInit(KHListNode *n)
{
	if (!KHListIsUnhashed(n)) {
		__KHListDel(n);
		INIT_HLIST_NODE(n);
	}
}

static inline void KHListAddHead(KHListHead *h, KHListNode *n)
{
	KHListNode *first = h->first;
	n->next = first;
	if (first)
		first->pprev = &n->next;
	h->first = n;
	n->pprev = &h->first;
}

/* next must be != NULL */
static inline void KHLlistAddBefore(KHListNode *next,KHListNode *n)
{
    assert(n);
    assert(next);
	n->pprev = next->pprev;
	n->next = next;
	next->pprev = &n->next;
	*(n->pprev) = n;
}

static inline void KHListAddAfter(KHListNode *n, KHListNode *next)
{
    assert(n);
    assert(next);
	next->next = n->next;
	n->next = next;
	next->pprev = &n->next;

	if(next->next)
		next->next->pprev  = &next->next;
}

static inline void KHListAddFake(KHListNode *n)
{
	n->pprev = &n->next;
}

static inline void KHListMoveList(KHListHead *n, KHListHead *old)
{
	n->first = old->first;
	if (n->first)
		n->first->pprev = &n->first;
	old->first = NULL;
}

#define KHListEntry(ptr, type, member) \
    vcontainer_of(ptr,type,member)

#define KHListForEach(pos, head) \
	for (pos = (head)->first; pos ; pos = pos->next)

typedef struct {
    KListNode node;
    void *data;
} KOListNode;

static inline void KOListInit(KOListNode *head) {
    KListInitHead(&head->node);
}

static inline void KOListAddHead(KOListNode *head, void *data) {
    KOListNode *newNode = ObjAlloc(KOListNode);
    newNode->data = data;
    KListAddHead(&head->node,&newNode->node);
}

static inline void KOListAddTail(KOListNode *head, void *data) {
    KOListNode *newNode = ObjAlloc(KOListNode);
    newNode->data = data;
    KListAddTail(&head->node,&newNode->node);
}

static inline void KOListDel(KOListNode *node) {
    __KListDelEntry(&node->node);
    ObjRelease(node);
}

static inline int KOListEmpty(const KOListNode *head) {
    return KListEmpty(&head->node);
}

static inline KOListNode *KOListNext(const KOListNode *node) {
    if (node->node.next == NULL) {
        return NULL;
    }

    return (KOListNode *)KListEntry(node->node.next,KOListNode,node);
}

static inline KOListNode *KOListPrev(const KOListNode *node) {
    if (node->node.prev == NULL) {
        return NULL;
    }

    KListEntry(node->node.prev,KOListNode,node);
}

static inline void *KOListData(const KOListNode *node) {
    return node->data;
}

static inline void KOListClean(KOListNode *node) {
    while(!KOListEmpty(node)) KOListDel(KListEntry(node->node.next,KOListNode,node));
}

typedef KOListNode KStackNode;
typedef KListNode KStack;

static inline void KStackInit(KStack *stack) {
    KListInitHead(stack);
}

static inline void KStackPush(KStack *stack,void *data) {
    KStackNode *stackNode = ObjAlloc(KStackNode);
    stackNode->data = data;
    KListAddTail(stack,&stackNode->node);
}

static inline bool KStackIsEmpty(KStack *stack) {
    return KListEmpty(stack);
}

static inline KListNode* __KStackTopListNode(KStack *stack) {
    if (KStackIsEmpty(stack)) {
        return NULL;
    }
    return stack->prev;
}

static inline void* KStackTop(KStack *stack) {
    KListNode *pListNode = __KStackTopListNode(stack);
    if (pListNode == NULL) {
        return NULL;
    }

    KStackNode *pNode = KListEntry(pListNode,KStackNode,node);
    return pNode->data;
}

static inline void KStackPop(KStack *stack) {
    KListNode *topListNode = __KStackTopListNode(stack);
    KListDel(topListNode);
    KStackNode *sNode = KListEntry(topListNode,KStackNode,node);
    ObjRelease(sNode);
}

static inline void KStackClean(KStack *stack) {
    while(!KStackIsEmpty(stack)) KStackPop(stack);
}

#endif