
#ifndef __HTTPHELPER_H__
#define __HTTPHELPER_H__
#include "KylinConfig.h"

typedef struct KHttpHelper KHttpHelper_t;

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*KHttpHelperGetDataHandler)(uint8_t *data,uint64_t size);

KHttpHelper_t *KCreateHttpHelper(uint32_t workerCount);

void KDestoryHttpHelper(KHttpHelper_t *helper);

int KHttpHelperGetData(KHttpHelper_t *helper,
                       const char *url,
                       KHttpHelperGetDataHandler handler);

#ifdef __cplusplus
} //extern "C"
#endif

#endif /* HTTPHELPER_H */