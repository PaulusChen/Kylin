
#include <inttypes.h>

#include <unistd.h>
#include <pthread.h>

#include <event.h>
#include <event2/http.h>

#include "KHttpHelper.h"
#include "KTools.h"
#include "KLog.h"
#include "KylinTypes.h"

typedef struct KHttpWorker {
    pthread_t thread;
    struct event_base *base;
} KHttpWorker_t;

struct KHttpHelper {
    KHttpWorker_t *workerList;
    uint32_t workerCount;
};

void *httpWorkerFunc(void *param) {
    KHttpWorker_t *pworker = (KHttpWorker_t *)param;
    pworker->base = event_base_new();
    event_base_dispatch(pworker->base);
    return NULL;
}

KHttpHelper_t *KCreateHttpHelper(uint32_t workerCount) {
    KHttpHelper_t *helper = ObjAlloc(KHttpHelper_t);

    helper->workerCount = workerCount;
    helper->workerList = ArrayAlloc(KHttpWorker_t,workerCount);

    uint32_t i = 0;
    for (i = 0; i != workerCount; i++) {
        pthread_create(&helper->workerList[i].thread,
                       NULL,
                       httpWorkerFunc,
                       &helper->workerList[i]);
    }

    return helper;
}

void KDestoryHttpHelper(KHttpHelper_t *helper) {
    assert(helper);
    ObjRelease(helper);
}

uint64_t urlHash(const char *url) {
    return 0;
}

void http_helper_get_data_handler(struct evhttp_request *req,void *arg) {

}

int KHttpHelperGetData(KHttpHelper_t *helper,
                       const char *url,
                       KHttpHelperGetDataHandler handler) {

    uint64_t hash = urlHash(url);
    uint64_t workerIndex = hash%helper->workerCount;

    KHttpWorker_t *worker = &helper->workerList[workerIndex];

    struct evhttp_uri *uri = evhttp_uri_parse(url);
    int port = evhttp_uri_get_port(uri);
    if (port == -1) {
        port = 80;
    }

    struct evhttp_connection *cn = NULL;
    cn = evhttp_connection_base_new(worker->base,
                                    NULL,
                                    evhttp_uri_get_host(uri),
                                    port);

    struct evhttp_request *req = NULL;
    req = evhttp_request_new(http_helper_get_data_handler,NULL);

    const char *query = evhttp_uri_get_query(uri);
    const char *path = evhttp_uri_get_path(uri);
    size_t len = (query?strlen(query):0) + (path?strlen(path):0) + 1;
    char *pathQuery = "/";
    if (len > 1) {
        pathQuery = ArrayAlloc(char,len);
        sprintf(pathQuery,"%s?%s",path,query);
    }

    evhttp_make_request(cn,req,EVHTTP_REQ_GET,pathQuery);

    if (len > 1) {
        ArrayRelease(pathQuery);
    }
    return KE_OK;
}

