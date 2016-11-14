
#ifndef __KSOCKET_H__
#define __KSOCKET_H__

#include "KBuffer.h"

struct KSocket;
typedef struct KSocket KSocket_t;

KSocket_t *CreateKSocket();

void DestoryKSocket(KSocket_t *trans);

int KSocketConnect(KSocket_t *trans,const char *addr,uint16_t port);

int KSocketClose(KSocket_t *trans);

int KSocketRead(KSocket_t *trans,
                uint8_t *buf,
                uint64_t len);

int KSocketWrite(KSocket_t *trans,
                 const uint8_t *buf,
                 uint64_t len);

#endif /* __KTRANSFER_H__ */



