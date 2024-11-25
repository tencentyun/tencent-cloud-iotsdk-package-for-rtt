#include "tc_iot_device_config.h"
#include "tc_iot_export.h"
#include "rtconfig.h"


//void parse_command(tc_iot_coap_client_config * config, int argc, char ** argv) ;

#define COAP_THREAD_STACK_SIZE   8096

void _coap_basic_con_default_handler(void * client, tc_iot_coap_message * message ) {
    unsigned char * payload = NULL;
    int payload_len;
    unsigned short message_code = 0;

    message_code = tc_iot_coap_get_message_code(message);

    tc_iot_coap_get_message_payload(message, &payload_len, &payload);
    if (payload == NULL) {
        payload = (unsigned char *)"";
    }

    TC_IOT_LOG_TRACE("response coap code=%s,payload_len=%d,message=%s",
            tc_iot_coap_get_message_code_str(message_code),
            payload_len,
            payload
            );
}

void tc_coap_basic_thread(void)
{
    tc_iot_coap_client coap_client;
    tc_iot_coap_client_config coap_config = {
        {
            /* device info*/
            TC_IOT_CONFIG_DEVICE_SECRET, TC_IOT_CONFIG_DEVICE_PRODUCT_ID,
            TC_IOT_CONFIG_DEVICE_NAME, TC_IOT_CONFIG_DEVICE_CLIENT_ID,
        },
        TC_IOT_CONFIG_COAP_SERVER_HOST,
        TC_IOT_CONFIG_COAP_SERVER_PORT,
        _coap_basic_con_default_handler,
        TC_IOT_CONFIG_DTLS_HANDSHAKE_TIMEOUT_MS,
        TC_IOT_CONFIG_USE_DTLS,
#if defined(ENABLE_DTLS)
        TC_IOT_COAP_DTLS_PSK,
        sizeof(TC_IOT_COAP_DTLS_PSK)-1,
        TC_IOT_COAP_DTLS_PSK_ID,
        sizeof(TC_IOT_COAP_DTLS_PSK_ID) -1,
        NULL,
        NULL,
        NULL,
#endif
    };

    int ret = 0;
    int i = 0;
    char pub_topic_query_param[128];
    char buffer[1024];
    int temperature = 35;
    int step = 1;



    /* 解析命令行参数 */
    //parse_command(&coap_config, argc, argv);

    tc_iot_hal_printf("CoAP Server: %s:%d\n", coap_config.host,coap_config.port);
    tc_iot_coap_construct(&coap_client, &coap_config);
    ret = tc_iot_coap_auth(&coap_client);
    if (ret != TC_IOT_SUCCESS) {
        tc_iot_hal_printf("CoAP auth failed, ret=%d.\n", ret);
        return;
    }

    tc_iot_hal_snprintf(pub_topic_query_param, sizeof(pub_topic_query_param), TC_IOT_COAP_PUB_TOPIC_PARM_FMT,
            coap_config.device_info.product_id, 
            coap_config.device_info.device_name);

    while (1) 
	{
        if (temperature >= 40 ) {
            step = -1;
        } else if (temperature <= -10) {
            step = 1;
        }
        temperature += step;

        tc_iot_hal_snprintf(buffer,sizeof(buffer), "{\"temperature\":%d}", temperature);

        TC_IOT_LOG_TRACE("[c->s] %s", buffer);

        // 基于 CoAP 协议上报数据
        tc_iot_coap_publish(&coap_client, TC_IOT_COAP_SERVICE_PUBLISH_PATH, pub_topic_query_param, buffer, NULL);
        tc_iot_hal_printf("Publish yielding ...\n");
        tc_iot_coap_yield(&coap_client, TC_IOT_COAP_MESSAGE_ACK_TIMEOUT_MS);

        for (i = 20; i > 0; i--) {
            tc_iot_hal_printf("%d ...", i);
            tc_iot_hal_sleep_ms(1000);
        }
        tc_iot_hal_printf("\n");
    }

    return;
}


int tc_coap_basic_example(void)
{
    rt_err_t result;
    rt_thread_t tid;
    int stack_size = COAP_THREAD_STACK_SIZE;
    int priority = 20;
    char *stack;

    static int is_coap_basic_started = 0;
	
    if (is_coap_basic_started)
    {
        tc_iot_hal_printf("tc_coap_basic_example has already started!");
        return 0;
    }  

    tid = rt_malloc(RT_ALIGN(sizeof(struct rt_thread), 8) + stack_size);
    if (!tid)
    {
        tc_iot_hal_printf("no memory for thread: tc_coap_basic_example");
        return -1;
    }

	
	tc_iot_hal_printf("tc_coap_basic_example,TC_IOT_SDK_VERSION=%s", TC_IOT_SDK_VERSION);
    stack = (char *)tid + RT_ALIGN(sizeof(struct rt_thread), 8);
    result = rt_thread_init(tid,
                            "coap_basic_example",
                            tc_coap_basic_thread, NULL, // fun, parameter
                            stack, stack_size,        // stack, size
                            priority, 2               //priority, tick
                           );

    if (result == RT_EOK)
    {
    	is_coap_basic_started = 1;
        rt_thread_startup(tid);
    }

    return 0;
}


#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(tc_coap_basic_example, startup tc coap example);
#endif

#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(tc_coap_basic_example, startup tc coap example);
#endif


