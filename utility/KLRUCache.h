#ifndef __KLRU_CACHE_H__
#define __KLRU_CACHE_H__

#define KLRUCache(type,name) GetTmpName(LRU_##type,name)

typedef struct {
    KRbRoot rbTree;
    KListHead listHead;
    uint64_t len;
} KLRUCache_t;

typedef struct {
    KRbNode rbNode;
    KListNode listNode;
    void *obj;
} KLRUNode_t;

#define InstLRUTemplate(name,                                       \
                        objType,                                    \
                        keyName,                                    \
                        comparePred)                                \
    typedef typeof(((objType *)0)->keyName)                         \
    KLRUCache(name,keyType);                                        \
    int KLRUCache(name,Init)(KLRUCache_t *cache,uint64_t len) {     \
    }                                                               \
    int KLRUCache(name,AddCache)(KLRUCache_t *cache,objType *obj) { \
    }                                                               \
    objType *KLRUCache(name,GetCache)                               \
    (KLRUCache_t *cache,KLRUCache(name,keyType) *key) {             \
        KRbNode *node = KRbSearch(&cache->rbTree,key);              \
    }                                                               \

#endif /* __KLRU_CACHE_H__ */