
#ifndef __KSTRING_H__
#define __KSTRING_H__

#include "KylinTypes.h"

typedef struct KStrRebuilder KStrRebuilder_t;

#ifdef __cplusplus
extern "C" {
#endif

KStrRebuilder_t *CreateKStrRebuilder(const char *pattern,
                                    const char *format);

void DestoryKStrRebuilder(KStrRebuilder_t *rebuilder);

int KStrRebuilderBuild(KStrRebuilder_t *rebuilder,
                       const char *origin,
                       char *newStr,
                       size_t newStrLen);

#ifdef __cplusplus
} //extern "C"
#endif

#endif /* __KSTRING_H__ */