
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include "KTools.h"
#include "KLog.h"

void KLogInit(const char *progName) {
    openlog(progName,LOG_PERROR|LOG_PID,LOG_DAEMON);
}

void KLog(bool pe,uint32_t type,const char *func,const char *format,...) {
    char errBuf[GetKB(512)];
    va_list arg_ptr;
    va_start(arg_ptr,format);
    vsprintf(errBuf,format,arg_ptr);

    if (pe) {
        int err = errno;
        syslog(type,"[%s]: %s Errno : [%d] %s",func,errBuf,err,strerror(err));
    } else {
        syslog(type,"[%s]: %s",func,errBuf);
    }

    va_end(arg_ptr);
}

void KLogClose() {
    closelog();
}

