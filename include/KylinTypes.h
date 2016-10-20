#ifndef _KYLINTYPES_H_
#define _KYLINTYPES_H_

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

typedef enum {
    KE_OK = 0,
    KE_UNKNOW = -1,
    KE_NULL_PTR = -2,
    KE_OUT_OF_RANGE = -3,
    KE_CONTAINER_FULL = -4,
    KE_MAX_ERROR_CODE
} KErrorCode;

#endif /*  _ */