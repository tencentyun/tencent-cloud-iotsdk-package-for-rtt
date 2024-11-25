#ifdef __cplusplus
extern "C" {
#endif

#include "tc_iot_inc.h"
#include "tc_iot_get_time.h"
#include <stdarg.h>
#include <rtthread.h>

#define HAL_OS_LOG_MAXLEN   512

void *tc_iot_hal_mutex_init(void)
{
    rt_mutex_t mutex= rt_mutex_create("tc_iot_mutex", RT_IPC_FLAG_FIFO);
    if (NULL == mutex) {
        return NULL;
    }

    return mutex;
}

void  tc_iot_hal_mutex_destroy(void *mutex)
{
    int err_num;
    err_num = err_num;
    if (0 != (err_num = rt_mutex_delete((rt_mutex_t)mutex))) 
	{
        perror("destroy mutex failed");
    }
}

void tc_iot_hal_mutex_lock(void *mutex)
{
    int err_num;
	  err_num = err_num;
    if (0 != (err_num = rt_mutex_take((rt_mutex_t)mutex, RT_WAITING_FOREVER)))
	{
        perror("lock mutex failed");
    }
}

void tc_iot_hal_mutext_unlock(void *mutex)
{
    int err_num;
    err_num = err_num;
    if (0 != (err_num = rt_mutex_release((rt_mutex_t)mutex))) 
	{
        perror("unlock mutex failed");
    }
}


void *tc_iot_hal_malloc(uint32_t size)
{
    return rt_malloc(size);
}

void tc_iot_hal_free(void *ptr)
{
    rt_free(ptr);
}


void tc_iot_hal_printf(const char *fmt, ...)
{
    va_list args;
	char log_buf[HAL_OS_LOG_MAXLEN];
    
    va_start(args, fmt);
    rt_vsnprintf(log_buf, HAL_OS_LOG_MAXLEN, fmt, args);
    va_end(args);
    rt_kprintf("%s", log_buf);
}

//rtthread not support float sprint 
//int tc_iot_hal_snprintf(char *str, size_t size, const char *format, ...)
//{
//    va_list args;
//    int     rc;

//    va_start(args, format);
//    rc = rt_vsnprintf(str, size, format, args);
//    va_end(args);

//    return rc;
//}


int  tc_iot_hal_get_value(const char* key , char* value , size_t len )
{
 	return 0;
}

int   tc_iot_hal_set_value(const char* key ,  const char* value )
{
	return 0;
}


//http://gz.auth-device-iot.tencentcloudapi.com/time
long tc_iot_hal_timestamp(void* zone) {
    return tc_iot_get_time_from_ntp();
}

int tc_iot_hal_sleep_ms(long ms) {

    //return rt_thread_mdelay(ms);

	return rt_thread_delay(rt_tick_from_millisecond(ms));
}

long tc_iot_hal_random() {
    return (unsigned int)rand();
}

void tc_iot_hal_srandom(unsigned int seed) {
    srand(seed);
}

#ifdef __cplusplus
}
#endif
