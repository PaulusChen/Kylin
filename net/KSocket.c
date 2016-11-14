
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "KylinTypes.h"
#include "KTools.h"
#include "KBuffer.h"
#include "KLog.h"

#define MAX_IP_LEN (16) // xxx.xxx.xxx.xxx\0

struct KSocket {
    int fd;
    char ip[MAX_IP_LEN];
    uint16_t port;
    uint32_t maxConn;
};

#include "KSocket.h"

KSocket_t *CreateKSocket() {
    KSocket_t *socket = ObjAlloc(KSocket_t);
    socket->fd = 0;
    ObjSetZero(socket->ip);
    socket->port = 0;
    socket->maxConn = 0;
    return socket;
}

void DestoryKSocket(KSocket_t *trans) {
    KSocketClose(trans);
    ObjRelease(trans);
}

int KSocketListen(KSocket_t *trans,const char *ip,uint16_t port,uint32_t maxConn) {
    assert(trans);
    assert(ip);

    if(strlen(ip) >= MAX_IP_LEN) {
        return KE_BAD_PARAM;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        KLogErrErrno("Failed socket creating.");
        return KE_SYS_OBJ_CREATE_FAILED;
    }
    fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL, 0));

    int optval = 1;
    if (setsockopt(fd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   (void *)&optval,
                   sizeof(optval)) == -1)
    {
        KLogErrErrno("Set reuse_addr failed.");
    }

    struct sockaddr_in addr;
    ObjSetZero(addr);
    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        KLogErrErrno("Bind the address [%s:%hu] Failed.",addr,port);
        KSocketClose(trans);
        return KE_SYS_OPT_FAILED;
    }

    if (listen(fd, maxConn) < 0)
    {
        KLogErrErrno("Listen the address [%s:%hu] Failed.",addr,port);
        KSocketClose(trans);
        return KE_SYS_OPT_FAILED;
    }

    trans->fd = fd;
    strcpy(trans->ip,ip);
    trans->port = port;
    trans->maxConn = maxConn;

    return KE_OK;
}

int KSocketConnect(KSocket_t *trans,const char *ip,uint16_t port) {
    assert(trans);
    assert(ip);

    if(strlen(ip) >= MAX_IP_LEN) {
        return KE_BAD_PARAM;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        KLogErrErrno("Failed socket creating.");
        return KE_SYS_OBJ_CREATE_FAILED;
    }
    fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL, 0));

    int optval = 1;
    if (setsockopt(fd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   (void *)&optval,
                   sizeof(optval)) == -1)
    {
        KLogErrErrno("Set reuse_addr failed.");
    }

    struct sockaddr_in addr;
    ObjSetZero(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int result = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (result == -1  &&  errno != EINPROGRESS)
    {
        KLogErrErrno("Connect to host failed, [%s:hu]",ip,port);
        KSocketClose(trans);
        return KE_SYS_OPT_FAILED;
    }

    trans->fd = fd;
    strcpy(trans->ip,ip);
    trans->port = port;
    trans->maxConn = 0;

    return KE_OK;
}

KSocket_t *KSocketAccept(KSocket_t *trans) {
    struct sockaddr_in client;
    int len = sizeof(client);
    int clientFd = accept(trans->fd,
                          (struct sockaddr *)&client,
                          (socklen_t *)&len);
    if (clientFd == -1)
    {
        KLogWarningErrno("Accept connection failed. ");
        return NULL;
    }

    fcntl(clientFd, F_SETFL, O_NONBLOCK | fcntl(clientFd, F_GETFL, 0));

    KSocket_t *clientSocket = CreateKSocket();
    clientSocket->fd = clientFd;
    return KE_OK;
}

int KSocketClose(KSocket_t *trans) {
    if(close(trans->fd) == 0) {
        KLogErrErrno("Close socket failed fd:[%"PRIu32"]. ",trans->fd);
        return KE_SYS_OPT_FAILED;
    }

    return KE_OK;
}

int KSocketRead(KSocket_t *trans,
                uint8_t *buf,
                uint64_t len) {
    assert(trans);
    assert(buf);

    int reval = recv(trans->fd,(char *)buf,len,0);

    if (reval < 0) {
        if (errno == EAGAIN || errno == EINTR) {
            sleep(1);

            reval = recv(trans->fd,(char *)buf,len,0);
        }
    }

    if (reval <= 0 )
    {
        KLogErrErrno("Recv socket error. ");
        return KE_SYS_OPT_FAILED;
    }

    return reval;
}

int KSocketWrite(KSocket_t *trans,
                 const uint8_t *buf,
                 uint64_t len) {
    assert(trans);
    assert(buf);

    int reval = send(trans->fd, (const char *)buf, len, MSG_NOSIGNAL);
    if (reval < 0) {
        if (errno == EAGAIN || errno == EINTR) {
            sleep(1);
            reval = send(trans->fd, (const char *)buf, len, MSG_NOSIGNAL);
        }
    }

    if (reval <= 0 )
    {
        KLogErrErrno("Send socket error. ");
        return KE_SYS_OPT_FAILED;
    }

    return reval;
}
