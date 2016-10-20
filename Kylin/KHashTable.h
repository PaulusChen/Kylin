/**
 * @file   KHashTable.h
 * @brief  KHashTable is a High-Performance implementing for hashtable.
 * KHashTable is a High-Performance implementing for hashtable.
 * The idea comes from the implementing of linux kernel hashtable and gdsl.
 * This implementing supports genereic programming that means you can pass
 * the type of the data you want to save in KHashTable, and then they
 * will generate a couple of functions that would be suitable for the type
 * you have passed into KHashTable.
 * @author ChenPeng
 * @date   2016-10-12
 */

#ifndef __KHASHTABLE_H__
#define __KHASHTABLE_H__

#include "KList.h"
#include "KylinTypes.h"

#define KHASH_TABLE_FILL_FACTOR_WARNING_LINE (32)
#define KHASH_TABLE_ALERT_TABLE_LEN GetMB(10) //about 1000 00 00 // need 1 * sizeof(KHashSlotHead) mbyte = 160MB

#define KHASH_TABLE_FLAG_DEFAULT 0
#define KHASH_TABLE_FLAG_NOT_GROWABLE make_flag(0)

typedef struct {
    KHListHead listHead;
    uint64_t len;
} KHashSlotHead;

typedef struct {
    uint32_t fillFactor;
    uint32_t maxSlotLen;
    uint32_t tableLen;
    uint32_t flag;
    uint64_t totalCount;
    KHashSlotHead *table;
} KHTable;

static inline uint64_t KHashTableDebugCount(KHTable *table) {
    uint64_t total = 0;
    uint32_t slotIndex = 0;
    for (slotIndex = 0;
         slotIndex != table->tableLen;
         slotIndex++) {
        KHashSlotHead *slot = &table->table[slotIndex];
        total += slot->len;
    }
    return total;
}

static inline uint32_t KHashTableGetFillFactor(KHTable *table) {
    assert(table);
    return table->fillFactor;
}

static inline uint32_t KHashTableGetMaxSlotLen(KHTable *table) {
    assert(table);
    return table->maxSlotLen;
}

static inline uint32_t KHashTableGetSlotCount(KHTable *table) {
    assert(table);
    return table->tableLen;
}

static inline uint32_t KHashTableGetTotalCount(KHTable *table) {
    assert(table);
    return table->totalCount;
}

#define KHASH_TABLE_DEFAULT_FILL_FACTOR (4)

#define KHashTable(type,name) GetTmpName(HT_##type,name)

#define InstHashTemplate(name,                                          \
                         objType,                                       \
                         keyName,                                       \
                         nodeName,                                      \
                         comparePred)                                   \
    typedef typeof(((objType *)0)->keyName)                             \
    KHashTable(name,keyType);                                           \
    typedef int (* KHashTable(objType,TraverseFun))(objType *obj,void *param); \
    static inline uint64_t KHashTable(name,HashFun)                     \
    (const KHashTable(name,keyType) *);                                 \
    static inline int KHashTable(name,_vtableGrow)(KHTable *table) {    \
        uint32_t newTableLen = table->tableLen * 2;                     \
        table->fillFactor *= 2;                                         \
        KHashSlotHead *newListTable =                                   \
            (KHashSlotHead *)calloc(newTableLen,                        \
                                    sizeof(KHashSlotHead));             \
        if (newListTable == NULL) {                                     \
            return KE_OK;                                               \
        }                                                               \
        if (table->tableLen * 2 > KHASH_TABLE_ALERT_TABLE_LEN) {        \
            return KE_UNKNOW;                                           \
        }                                                               \
        uint32_t slotIndex = 0;                                         \
        table->maxSlotLen = 0;                                          \
        for (slotIndex = 0;                                             \
             slotIndex != table->tableLen;                              \
             slotIndex++) {                                             \
            KHashSlotHead *slot = &table->table[slotIndex];             \
            KHListNode *current = NULL;                                 \
            for (current = slot->listHead.first; current ;) {           \
                KHListNode *pNext = current->next;                      \
                KHListDel(current);                                     \
                objType *curObj = KHListEntry(current,objType,nodeName); \
                const KHashTable(name,keyType) *currentKey =            \
                    &curObj->keyName;                                   \
                uint64_t hashVal = KHashTable(name,HashFun)(currentKey); \
                KHashSlotHead *newSlot =                                \
                    &newListTable[hashVal%newTableLen];                 \
                KHListAddHead(&newSlot->listHead,current);              \
                newSlot->len++;                                         \
                table->maxSlotLen =                                     \
                    (newSlot->len > table->maxSlotLen ?                 \
                     newSlot->len : table->maxSlotLen);                 \
                current = pNext;                                        \
            }                                                           \
        }                                                               \
        free(table->table);                                             \
        table->table = newListTable;                                    \
        table->tableLen = newTableLen;                                  \
        return KE_OK;                                                   \
    }                                                                   \
    static inline void KHashTable(name,Init)(KHTable *table,            \
                                             uint64_t initSize,         \
                                             uint32_t flag) {           \
        table->fillFactor = KHASH_TABLE_DEFAULT_FILL_FACTOR;            \
        table->maxSlotLen = 0;                                          \
        table->tableLen = initSize/table->fillFactor;                   \
        table->flag = flag;                                             \
        table->totalCount = 0;                                          \
        table->table = (KHashSlotHead *)calloc(table->tableLen,         \
                                               sizeof(KHashSlotHead));  \
    }                                                                   \
    static inline int KHashTable(name,Insert)(KHTable *table,           \
                                              objType *obj) {           \
        assert(table);                                                  \
        assert(obj);                                                    \
        if (table->maxSlotLen > table->fillFactor) {                    \
            if (CheckFlag(table->flag,KHASH_TABLE_FLAG_NOT_GROWABLE)) { \
                return KE_CONTAINER_FULL;                               \
            } else {                                                    \
                KHashTable(name,_vtableGrow)(table);                    \
            }                                                           \
        }                                                               \
        uint64_t hashVal = KHashTable(name,HashFun)(&obj->keyName);     \
        uint32_t slotIndex = hashVal % table->tableLen;                 \
        KHashSlotHead *slot = &table->table[slotIndex];                 \
        KHListAddHead(&slot->listHead,&obj->nodeName);                  \
        slot->len++;                                                    \
        table->totalCount++;                                            \
        table->maxSlotLen = (slot->len > table->maxSlotLen ?            \
                             slot->len : table->maxSlotLen);            \
        return KE_OK;                                                   \
    }                                                                   \
    static inline void KHashTable(name,Delete)(KHTable *table,          \
                                               objType *obj) {          \
        assert(table);                                                  \
        assert(obj);                                                    \
        uint64_t hashVal = KHashTable(name,HashFun)(&obj->keyName);     \
        uint32_t slotIndex = hashVal % table->tableLen;                 \
        KHashSlotHead *slot = &table->table[slotIndex];                 \
        slot->len--;                                                    \
        table->totalCount--;                                            \
        KHListDel(&obj->nodeName);                                      \
    }                                                                   \
    static inline objType *KHashTable(name,Search)                      \
    (KHTable *table,                                                    \
     const KHashTable(name,keyType) *key) {                             \
        assert(table);                                                  \
        assert(key);                                                    \
        uint64_t hashVal = KHashTable(name,HashFun)(key);               \
        uint32_t slotIndex = hashVal % table->tableLen;                 \
        KHashSlotHead *slot = &table->table[slotIndex];                 \
        KHListNode *current = NULL;                                     \
        KHListForEach(current,&slot->listHead) {                        \
            objType *curObj = KHListEntry(current,objType,nodeName);    \
            const KHashTable(name,keyType) *currentKey =                \
                &curObj->keyName;                                       \
            if(comparePred(currentKey,key) == 0) {                      \
                return curObj;                                          \
            }                                                           \
        }                                                               \
        return NULL;                                                    \
    }                                                                   \
    static inline objType *KHashTable(name,DeleteByKey)                 \
    (KHTable *table,                                                    \
     const KHashTable(name,keyType) *key) {                             \
        assert(table);                                                  \
        assert(key);                                                    \
        objType *obj = KHashTable(name,Search)(table,key);              \
        if (obj == NULL) return NULL;                                   \
        KHashTable(name,Delete)(table,obj);                             \
        return obj;                                                     \
    }                                                                   \
    static inline int KHashTable(name,Traverse)                         \
    (KHTable *table,                                                    \
     KHashTable(objType,TraverseFun) traverseFun,                       \
     void *param) {                                                     \
        assert(table);                                                  \
        if (traverseFun == NULL) return KE_OK;                          \
        uint32_t slotIndex = 0;                                         \
        for (slotIndex = 0;                                             \
             slotIndex != table->tableLen;                              \
             slotIndex++) {                                             \
            KHashSlotHead *slot = &table->table[slotIndex];             \
            KHListNode *current = NULL;                                 \
            KHListForEach(current,&slot->listHead) {                    \
                objType *obj = KHListEntry(current,objType,nodeName);   \
                int reval = traverseFun(obj,param);                     \
                if (reval != KE_OK) {                                   \
                    return reval;                                       \
                }                                                       \
            }                                                           \
        }                                                               \
        return KE_OK;                                                   \
    }                                                                   \
    static inline int KHashTable(name,Clear)                            \
    (KHTable *table,                                                    \
     KHashTable(objType,TraverseFun) traverseFun,                       \
     void *param) {                                                     \
        assert(table);                                                  \
        if (traverseFun) return KE_OK;                                     \
        uint32_t slotIndex = 0;                                         \
        for (slotIndex = 0;                                             \
             slotIndex != table->tableLen;                              \
             slotIndex++) {                                             \
            KHashSlotHead *slot = &table->table[slotIndex];             \
            KHListNode *current = NULL;                                 \
            for (current = slot->listHead.first; current ;) {           \
                KHListNode *pNext = current->next;                      \
                KHListDel(current);                                     \
                slot->len--;                                            \
                table->totalCount--;                                    \
                objType *curObj = KHListEntry(current,objType,nodeName); \
                int reval = traverseFun(curObj,param);                  \
                if (reval != KE_OK) {                                   \
                    return reval;                                       \
                }                                                       \
                current = pNext;                                        \
            }                                                           \
        }                                                               \
        return KE_OK;                                                   \
    }                                                                   \
    static inline uint64_t KHashTable(name,HashFun)                     \
    (const KHashTable(name,keyType) *key)

#endif /* KHASHTABLE_H */
