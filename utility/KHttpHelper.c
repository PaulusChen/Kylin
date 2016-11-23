
#include <inttypes.h>

#include <unistd.h>
#include <pthread.h>

#include <event.h>
#include <event2/http.h>
#include <event2/dns.h>

#include "KHttpHelper.h"
#include "KTools.h"
#include "KLog.h"
#include "KylinTypes.h"

#define HTTP_CREATED 201
#define HTTP_PARTIAL_CONTENT 206

typedef struct KHttpWorker {
    pthread_t thread;
    struct event_base *base;
    struct evdns_base *dnsbase;
    //pthread_mutex_t baseLock;
    pthread_cond_t cond;
} KHttpWorker_t;

struct KHttpHelperRequestTask {
    struct evhttp_connection *cn ;
    struct evhttp_uri *uri;
    struct evhttp_request *req;
    struct evbuffer *buffer;
    enum evhttp_cmd_type cmdType;
    KHttpWorker_t *worker;
    int status;
    void *userParam;
    KHttpHelperGetDataHandler handler;
    char *pathQuery;
};

struct KHttpHelper {
    KHttpWorker_t *workerList;
    uint32_t workerCount;
};

static int khttpHelperTaskClear(KHttpHelperRequestTask_t *task) {

    if (task->cn) {
        evhttp_connection_free(task->cn);
        task->cn = NULL;
    }

    if (task->uri) {
        evhttp_uri_free(task->uri);
        task->uri = NULL;

    }

    task->req = NULL;

    if (task->buffer) {
        evbuffer_free(task->buffer);
        task->buffer = NULL;
    }

    task->handler = NULL;

    if (task->pathQuery) {
        ArrayRelease(task->pathQuery);
        task->pathQuery = NULL;
    }

    return KE_OK;
}

static void httpHelperGetDataHandler(struct evhttp_request *req,void *arg);
static int khttpHelperTaskInit(KHttpHelperRequestTask_t *task,
                               KHttpWorker_t *worker,
                               const char *url,
                               KHttpHelperGetDataHandler handler) {
    khttpHelperTaskClear(task);

    task->handler = handler;
    task->uri = evhttp_uri_parse(url);
    task->worker = worker;

    if (task->buffer == NULL) {
        task->buffer = evbuffer_new();
    }

    int port = evhttp_uri_get_port(task->uri);
    if (port == -1) {
        port = 80;
    }

    const char *host = evhttp_uri_get_host(task->uri);
    task->cn = NULL;
    //pthread_mutex_lock(&worker->baseLock);
    task->cn = evhttp_connection_base_new(worker->base,
                                          worker->dnsbase,
                                          host,
                                          port);

    //pthread_mutex_unlock(&worker->baseLock);

    task->req = NULL;

    const char *query = evhttp_uri_get_query(task->uri);
    const char *path = evhttp_uri_get_path(task->uri);
    size_t len = (query?strlen(query):0) + (path?strlen(path):0) + 1;
    task->pathQuery = NULL;
    if (len > 1) {
        task->pathQuery = ArrayAlloc(char,len);

        if (query) {
            sprintf(task->pathQuery,"%s?%s",path,query);
        } else {
            sprintf(task->pathQuery,"%s",path);
        }
    }

    //KLogDebug("Init a task, url :[%s] host :[%s] port:[%hu] pathQuery :[%s]",
    //          url,host,port,task->pathQuery);

    task->req = evhttp_request_new(httpHelperGetDataHandler,task);

    evhttp_add_header(evhttp_request_get_output_headers(task->req),"Host",host);

    evhttp_add_header(evhttp_request_get_output_headers(task->req),
                      "Connection","keep-alive");

    return KE_OK;
}

static int khttpHelperTaskRun(KHttpHelperRequestTask_t *task,
                              enum evhttp_cmd_type cmdType) {
    const char *requestStr = "/";
    if (task->pathQuery) {
        requestStr = task->pathQuery;
    }

    task->cmdType = cmdType;

    evhttp_make_request(task->cn,task->req,cmdType,requestStr);
    return KE_OK;
}

static void *httpWorkerFunc(void *param) {
    KHttpWorker_t *pworker = (KHttpWorker_t *)param;
    KLogInfo("KHttpHelper Running...");
    event_base_dispatch(pworker->base);
    KLogInfo("KHttpHelper Quiting...");
    return NULL;
}

void httpWorkerTimeoutHandler(evutil_socket_t fd, short what, void *arg) {

}


void httpWorkerAddTimer(struct event_base *base,uint64_t time)
{
    struct timeval one_sec = { time, 0 };
    struct event *ev;
    /* We're going to set up a repeating timer to get called called 100
       times. */
    ev = event_new(base, -1, EV_PERSIST, httpWorkerTimeoutHandler, NULL);
    event_add(ev, &one_sec);
}


KHttpHelper_t *KCreateHttpHelper(uint32_t workerCount) {
    KHttpHelper_t *helper = ObjAlloc(KHttpHelper_t);
    helper->workerCount = workerCount;
    helper->workerList = ArrayAlloc(KHttpWorker_t,workerCount);

    uint32_t i = 0;
    for (i = 0; i != workerCount; i++) {
        KHttpWorker_t *pWorker = &helper->workerList[i];
        pWorker->base = event_base_new();
        pWorker->dnsbase = evdns_base_new(pWorker->base,1);
        //pthread_mutex_init(&pWorker->baseLock,NULL);

        httpWorkerAddTimer(pWorker->base,1);
        pthread_create(&pWorker->thread,
                       NULL,
                       httpWorkerFunc,
                       pWorker);
    }

    return helper;
}

void KDestoryHttpHelper(KHttpHelper_t *helper) {
    assert(helper);
    size_t i = 0;
    if (helper->workerList) {
        for (i = 0; i != helper->workerCount ;i++) {
            event_base_loopexit(helper->workerList[i].base,NULL);
            event_base_free(helper->workerList[i].base);
            evdns_base_free(helper->workerList[i].dnsbase,1);
        }
        for (i = 0; i != helper->workerCount ;i++) {
            pthread_join(helper->workerList[i].thread,NULL);
        }
    }

    ObjRelease(helper);
}

static inline uint32_t urlHash(const char *url) {

    int len = strlen(url);

    const char *end = url + len;
    uint32_t h = 0, g = 0;
    while (url < end) {
        h = (h << 4) + *url++;
        if ((g = (h & 0xF0000000))) {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }
    return h;
}

static void httpHelperGetDataHandler(struct evhttp_request *req,void *arg) {
    assert(arg);

    KHttpHelperRequestTask_t *task = (KHttpHelperRequestTask_t *)arg;
    const char *newLocation = NULL;

    if (req == NULL) {
        KLogDebug("Request timeout? "
                  "callback function got req that point to NULL");

        task->status = KE_TIMEOUT;
        task->handler(task,task->userParam);
    }


    int responseCode = evhttp_request_get_response_code(req);
    switch(responseCode)
    {

    case HTTP_OK:
    case HTTP_NOCONTENT:
    case HTTP_PARTIAL_CONTENT:
        evbuffer_add_buffer(task->buffer,evhttp_request_get_input_buffer(req));
        task->handler(task,task->userParam);
        break;
    case HTTP_CREATED :
    case HTTP_MOVEPERM:
    case HTTP_MOVETEMP:
        newLocation = evhttp_find_header(evhttp_request_get_input_headers(req),
                                                     "Location");
        if (!newLocation) {
            KLogErr("I got a kind of *MOVE* response code "
                    "without Location header item.");

            task->status = KE_UNKNOW;
            task->handler(task,task->userParam);
            goto FINISH;
        }
        if((task->status =
            khttpHelperTaskInit(task,
                                task->worker,
                                newLocation,
                                task->handler)) != KE_OK) {

            task->handler(task,task->userParam);
            goto FINISH;
        }
        if((task->status = khttpHelperTaskRun(task,task->cmdType)) != KE_OK) {
            task->handler(task,task->userParam);
            goto FINISH;
        }
    default:
        //unknow error
        KLogErr("Send request failed...ResponseCode : [%d]",responseCode);
    }

FINISH:
    khttpHelperTaskClear(task);
    ObjRelease(task);
}

size_t KHttpHelperTaskGetRespBodySize(KHttpHelperRequestTask_t *task) {
    if(task == NULL) {
        return 0;
    }

    return evbuffer_get_length(task->buffer);
}

const char *KHttpHelperTaskGetContentType(KHttpHelperRequestTask_t *task) {

    const char *contentType =
        evhttp_find_header(evhttp_request_get_input_headers(task->req),
                           "Content-Type");

    return contentType;
}

int64_t KHttpHelperTaskGetContentLen(KHttpHelperRequestTask_t *task) {
    const char *contentLen =
        evhttp_find_header(evhttp_request_get_input_headers(task->req),
                           "Content-Length");

    if (contentLen == NULL) {
        return KE_NO_ENTRY;
    }

    return atoll(contentLen);
}

int KHttpHelperTaskGetStatus(KHttpHelperRequestTask_t *task) {
    return task->status;
}

size_t KHttpHelperTaskGetRespBodyData(KHttpHelperRequestTask_t *task,
                                      char *buf,
                                      size_t offset,
                                      size_t len) {
    if(task == NULL) {
        return 0;
    }

    size_t totalLen = KHttpHelperTaskGetRespBodySize(task);
    if (totalLen < offset) {
        return 0;
    }

    const uint8_t *data = evbuffer_pullup(task->buffer,-1);
    if (data == NULL) {
        return 0;
    }

    if (offset + len > totalLen) {
        len = totalLen - offset;
    }

    memcpy(buf,data + offset,len);

    return len;
}

int KHttpHelperRequest(KHttpHelper_t *helper,
                       const char *url,
                       KHttpHelperGetDataHandler handler,
                       void *userParam) {
    uint32_t hash = urlHash(url);
    uint32_t workerIndex = hash%helper->workerCount;
    KHttpWorker_t *worker = &helper->workerList[workerIndex];
    KHttpHelperRequestTask_t *task = ObjAlloc(KHttpHelperRequestTask_t);

    InitThrow();
    Try(khttpHelperTaskInit(task,worker,url,handler));
    task->userParam = userParam;
    Try(khttpHelperTaskRun(task,EVHTTP_REQ_GET));

    return KE_OK;
}

int KHttpHelperRequestHead(KHttpHelper_t *helper,
                           const char *url,
                           KHttpHelperGetDataHandler handler,
                           void *userParam) {
    uint64_t hash = urlHash(url);
    uint64_t workerIndex = hash%helper->workerCount;
    KHttpWorker_t *worker = &helper->workerList[workerIndex];
    KHttpHelperRequestTask_t *task = ObjAlloc(KHttpHelperRequestTask_t);

    InitThrow();
    Try(khttpHelperTaskInit(task,worker,url,handler));
    task->userParam = userParam;
    Try(khttpHelperTaskRun(task,EVHTTP_REQ_HEAD));

    return KE_OK;
}

int KHttpHelperRequestRange(KHttpHelper_t *helper,
                            const char *url,
                            size_t offset,
                            size_t len,
                            KHttpHelperGetDataHandler handler,
                            void *userParam) {
    uint64_t hash = urlHash(url);
    uint64_t workerIndex = hash%helper->workerCount;
    KHttpWorker_t *worker = &helper->workerList[workerIndex];
    KHttpHelperRequestTask_t *task = ObjAlloc(KHttpHelperRequestTask_t);

    InitThrow();
    Try(khttpHelperTaskInit(task,worker,url,handler));
    task->userParam = userParam;

    char rangeStr[256];
    sprintf(rangeStr,"bytes=%"PRIu64"-%"PRIu64,offset,offset + len);
    evhttp_add_header(evhttp_request_get_output_headers(task->req),
                      "Range",
                      rangeStr);

    Try(khttpHelperTaskRun(task,EVHTTP_REQ_GET));

    return KE_OK;
}


