
#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "KylinConfig.h"

extern const char *VErrorString[];

const char *KGetErrStr(KErrorCode errval);

#define SetFlag(val,flag) (val)|=(flag)
#define ClearFlag(val,flag) (val)=(val)&~(flag)
#define CheckFlag(val,flag) (!!((val)&(flag)))

#define GET_MASK(len) ((1LL<<(len)) - 1)
#define GetBitmapByteLen(bitLen)        ((bitLen)/8)
#define CheckNULLPtr(ptr)                       \
    if(NULL == ptr) return KE_NULL_PTR

#define InitThrow()                             \
    int __VOOLE_THROW_REVAL = KE_OK

#define Try(fun)                                                        \
    if((__VOOLE_THROW_REVAL = (fun)) < KE_OK) return __VOOLE_THROW_REVAL

#define TryF(fun,tag)                                                   \
    if((__VOOLE_THROW_REVAL = (fun)) < KE_OK) goto tag

#define ReturnThrow() return __VOOLE_THROW_REVAL

#define GetTryReval() __VOOLE_THROW_REVAL

#define KCheckInit(type,success) \
    type __KYLIN_CHECK_REVAL = success; \
    type __KYLIN_CHECK_SUCCESS = success;

#define KCheckEQ(fun,tag)                       \
    if((__KYLIN_CHECK_REVAL = (fun)) != __KYLIN_CHECK_SUCCESS) goto tag

#define KCheckReval() __KYLIN_CHECK_REVAL

/////////////////////////////TypeDef//////////////////////////////
#define TypeDefName(type) type##Def

#define TypeDefObj(type) type##Def##Obj

#define ConstructorName(type)                   \
    type##Constructor__

#define ConstructorOfType(type,InitParamType)                   \
    void ConstructorName(type)(type *obj,InitParamType *param)

#define DestructorName(type)                    \
    type##Destuctor__

#define DestructorOfType(type)                  \
    void DestructorName(type)(type *obj)

#define DeclareType(type)                           \
    typedef struct {                                \
        void *parent;                               \
        const char *typename;                       \
        void (*InitFunc)(void *obj,void *param);    \
        void (*DesFunc)(void *obj);                 \
    } TypeDefName(type);                            \
    extern TypeDefName(type) TypeDefObj(type)

#define ImplementType(type)                                             \
    TypeDefName(type) TypeDefObj(type) = {                              \
        .parent = NULL,                                                 \
        .typename = #type,                                              \
        .InitFunc = (void (*)(void *,void *)) ConstructorName(type),    \
        .DesFunc = (void (*)(void *))DestructorName(type) }

#define ArrayAlloc(type,size) \
    (type *)calloc(size,sizeof(type))

#define ArrayRelease(obj)                         \
    free(obj);

#define ObjAlloc(type)                          \
    (type *)calloc(1,sizeof(type))

#define ObjRelease(obj)                         \
    free(obj);

#define NewObj(type,name,param)                     \
    type *name = (type *)calloc(1,sizeof(type));    \
    TypeDefObj(type).InitFunc(name,param)

#define InitialObj(type,name,param)             \
    TypeDefObj(type).InitFunc(name,param)

#define DeleteObj(type,name)                    \
    TypeDefObj(type).DesFunc(name);     \
    free(name)

#define CleanObj(type,name)                                             \
    TypeDefObj(type).DesFunc(name)

#define likely(x) __builtin_expect(!!(x),1)

#define unlikely(x) __builtin_expect(!!(x),0)

#define offsetoftype(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define vcontainer_of(ptr, type, member) ({                             \
            const typeof(((type *)0)->member)*__mptr = (ptr);           \
            (type *)((char *)__mptr - offsetoftype(type, member)); })


static inline uint64_t GetRadixFloor(uint64_t val,uint64_t radix) {
    return (val/radix)*radix;
}

static inline uint64_t GetRadixCeil(uint64_t val,uint64_t radix) {
    return ((val+radix - 1)/radix)*radix;
}

static inline void TestHeap() {
    void *pTest = malloc(GetMB(10));
    free(pTest);
}

typedef struct {
    struct timespec begTime;
} PerfToolsTimer;

static inline void PerfToolsTimerReset(PerfToolsTimer *timer) {
    clock_gettime(CLOCK_REALTIME,&timer->begTime);
}

//milliseconds
static inline uint64_t PerfToolsTimerSpan(PerfToolsTimer *timer) {
    struct timespec nowTime;
    clock_gettime(CLOCK_REALTIME,&nowTime);

    uint64_t reval = ((nowTime.tv_sec - timer->begTime.tv_sec - 1) * 1000);
    reval += (nowTime.tv_nsec - timer->begTime.tv_nsec + 1000000000LL)/1000000;
    return reval;
}

#ifdef __cplusplus
extern "C" {
#endif

    uint8_t contSignFromLowBit(uint64_t bitmap);
    int bitMapCheck(const uint8_t *bitmap,size_t beg,size_t *len);
    int bitMapSet(uint8_t *bitmap,size_t beg,size_t len);

#ifdef __cplusplus
} //extern "C"
#endif

#endif
