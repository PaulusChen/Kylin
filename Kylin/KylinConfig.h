
#ifndef __KYLIN_CONFIG_H__
#define __KYLIN_CONFIG_H__

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


//configure
#define MAX_URL_LEN GetKB(100)

//#define KLOG_DEFAULT_IMPLEMENT 1

#endif