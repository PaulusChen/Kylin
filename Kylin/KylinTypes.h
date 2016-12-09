#ifndef _KYLINTYPES_H_
#define _KYLINTYPES_H_

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include "KylinConfig.h"

#define GetTmpName(type,name) type##_##name

typedef enum {
    KE_OK = 0,
    KE_SYS_ERR = -1,
    KE_UNKNOW = -2,
    KE_NULL_PTR = -3,
    KE_NO_ENTRY = -4,
    KE_OUT_OF_RANGE = -5,
    KE_CONTAINER_FULL = -6,
    KE_DB_ERROR = -7,
    KE_SYNTAX_ERROR = -8,
    KE_SYS_OBJ_CREATE_FAILED = -9,
    KE_SYS_OPT_FAILED = -10,
    KE_BAD_PARAM = -11,
    KE_3RD_PART_LIBS_ERROR = -12,
    KE_NO_MEMORY = -13,
    KE_TIMEOUT = -14,
    KE_RELEASE_EARLIER = 15,
    KE_MAX_ERROR_CODE
} KErrorCode;

#endif /*  _ */