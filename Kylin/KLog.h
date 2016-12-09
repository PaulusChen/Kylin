
#ifndef __KLOG_H__
#define __KLOG_H__
#include <syslog.h>

#include "KylinTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

    void KLogInit(const char *progName);
    void KLog(bool pe,uint32_t type,const char *func,const char *format,...);

#ifdef __cplusplus
} //extern "C"
#endif


#define KLogDebug(fmt,args...)                   \
    KLog(false,LOG_DEBUG,__FUNCTION__,fmt,##args)

#define KLogInfo(fmt,args...)                    \
    KLog(false,LOG_INFO,__FUNCTION__,fmt,##args)

#define KLogNotice(fmt,args...)                  \
    KLog(false,LOG_NOTICE,__FUNCTION__,fmt,##args)

#define KLogWarning(fmt,args...)                 \
    KLog(false,LOG_WARNING,__FUNCTION__,fmt,##args)

#define KLogErr(fmt,args...)                     \
    KLog(false,LOG_ERR,__FUNCTION__,fmt,##args)

#define KLogDebugErrno(fmt,args...)              \
    KLog(true,LOG_DEBUG,__FUNCTION__,fmt,##args)

#define KLogInfoErrno(fmt,args...)               \
    KLog(true,LOG_INFO,__FUNCTION__,fmt,##args)

#define KLogNoticeErrno(fmt,args...)             \
    KLog(true,LOG_NOTICE,__FUNCTION__,fmt,##args)

#define KLogWarningErrno(fmt,args...)            \
    KLog(true,LOG_WARNING,__FUNCTION__,fmt,##args)

#define KLogErrErrno(fmt,args...)                \
    KLog(true,LOG_ERR,__FUNCTION__,fmt,##args)


#define VLOG_OPEN_LOG_BLOCK 1

#ifdef VLOG_OPEN_LOG_BLOCK

#define KLogBlockDebug()                         \
    if (true)                                   \

#define KLogBlockInfo()                          \
    if (true)                                   \

#define KLogBlockNotice()                        \
    if (true)                                   \

#define KLogBlockWarning()                       \
    if (true)

#else

#define KLogBlockDebug()                         \
    if (false)                                  \

#define KLogBlockInfo()                          \
    if (false)                                  \

#define KLogBlockNotice()                        \
    if (false)                                  \

#define KLogBlockWarning()                       \
    if (false)

#endif

void KKLogClose();


#endif