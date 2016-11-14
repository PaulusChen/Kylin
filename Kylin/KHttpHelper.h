
#ifndef __HTTPHELPER_H__
#define __HTTPHELPER_H__
#include "KylinConfig.h"

typedef struct KHttpHelper KHttpHelper_t;

#ifdef __cplusplus
extern "C" {
#endif

KHttpHelper_t *KCreateHttpHelper(const char *url);

void KDestoryHttpHelper(KHttpHelper_t *helper);

int64_t KHttpHelperGetContentLen(KHttpHelper_t *helper);

int KHttpHelperGetContentType(KHttpHelper_t *helper,
                              char *contentType,
                              uint64_t len);

int KHttpHelperDownloadRange(KHttpHelper_t *helper,
                            uint8_t *buf,
                            uint64_t offset,
                             uint64_t *len);

#ifdef __cplusplus
} //extern "C"
#endif

#endif /* HTTPHELPER_H */