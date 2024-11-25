#include "tc_iot_device_config.h"
#include "tc_iot_device_logic.h"
#include "tc_iot_export.h"
#include <rtthread.h>

#define MQTT_YEILD_TIME					1000
#define MQTT_SHADOW_THREAD_STACK_SIZE 	10240

void parse_command(tc_iot_mqtt_client_config * config, int argc, char ** argv) ;
void get_message_ack_callback(tc_iot_command_ack_status_e ack_status, tc_iot_message_data * md , void * session_context);
extern tc_iot_shadow_config g_tc_iot_shadow_config;
extern tc_iot_shadow_local_data g_tc_iot_device_local_data;

/* ѭ���˳���ʶ */
volatile int stop = 0;
void sig_handler(int sig) {
    if (sig == SIGINT) {
        tc_iot_hal_printf("SIGINT received, going down.\n");
        stop ++;
    } else if (sig == SIGTERM) {
        tc_iot_hal_printf("SIGTERM received, going down.\n");
        stop ++;
    } else {
        tc_iot_hal_printf("signal received:%d\n", sig);
    }
    if (stop >= 3) {
        tc_iot_hal_printf("SIGINT/SIGTERM received over %d times, force shutdown now.\n", stop);
        exit(0);
    }
}


/**
 * @brief operate_device �����豸���ƿ���
 *
 * @param p_device_data �豸״̬����
 */
void operate_device(tc_iot_shadow_local_data * p_device_data) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    tc_iot_hal_printf( "%04d-%02d-%02d %02d:%02d:%02d do something for data change.\n" ,
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}


/**
 * @brief ��������ʾ�����豸��״̬�����仯ʱ����θ����豸�����ݣ����ϱ�������ˡ�
 */
void do_sim_data_change(void) {
    TC_IOT_LOG_TRACE("simulate data change.");
    int i = 0;

    g_tc_iot_device_local_data.param_bool = !g_tc_iot_device_local_data.param_bool;

    g_tc_iot_device_local_data.param_enum += 1;
    g_tc_iot_device_local_data.param_enum %= 3;

    g_tc_iot_device_local_data.param_number += 1;
    g_tc_iot_device_local_data.param_number = g_tc_iot_device_local_data.param_number > 4095?0:g_tc_iot_device_local_data.param_number;

    for (i = 0; i < 0+1;i++) {
        g_tc_iot_device_local_data.param_string[i] += 1;
        g_tc_iot_device_local_data.param_string[i] = g_tc_iot_device_local_data.param_string[0] > 'Z'?'A':g_tc_iot_device_local_data.param_string[0];
        g_tc_iot_device_local_data.param_string[i] = g_tc_iot_device_local_data.param_string[0] < 'A'?'A':g_tc_iot_device_local_data.param_string[0];
    }
    g_tc_iot_device_local_data.param_string[0+2] = 0;


    /* �ϱ���������״̬ */
    tc_iot_report_device_data(tc_iot_get_shadow_client());
}


void mqtt_shadow_thread(void) {
    tc_iot_mqtt_client_config * p_client_config;
    bool use_static_token;
    int ret;
    long timestamp = tc_iot_hal_timestamp(NULL);
    tc_iot_hal_srandom(timestamp);
    long nonce = tc_iot_hal_random();

	

//    signal(SIGINT, sig_handler);
//    signal(SIGTERM, sig_handler);
//    setbuf(stdout, NULL);

    p_client_config = &(g_tc_iot_shadow_config.mqtt_client_config);

    /* ���������в��� */
//    parse_command(p_client_config, argc, argv);

    /* ���� product id ��device name ���壬���ɷ����Ͷ��ĵ� Topic ���ơ� */
    snprintf(g_tc_iot_shadow_config.sub_topic,TC_IOT_MAX_MQTT_TOPIC_LEN, TC_IOT_SHADOW_SUB_TOPIC_FMT,
            p_client_config->device_info.product_id,p_client_config->device_info.device_name);
    snprintf(g_tc_iot_shadow_config.pub_topic,TC_IOT_MAX_MQTT_TOPIC_LEN, TC_IOT_SHADOW_PUB_TOPIC_FMT,
            p_client_config->device_info.product_id,p_client_config->device_info.device_name);

    /* �ж��Ƿ���Ҫ��ȡ��̬ token */
    use_static_token = strlen(p_client_config->device_info.username) && strlen(p_client_config->device_info.password);

    if (!use_static_token) {
        /* ��ȡ��̬ token */
        tc_iot_hal_printf("requesting username and password for mqtt.\n");
        ret = TC_IOT_AUTH_FUNC( timestamp, nonce, &p_client_config->device_info, TC_IOT_TOKEN_MAX_EXPIRE_SECOND);
        if (ret != TC_IOT_SUCCESS) {
            tc_iot_hal_printf("refresh token failed, trouble shooting guide: " "%s#%d\n", TC_IOT_TROUBLE_SHOOTING_URL, ret);
            return;
        }
        tc_iot_hal_printf("request username and password for mqtt success.\n");
    } else {
        tc_iot_hal_printf("username & password using: %s %s\n", p_client_config->device_info.username, p_client_config->device_info.password);
    }

    ret = tc_iot_server_init(tc_iot_get_shadow_client(), &g_tc_iot_shadow_config);
    if (ret != TC_IOT_SUCCESS) {
        tc_iot_hal_printf("tc_iot_server_init failed, trouble shooting guide: " "%s#%d\n", TC_IOT_TROUBLE_SHOOTING_URL, ret);
        return;
    }

    while (!stop) {
        tc_iot_server_loop(tc_iot_get_shadow_client(), MQTT_YEILD_TIME);
		do_sim_data_change();
    }

    tc_iot_server_destroy(tc_iot_get_shadow_client());
    return;
}

int tc_mqtt_shadow_example(void)
{
    rt_err_t result;
    rt_thread_t tid;
    int stack_size = MQTT_SHADOW_THREAD_STACK_SIZE;
    int priority = 20;
    char *stack;

    static int is_shadow_started = 0;
    if (is_shadow_started)
    {
        tc_iot_hal_printf("mqtt_shadow_example has already started!");
        return 0;
    }    

    tid = rt_malloc(RT_ALIGN(sizeof(struct rt_thread), 8) + stack_size);
    if (!tid)
    {
        tc_iot_hal_printf("no memory for thread: mqtt_shadow_example");
        return -1;
    }

	tc_iot_hal_printf("TC MQTT Shadow Exmaple,TC_IOT_SDK_VER:%s", TC_IOT_SDK_VERSION);
    stack = (char *)tid + RT_ALIGN(sizeof(struct rt_thread), 8);
    result = rt_thread_init(tid,
                            "mqtt_shadow",
                            (void *)mqtt_shadow_thread, NULL, // fun, parameter
                            stack, stack_size,        // stack, size
                            priority, 2               //priority, tick
                           );

    if (result == RT_EOK)
    {
        rt_thread_startup(tid);
        is_shadow_started = 1;
    }

    return 0;
}



#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(tc_mqtt_shadow_example, startup mqtt shadow example);
#endif

#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(tc_mqtt_shadow_example, startup mqtt shadow example);
#endif



