#ifndef _KYLINTYPES_H_
#define _KYLINTYPES_H_

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#define _in 

#define _out 

#ifndef __cplusplus
#define bool uint8_t

#define true 1

#define false 0
#endif

#define KILO_BYTE_FACTOR (1024)
#define MEGA_BYTE_FACTOR (1024 * KILO_BYTE_FACTOR)
#define GIGA_BYTE_FACTOR (1024LL * MEGA_BYTE_FACTOR)
#define TRILLION_BYTE_FACTOR (1024LL * GIGA_BYTE_FACTOR)
#define GetKB(val) ((val) * KILO_BYTE_FACTOR)
#define GetMB(val) ((val) * MEGA_BYTE_FACTOR)
#define GetGB(val) ((val) * GIGA_BYTE_FACTOR)
#define GetTB(val) ((val) * TRILLION_BYTE_FACTOR)
#define make_flag(pos) (1<<pos)

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
    KE_AGAIN = -15,
    KE_MAX_ERROR_CODE
} KErrorCode;

#endif /*  _ */