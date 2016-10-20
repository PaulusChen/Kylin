
#ifndef __HTTPHELPER_H__
#define __HTTPHELPER_H__
#include "KylinConfig.h"

typedef struct KHttpHelper KHttpHelper_t;

KHttpHelper_t *KCreateHttpHelper(const char *url);

void DistoryHttpHelper(KHttpHelper_t *helper);

int64_t KHttpHelperGetContentLen(KHttpHelper_t *helper);

int KHttpHelperDownloadRange(KHttpHelper_t *helper,
                            uint8_t *buf,
                            uint64_t offset,
                             uint64_t *len);

#endif /* HTTPHELPER_H */