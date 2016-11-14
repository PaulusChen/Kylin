#ifndef __KTHREAD_POOL_H__
#define __KTHREAD_POOL_H__

#include "KylinTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct KThread KThread_t;
typedef void (*KThreadFun)(KThread_t *thread,void *param);

#define KTHREAD_CREATE_THREAD_FLAG_NULL 0

KThread_t *CreateKThread(KThreadFun func,void *param,uint32_t flag);
void KThreadDestory(KThread_t *thread);


typedef struct KThreadPoolTask KThreadPoolTask_t;
typedef struct KThreadPool KThreadPool_t;

typedef void (*KThreadPoolTaskFun)(KThreadPool_t *pool,
                                  KThreadPoolTask_t *task,
                                  void *param);

KThreadPool_t *CreateKThreadPool(uint32_t minThreadCount,uint32_t maxThreadCount);

void DestoryKThreadPool(KThreadPool_t *pool);

#define KTHREAD_POOL_ADD_TASK_FLAG_NULL 0

KThreadPoolTask_t *KThreadPoolAddTask(KThreadPool_t *pool,
                                      KThreadPoolTaskFun func,
                                      void *param,
                                      uint32_t flag);

#ifdef __cplusplus
} //extern "C"
#endif

#endif /* __KTHREAD_POOL_H__ */