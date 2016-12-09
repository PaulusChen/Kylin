
#ifndef __KLOG_H__
#define __KLOG_H__
#include <syslog.h>

#include "KylinTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void KLogInit(const char *progName);

extern void KLog(bool pe,uint32_t type,const char *format,...);

extern void KKLogClose();

#ifdef __cplusplus
} //extern "C"
#endif


#define KLogDebug(fmt,args...)                   \
    KLog(false,LOG_DEBUG,fmt,##args)

#define KLogInfo(fmt,args...)                    \
    KLog(false,LOG_INFO,fmt,##args)

#define KLogNotice(fmt,args...)                  \
    KLog(false,LOG_NOTICE,fmt,##args)

#define KLogWarning(fmt,args...)                 \
    KLog(false,LOG_WARNING,fmt,##args)

#define KLogErr(fmt,args...)                     \
    KLog(false,LOG_ERR,fmt,##args)

#define KLogDebugErrno(fmt,args...)              \
    KLog(true,LOG_DEBUG,fmt,##args)

#define KLogInfoErrno(fmt,args...)               \
    KLog(true,LOG_INFO,fmt,##args)

#define KLogNoticeErrno(fmt,args...)             \
    KLog(true,LOG_NOTICE,fmt,##args)

#define KLogWarningErrno(fmt,args...)            \
    KLog(true,LOG_WARNING,fmt,##args)

#define KLogErrErrno(fmt,args...)                \
    KLog(true,LOG_ERR,fmt,##args)


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

#endif