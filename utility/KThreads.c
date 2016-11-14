
#include <unistd.h>
#include <pthread.h>

#include "KThreads.h"
#include "KList.h"
#include "KTools.h"
#include "KLog.h"


struct KThread {
    pthread_t thread;
    KThreadFun func;
    uint32_t flag;
    void *param;
};

static void *comSysThreadFun(void *param) {
    KThread_t *thread = (KThread_t *)param;
    thread->func(thread,thread->param);
    return NULL;
}

void kthreadInit(KThread_t *thread,KThreadFun func,void *param,uint32_t flag) {
    thread->func = func;
    thread->flag = flag;
    thread->param = param;

    pthread_create(&thread->thread,
                   NULL,
                   comSysThreadFun,
                   thread);
}

void kthreadFinal(KThread_t *thread) {
    void *p = NULL;
    pthread_join(thread->thread, &p);
}

KThread_t *CreateKThread(KThreadFun func,void *param,uint32_t flag) {
    KThread_t *newThread = ObjAlloc(KThread_t);
    kthreadInit(newThread,func,param,flag);
    return newThread;
}

void KThreadDestory(KThread_t *thread) {
    kthreadFinal(thread);
    ObjRelease(thread);
}

typedef struct KThreadPoolThread {
    KListNode threadListNode;
    KThread_t thread;
    KThreadPool_t *pool;
} KThreadPoolThread_t;

typedef struct KThreadPoolTask {
    KThreadPool_t *pool;
    KListNode taskListNode;
    KThreadPoolTaskFun func;
    void *param;
} KThreadPoolTask_t;

#define KTHREAD_POOL_FLAG_QUITING make_flag(1)

struct KThreadPool {
    KList taskList;
    pthread_cond_t taskCond;
    pthread_mutex_t taskLocker;
    KList threadList;
    uint32_t minThreadCount;
    uint32_t maxThreadCount;
    uint32_t flag;
};

static void threadPoolFunc(KThread_t *thread,void *param) {
    KThreadPoolThread_t *poolThread = (KThreadPoolThread_t *)param;
    KThreadPool_t *pool = poolThread->pool;

    while(!CheckFlag(pool->flag,KTHREAD_POOL_FLAG_QUITING)) {
        pthread_mutex_lock(&pool->taskLocker);
        if (KListEmpty(&pool->taskList)) {
            //waitcond
            pthread_cond_wait(&pool->taskCond,
                              &pool->taskLocker);
        }
        KListNode *node = KListHead(&pool->taskList);
        KListDel(node);
        pthread_mutex_unlock(&pool->taskLocker);
        KThreadPoolTask_t *task = KListEntry(node,KThreadPoolTask_t,taskListNode);
        task->func(pool,task,task->param);
        ObjRelease(task);
    }
    pthread_exit(NULL);
}

KThreadPool_t *CreateKThreadPool(uint32_t minThreadCount,uint32_t maxThreadCount) {
    KThreadPool_t *pool = ObjAlloc(KThreadPool_t);
    pool->minThreadCount = minThreadCount;
    pool->maxThreadCount = maxThreadCount;
    KListInit(&pool->taskList);
    KListInit(&pool->threadList);

    uint32_t i = 0;
    for (i = 0; i != minThreadCount; i++) {
        KThreadPoolThread_t *poolThread = ObjAlloc(KThreadPoolThread_t);
        poolThread->pool = pool;
        kthreadInit(&poolThread->thread,
                    threadPoolFunc,
                    poolThread,
                    KTHREAD_CREATE_THREAD_FLAG_NULL);

        KListAddTail(&pool->threadList,&poolThread->threadListNode);
    }

    return pool;
}

void DestoryKThreadPool(KThreadPool_t *pool) {
    SetFlag(pool->flag,KTHREAD_POOL_FLAG_QUITING);

    KListNode *cur = pool->threadList.next;
    while(cur != &pool->threadList) {
        KThreadPoolThread_t *poolThread = KListEntry(cur,
                                                     KThreadPoolThread_t,
                                                     threadListNode);
        cur = cur->next;
        kthreadFinal(&poolThread->thread);
        ObjRelease(poolThread);
    }

    cur = pool->taskList.next;
    while(cur != &pool->taskList) {
        KThreadPoolTask_t *poolTask = KListEntry(cur,
                                                 KThreadPoolTask_t,
                                                 taskListNode);
        cur = cur->next;
        ObjRelease(poolTask);
    }

    ObjRelease(pool);
}

KThreadPoolTask_t *KThreadPoolAddTask(KThreadPool_t *pool,
                                      KThreadPoolTaskFun func,
                                      void *param,
                                      uint32_t flag) {

    KThreadPoolTask_t *task = ObjAlloc(KThreadPoolTask_t);
    task->pool = pool;
    task->func = func;
    task->param = param;

    pthread_mutex_lock(&pool->taskLocker);
    KListAddTail(&pool->taskList,&task->taskListNode);
    pthread_cond_signal(&pool->taskCond);
    pthread_mutex_unlock(&pool->taskLocker);
    return NULL;
}