
#ifndef __HTTPHELPER_H__
#define __HTTPHELPER_H__

#include "KylinConfig.h"
#include "KylinTypes.h"

typedef struct KHttpHelper KHttpHelper_t;
typedef struct KHttpHelperRequestTask KHttpHelperRequestTask_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*KHttpHelperGetDataHandler)(KHttpHelperRequestTask_t *task,
                                          void *param);

KHttpHelper_t *KCreateHttpHelper(uint32_t workerCount);

void KDestoryHttpHelper(KHttpHelper_t *helper);

int KHttpHelperRequest(KHttpHelper_t *helper,
                       const char *url,
                       KHttpHelperGetDataHandler handler,
                       void *userParam);

int KHttpHelperRequestHead(KHttpHelper_t *helper,
                           const char *url,
                           KHttpHelperGetDataHandler handler,
                           void *userParam);

int KHttpHelperRequestRange(KHttpHelper_t *helper,
                            const char *url,
                            size_t offset,
                            size_t len,
                            KHttpHelperGetDataHandler handler,
                            void *userParam);

size_t KHttpHelperTaskGetRespBodySize(KHttpHelperRequestTask_t *task);

size_t KHttpHelperTaskGetRespBodyData(KHttpHelperRequestTask_t *task,
                                      char *buf,size_t offset,size_t len);

const char *KHttpHelperTaskGetContentType(KHttpHelperRequestTask_t *task);

int64_t KHttpHelperTaskGetContentLen(KHttpHelperRequestTask_t *task);

int KHttpHelperTaskGetStatus(KHttpHelperRequestTask_t *task);

#ifdef __cplusplus
} //extern "C"
#endif

#endif /* HTTPHELPER_H */