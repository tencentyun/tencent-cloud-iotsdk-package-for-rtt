#ifdef __cplusplus
extern "C" {
#endif

#include "tc_iot_inc.h"
#include "rtconfig.h"

unsigned long tc_iot_hal_get_ms()
{
#if (RT_TICK_PER_SECOND == 1000)
			/* #define RT_TICK_PER_SECOND 1000 */
			return (unsigned long)rt_tick_get();
#else
			unsigned long tick = 0;
					
			tick = rt_tick_get();
			tick = tick * 1000;
			return (unsigned long)((tick + RT_TICK_PER_SECOND - 1)/RT_TICK_PER_SECOND);
#endif

}

void tc_iot_hal_timer_init(tc_iot_timer* timer) {

     timer->ts = tc_iot_hal_get_ms();
}

uint32_t rtc1_get_sys_tick(void)
{
	return rt_tick_get();
}

char tc_iot_hal_timer_is_expired(tc_iot_timer* timer) {

    time_int_type now_ts = tc_iot_hal_get_ms();

	if (timer->ts <= now_ts)
	{
		return 1;
	}
    return 0;
}

void tc_iot_hal_timer_countdown_ms(tc_iot_timer* timer, unsigned int timeout) {
	timer->ts = tc_iot_hal_get_ms();
    timer->ts += timeout;
}

void tc_iot_hal_timer_countdown_second(tc_iot_timer* timer,
                                       unsigned int timeout) {
    tc_iot_hal_timer_countdown_ms(timer, timeout*1000);
}

time_int_type tc_iot_hal_timer_left_ms(tc_iot_timer* timer) {
    
    return (time_int_type)(timer->ts - tc_iot_hal_get_ms()); 
}



#ifdef __cplusplus
}
#endif
