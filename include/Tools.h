
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
#include <stdint.h>

#include "KylinConfig.h"

#ifndef __cplusplus
#define bool uint8_t

#define true 1

#define false 0
#endif

#define KILO_BYTE_FACTOR (1024)
#define MEGA_BYTE_FACTOR (1024 * KILO_BYTE_FACTOR)
#define GIGA_BYTE_FACTOR (1024LL * MEGA_BYTE_FACTOR)
#define TRILLION_BYTE_FACTOR (1024LL * GIGA_BYTE_FACTOR)

#define GetKB(val) ((val) * KILO_BYTE_FACTOR)
#define GetMB(val) ((val) * MEGA_BYTE_FACTOR)
#define GetGB(val) ((val) * GIGA_BYTE_FACTOR)
#define GetTB(val) ((val) * TRILLION_BYTE_FACTOR)

#define make_flag(pos) (1<<pos)

extern const char *VErrorString[];

typedef enum {
    VE_OK = 0,
    VE_UNKNOW = -1,
    VE_NULL_PTR = -2,
    VE_CONTAINER_FULL = -3
} VErrorCode;

const char *VlssGetErrStr(VErrorCode errval);

#define SetFlag(val,flag) (val)|=(flag)
#define ClearFlag(val,flag) (val)=(val)&~(flag)
#define CheckFlag(val,flag) (!!((val)&(flag)))

#define GET_MASK(len) ((1LL<<(len)) - 1)
#define GetBitmapByteLen(bitLen)        ((bitLen)/8)

#define _in 

#define _out 

#define CheckNULLPtr(ptr)                       \
    if(NULL == ptr) return VE_NULL_PTR

#define InitThrow()                             \
    int __VOOLE_THROW_REVAL = VE_OK

#define Try(fun)                                                        \
    if((__VOOLE_THROW_REVAL = (fun)) < VE_OK) return __VOOLE_THROW_REVAL

#define TryF(fun,tag)                                                   \
    if((__VOOLE_THROW_REVAL = (fun)) < VE_OK) goto tag

#define ReturnThrow() return __VOOLE_THROW_REVAL

#define GetTryReval() __VOOLE_THROW_REVAL


/////////////////////////////TypeDef//////////////////////////////
#define TypeDefName(type) type##Def

#define TypeDefObj(type) type##Def##Obj


#define DeclareType(type)                           \
    typedef struct {                                \
        void *parent;                               \
        const char *typename;                       \
        void (*InitFunc)(void *obj,void *param);    \
        void (*DesFunc)(void *obj);                 \
    } TypeDefName(type);                            \
    extern TypeDefName(type) TypeDefObj(type)

#define ConstructorName(type)                   \
    type##Constructor__

#define ConstructorOfType(type,InitParamType)                   \
    void ConstructorName(type)(type *obj,InitParamType *param)

#define DestructorName(type)                    \
    type##Destuctor__

#define DestructorOfType(type)                  \
    void DestructorName(type)(type *obj)

#define ImplementType(type)                                             \
    TypeDefName(type) TypeDefObj(type) = {                              \
        .parent = NULL,                                                 \
        .typename = #type,                                              \
        .InitFunc = (void (*)(void *,void *)) ConstructorName(type),    \
        .DesFunc = (void (*)(void *))DestructorName(type) }

#define NewObj(type,name,param)                     \
    type *name = (type *)calloc(1,sizeof(type));    \
    TypeDefObj(type).InitFunc(name,param)

#define InitialObj(type,name,param)             \
    TypeDefObj(type).InitFunc(name,param)

#define DeleteObj(type,name)                    \
    TypeDefObj(type).DesFunc(name);             \
    free(name)

#define CleanObj(type,name)                     \
    TypeDefObj(type).DesFunc(name)

#define offsetoftype(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define vcontainer_of(ptr, type, member) ({                             \
            const typeof(((type *)0)->member)*__mptr = (ptr);           \
            (type *)((char *)__mptr - offsetoftype(type, member)); })


#define ObjAlloc(type)                          \
    (type *)calloc(1,sizeof(type))

#define ObjRelease(obj)                         \
    free(obj);

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
