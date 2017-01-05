
#include "KTools.h"

const char *KErrorString[] = {
    "KE_OK",
    "KE_SYS_ERR",
    "KE_UNKNOW",
    "KE_NULL_PTR",
    "KE_NO_ENTRY",
    "KE_OUT_OF_RANGE",
    "KE_CONTAINER_FULL",
    "KE_DB_ERROR",
    "KE_SYNTAX_ERROR",
    "KE_SYS_OBJ_CREATE_FAILED",
    "KE_SYS_OPT_FAILED",
    "KE_BAD_PARAM",
    "KE_3RD_PART_LIBS_ERROR",
    "KE_NO_MEMORY",
    "KE_TIMEOUT",
    "KE_AGAIN",
    "KE_RELEASE_EARLIER",
    "KE_MAX_ERROR_CODE",
    "End of error list",
    "End of error list",
    "End of error list",
    "End of error list"
};

const char *KGetErrStr(int errval) {
    if(errval > 0) return NULL;

    return KErrorString[errval * -1];
}

uint64_t swapByteOrder64(uint64_t x)
{
    x = ((x << 8) & 0xFF00FF00FF00FF00ULL) | ((x >> 8) & 0x00FF00FF00FF00FFULL);
    x = ((x << 16) & 0xFFFF0000FFFF0000ULL) | ((x >> 16) & 0x0000FFFF0000FFFFULL);
    return (x >> 32) | (x << 32);
}

int bitMapCheck(const uint8_t *bitmap,size_t beg,size_t *len) {
    assert(bitmap);
    assert(len);

    uint64_t *pCheck = (uint64_t *)bitmap;

    size_t beginBMI = GetRadixFloor(beg,64)/64;
    uint8_t beginBit = beg%64;
    uint64_t checkVal = pCheck[beginBMI];
    uint64_t bitCount = contSignFromLowBit(checkVal>>beginBit);
    if (bitCount != (64U - beginBit)) {
        goto BIT_CHECK_FINISH;
    }

    size_t endBMI = GetRadixFloor(beg + *len,64)/64;
    if (beginBMI == endBMI) return KE_OK;

    uint32_t i = 0;
    for (i = beginBMI + 1; i < endBMI; i++) {
        uint8_t nsPos = contSignFromLowBit(pCheck[i]);
        bitCount += nsPos;
        if (nsPos != 64) {
            goto BIT_CHECK_FINISH;
        }
    }

    checkVal = pCheck[endBMI];
    uint8_t lastBitCount = contSignFromLowBit(checkVal);
    bitCount += lastBitCount;
BIT_CHECK_FINISH:
    *len = (*len < bitCount ? *len : bitCount);
    return KE_OK;
}

uint64_t bitMapGetSetMap(uint8_t beg,size_t len) {
    if (len == 64) {
        return 0xFFFFFFFFFFFFFFFFLL;
    }

    uint64_t reval = 1LL<<len;
    reval -= 1;
    reval <<= beg;
    return reval;
}

int bitMapSet(uint8_t *bitmap,size_t beg,size_t len) {

    uint64_t *pSet = (uint64_t *)bitmap;
    size_t beginBMI = GetRadixFloor(beg,64)/64;
    uint8_t beginBit = beg%64;
    uint8_t beginLen = (uint8_t)(64 - beginBit);
    size_t mapIndex = beginBMI;
    beginLen = (beginLen > len ? len : beginLen);
    uint64_t setMask = bitMapGetSetMap(beginBit,beginLen);
    pSet[mapIndex] |= setMask;
    mapIndex++;

    size_t endBMI = GetRadixFloor(beg + len,64)/64;
    uint32_t i = 0;
    for (i = mapIndex; i < endBMI; i++) {
        pSet[i] |= 0xFFFFFFFFFFFFFFFF;
    }

    if (beginBMI != endBMI) {
        uint8_t lastBit = (beg+len)%64;
        setMask = bitMapGetSetMap(0,lastBit);
        pSet[endBMI] |= setMask;
    }

    return KE_OK;
}

uint8_t contSignFromLowBit(uint64_t bitmap) {
    bitmap = ~bitmap;

    if (!bitmap) {
        return 64;
    }

    uint8_t index = 63;
    if (bitmap & 0x00000000FFFFFFFF) {
        index -= 32;
    } else {
        bitmap >>= 32;
    }
    bitmap &= 0x00000000FFFFFFFF;

    if (bitmap & 0x0000FFFF) {
        index -= 16;
    } else {
        bitmap >>= 16;
    }
    bitmap &= 0x0000FFFF;

    if (bitmap & 0x00FF) {
        index -= 8;
    } else {
        bitmap >>= 8;
    }
    bitmap &= 0x00FF;

    if (bitmap & 0x0F) {
        index -= 4;
    } else {
        bitmap >>= 4;
    }
    bitmap &= 0x0F;

    if (bitmap & 0x3) {
        index -= 2;
    } else {
        bitmap >>= 2;
    }
    bitmap &= 0x3;

    if (bitmap & 0x1) {
        index -= 1;
    } else {
        bitmap >>= 1;
    }

    return index;
}
