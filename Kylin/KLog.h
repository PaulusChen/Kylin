
#ifndef __KLOG_H__
#define __KLOG_H__
#include <syslog.h>

#include "KylinTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

    extern void KLogInit(const char *progName);

    extern void KLog(bool pe,uint32_t type,const char *func,const char *format,...);

    extern void KKLogClose();

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

#define KLogBlockDebugIf(cond)                  \
    if (true && cond)                           \

#define KLogBlockInfoIf(cond)                   \
    if (true && cond)                           \

#define KLogBlockNoticeIf(cond)                 \
    if (true && cond)                           \

#define KLogBlockWarningIf(cond)                \
    if (true && cond)                           \

#else

#define KLogBlockDebug()                         \
    if (false)                                  \

#define KLogBlockInfo()                          \
    if (false)                                  \

#define KLogBlockNotice()                        \
    if (false)                                  \

#define KLogBlockWarning()                       \
    if (false)

#define KLogBlockDebugIf(cond)                  \
    if (false && cond)                           \

#define KLogBlockInfoIf(cond)                   \
    if (false && cond)                           \

#define KLogBlockNoticeIf(cond)                 \
    if (false && cond)                           \

#define KLogBlockWarningIf(cond)                \
    if (false && cond)                           \

#endif

#endif