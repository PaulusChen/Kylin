
#ifndef __KBUFFER_H__
#define __KBUFFER_H__

#include <assert.h>

#include "KylinTypes.h"
#include "KTools.h"

#define KBuffer(type,name) GetTmpName(KBuf_##type,name)

#define KBufferDeclar(objType)                                          \
    typedef struct {                                                    \
        uint64_t writePos;                                              \
        uint64_t readPos;                                               \
        uint64_t capacity;                                              \
        objType *objList;                                               \
    } KBuffer(objType,buffer);                                          \
                                                                        \
    static inline void KBuffer(objType,Init)                            \
    (KBuffer(objType,buffer) *buf,uint64_t size) {                      \
        assert(buf);                                                    \
        buf->capacity = size;                                           \
        buf->objList = ArrayAlloc(objType,size);                        \
        buf->writePos = 0;                                              \
        buf->readPos = 0;                                               \
    }                                                                   \
                                                                        \
    static inline void KBuffer(objType,Clean)                           \
    (KBuffer(objType,buffer) *buf) {                                    \
        buf->writePos = 0;                                              \
        buf->readPos = 0;                                               \
    }                                                                   \
                                                                        \
    static inline void KBuffer(objType,Destory)                         \
    (KBuffer(objType,buffer) *buf) {                                    \
        KBuffer(objType,Clean)(buf);                                    \
        buf->capacity = 0;                                              \
        ArrayRelease(buf->objList);                                     \
    }                                                                   \
                                                                        \
    static inline uint64_t KBuffer(objType,GetLen)                      \
    (KBuffer(objType,buffer) *buf) {                                    \
        return buf->writePos - buf->readPos;                            \
    }                                                                   \
                                                                        \
    static inline void KBuffer(objType,_resize)                         \
    (KBuffer(objType,buffer) *buf,uint64_t size) {                      \
        assert(buf);                                                    \
        uint64_t orgPos = buf->capacity;                                \
        if(size > buf->capacity * 2) {                                  \
            buf->capacity = size;                                       \
        }                                                               \
        buf->capacity *= 2;                                             \
        buf->objList = ArrayReAlloc(buf->objList,                       \
                                    objType,                            \
                                    buf->capacity);                     \
        memset(buf->objList + (orgPos + 1) * sizeof(objType),           \
               0,buf->capacity - orgPos);                               \
    }                                                                   \
                                                                        \
    static inline uint64_t KBuffer(objType,AppendTail)                  \
    (KBuffer(objType,buffer) *buf,objType obj) {                        \
        assert(buf);                                                    \
        if (buf->writePos == buf->capacity) {                           \
            KBuffer(objType,_resize)(buf,buf->capacity);                \
        }                                                               \
        memcpy(&buf->objList[buf->writePos],                            \
               &obj,                                                    \
               sizeof(objType));                                        \
        buf->writePos++;                                                \
        return buf->writePos;                                           \
    }                                                                   \
                                                                        \
    static inline uint64_t KBuffer(objType,AppendListTail)              \
    (KBuffer(objType,buffer) *buf,                                      \
     objType *objList,                                                  \
     uint64_t count) {                                                  \
        assert(buf);                                                    \
        if (buf->writePos + count >= buf->capacity) {                   \
            KBuffer(objType,_resize)                                    \
                (buf,(count + buf->writePos) * sizeof(objType));        \
        }                                                               \
        memcpy(buf->objList + buf->writePos,                            \
               objList,                                                 \
               sizeof(objType) * count);                                \
        buf->writePos += count;                                         \
        return buf->writePos;                                           \
    }                                                                   \
                                                                        \
    static inline uint64_t KBuffer(objType,RemoveTail)                  \
    (KBuffer(objType,buffer) *buf) {                                    \
        assert(buf);                                                    \
        buf->writePos--;                                                \
        return buf->writePos;                                           \
    }                                                                   \
                                                                        \
    static inline uint64_t KBuffer(objType,RemoveListTail)              \
    (KBuffer(objType,buffer) *buf,uint64_t count) {                     \
        assert(buf);                                                    \
        buf->writePos -= count;                                         \
        return buf->writePos;                                           \
    }                                                                   \
                                                                        \
    static inline objType KBuffer(objType,GetWritePos)                  \
    (KBuffer(objType,buffer) *buf) {                                    \
        return buf->writePos;                                           \
    }                                                                   \
                                                                        \
    static inline objType KBuffer(objType,Peek)                         \
    (KBuffer(objType,buffer) *buf,uint64_t index) {                     \
        if (buf->writePos < buf->readPos + index) {                     \
            return KE_OUT_OF_RANGE;                                     \
        }                                                               \
        return buf->objList[buf->readPos + index];                      \
    }                                                                   \
                                                                        \
    static inline const objType* KBuffer(objType,Get)                   \
    (KBuffer(objType,buffer) *buf,uint64_t getLen) {                    \
        if (buf->readPos + getLen > buf->writePos) {                    \
            return NULL;                                                \
        }                                                               \
        uint64_t oldReadPos = buf->readPos;                             \
        buf->readPos += getLen;                                         \
        return buf->objList + oldReadPos;                               \
    }                                                                   \
                                                                        \
    static inline objType* KBuffer(objType,WriteBegin)                  \
    (KBuffer(objType,buffer) *buf,uint64_t index,uint64_t exWritePos) { \
        if (buf->capacity < buf->writePos + index + exWritePos) {       \
            KBuffer(objType,_resize)(buf,                               \
                                     buf->writePos + index + exWritePos); \
        }                                                               \
        buf->writePos += exWritePos;                                    \
        return buf->objList + buf->readPos + index;                     \
    }                                                                   \
                                                                        \
    static inline objType* KBuffer(objType,WriteEnd)                    \
    (KBuffer(objType,buffer) *buf,uint64_t exWritePos) {                \
        return  KBuffer(objType,WriteBegin)(buf,                        \
                                            buf->writePos - buf->readPos, \
                                            exWritePos);                \
    }

#define KBufferImpl(objType)

KBufferDeclar(uint8_t)

#endif /* __KBUFFER_H__ */