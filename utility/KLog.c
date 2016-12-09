
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include "KTools.h"
#include "KLog.h"

#ifdef KLOG_DEFAULT_IMPLEMENT

void KLogInit(const char *progName) {
    openlog(progName,LOG_PERROR|LOG_PID,LOG_DAEMON);
}

void KLog(bool pe,uint32_t type,const char *format,...) {
    char errBuf[GetKB(512)];
    va_list arg_ptr;
    va_start(arg_ptr,format);
    if (pe) {
        vsprintf(errBuf,format,arg_ptr);
        int err = errno;
        syslog(type,"%s Errno : [%d] %s",errBuf,err,strerror(err));
    } else {
        vsyslog(type,format,arg_ptr);
    }

    va_end(arg_ptr);
}

void KLogClose() {
    closelog();
}

#endif