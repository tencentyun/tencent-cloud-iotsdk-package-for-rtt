#ifndef TC_IOT_PLATFORM_H
#define TC_IOT_PLATFORM_H

#include <ctype.h>
// #include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <rtthread.h>
#include "sal_ipaddr.h"


//extern void rt_kprintf(const char *fmt, ...);
//extern void *rt_malloc(rt_size_t size);
//extern void rt_free(void *rmem);
//extern rt_int32_t rt_vsnprintf(char *buf, rt_size_t  size, const char *fmt,va_list args);

//#define tc_iot_hal_snprintf snprintf


//#define tc_iot_hal_malloc rt_malloc
//#define tc_iot_hal_free rt_free
//#define tc_iot_hal_printf rt_kprintf
#define tc_iot_hal_snprintf snprintf

//#define htons(a) a
//#define htonl(a) a
//#define ntohs(a) a
//#define ntohl(a) a



typedef unsigned long  time_int_type;
struct _tc_iot_timer { time_int_type ts; };
typedef struct _tc_iot_timer tc_iot_timer;




#endif /* end of include guard */
