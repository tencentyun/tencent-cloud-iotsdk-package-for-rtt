#include "tc_iot_device_config.h"
#include "tc_iot_device_logic.h"
#include "tc_iot_export.h"

/* �豸��ǰ״̬���� */
tc_iot_shadow_local_data g_tc_iot_coap_device_local_data = {
        false,
    TC_IOT_PROP_param_enum_enum_a,
    0,
    {'\0'},
};

// ����ģ���ֶΣ���������󳤶�
#define MAX_TEMPLATE_KEY_LEN   20
// ����ģ���ֶ�ȡֵ��󳤶�
#define MAX_VAL_BUFFER_LEN     50
#define TC_IOT_COAP_MAX_FIELD_LEN 22
#define COAP_THREAD_STACK_SIZE  8096


void parse_command(tc_iot_coap_client_config * config, int argc, char ** argv);
static void _coap_con_get_rpc_handler(tc_iot_coap_client * client, tc_iot_coap_con_status_e ack_status, 
                                      tc_iot_coap_message * message , void * session_context);
static void _coap_con_rpc_handler(tc_iot_coap_client * client, tc_iot_coap_con_status_e ack_status, 
                                  tc_iot_coap_message * message , void * session_context);
void _coap_con_default_handler(void * client, tc_iot_coap_message * message );

tc_iot_coap_client g_coap_client;
tc_iot_coap_client_config g_coap_config = {
    {
        /* device info*/
        TC_IOT_CONFIG_DEVICE_SECRET, TC_IOT_CONFIG_DEVICE_PRODUCT_ID,
        TC_IOT_CONFIG_DEVICE_NAME, TC_IOT_CONFIG_DEVICE_CLIENT_ID,
    },
    TC_IOT_CONFIG_COAP_SERVER_HOST,
    TC_IOT_CONFIG_COAP_SERVER_PORT,
    _coap_con_default_handler,
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
char rpc_pub_topic_query_param[128];
char rpc_sub_topic_query_param[128];

static void step_log(const char * step_desc, bool begin) {
    if (begin) {
        TC_IOT_LOG_TRACE("\n-----%s--------%s---------------\n","begin", step_desc);
    } else {
        TC_IOT_LOG_TRACE("-----%s--------%s---------------\n"," end ", step_desc);
    }
}

int do_coap_get(tc_iot_coap_client * p_coap_client, bool metadata, bool reported) {
    int ret = 0;
    char request[1024];
    tc_iot_json_writer writer;
    tc_iot_json_writer * w = &writer;

    tc_iot_json_writer_open(w, request, sizeof(request));
    tc_iot_json_writer_string(w ,"method", "get");
    tc_iot_json_writer_bool(w ,"metadata", metadata);
    tc_iot_json_writer_bool(w ,"reported", reported);
    ret = tc_iot_json_writer_close(w);

    if (ret <= 0) {
        TC_IOT_LOG_INFO("encode json failed ,ret=%d", ret);
    } else {
        tc_iot_hal_printf("[c->s]:%s\n", request);
    }

    // ���� CoAP Э��� RPC ����
    ret = tc_iot_coap_rpc(p_coap_client, TC_IOT_COAP_SERVICE_RPC_PATH, rpc_pub_topic_query_param, 
                    rpc_sub_topic_query_param, request, _coap_con_get_rpc_handler);
    if (ret < 0) {
        TC_IOT_LOG_ERROR("request failed ,ret=%d", ret);
    }
    ret = tc_iot_coap_yield(p_coap_client, TC_IOT_COAP_MESSAGE_ACK_TIMEOUT_MS);
    if (ret < 0) {
        TC_IOT_LOG_ERROR("coap yield failed ,ret=%d", ret);
    }
    return ret;
}

int do_coap_update(tc_iot_coap_client * p_coap_client, 
                  unsigned char * report_bits, tc_iot_shadow_local_data * p_local_data) {

    int ret = 0;
    char request[1024];
    tc_iot_json_writer writer;
    tc_iot_json_writer * w = &writer;

    tc_iot_json_writer_open(w, request, sizeof(request));
    tc_iot_json_writer_string(w ,"method", "update");
    tc_iot_json_writer_object_begin(w ,"state");
    tc_iot_json_writer_object_begin(w ,"reported");

    if (TC_IOT_BIT_GET(report_bits, TC_IOT_PROP_param_bool)) {
        tc_iot_json_writer_bool(w ,"param_bool", p_local_data->param_bool);
    }

    if (TC_IOT_BIT_GET(report_bits, TC_IOT_PROP_param_enum)) {
        tc_iot_json_writer_int(w ,"param_enum", p_local_data->param_enum);
    }

    if (TC_IOT_BIT_GET(report_bits, TC_IOT_PROP_param_number)) {
        tc_iot_json_writer_decimal(w ,"param_number", p_local_data->param_number);
    }

    if (TC_IOT_BIT_GET(report_bits, TC_IOT_PROP_param_string)) {
        tc_iot_json_writer_string(w ,"param_string", p_local_data->param_string);
    }

    tc_iot_json_writer_object_end(w);
    tc_iot_json_writer_object_end(w);
    ret = tc_iot_json_writer_close(w);

    if (ret <= 0) {
        TC_IOT_LOG_INFO("encode json failed ,ret=%d", ret);
    } else {
        tc_iot_hal_printf("[c->s]:%s\n", request);
    }

    // ���� CoAP Э��� RPC ����
    ret = tc_iot_coap_rpc(p_coap_client, TC_IOT_COAP_SERVICE_RPC_PATH, rpc_pub_topic_query_param, 
                          rpc_sub_topic_query_param, request, _coap_con_rpc_handler);
    if (ret < 0) {
        TC_IOT_LOG_ERROR("request failed ,ret=%d", ret);
    }

    ret = tc_iot_coap_yield(p_coap_client, TC_IOT_COAP_MESSAGE_ACK_TIMEOUT_MS);
    if (ret < 0) {
        TC_IOT_LOG_ERROR("coap yield failed ,ret=%d", ret);
    }
    return ret;
}

int do_coap_delete(tc_iot_coap_client * p_coap_client, 
                  unsigned char * desired_bits) {

    int ret = 0;
    char request[1024];
    tc_iot_json_writer writer;
    tc_iot_json_writer * w = &writer;

    tc_iot_json_writer_open(w, request, sizeof(request));
    tc_iot_json_writer_string(w ,"method", "delete");
    tc_iot_json_writer_object_begin(w ,"state");
    tc_iot_json_writer_object_begin(w ,"desired");

    if (TC_IOT_BIT_GET(desired_bits, TC_IOT_PROP_param_bool)) {
        tc_iot_json_writer_null(w ,"param_bool");
    }

    if (TC_IOT_BIT_GET(desired_bits, TC_IOT_PROP_param_enum)) {
        tc_iot_json_writer_null(w ,"param_enum");
    }

    if (TC_IOT_BIT_GET(desired_bits, TC_IOT_PROP_param_number)) {
        tc_iot_json_writer_null(w ,"param_number");
    }

    if (TC_IOT_BIT_GET(desired_bits, TC_IOT_PROP_param_string)) {
        tc_iot_json_writer_null(w ,"param_string");
    }

    tc_iot_json_writer_object_end(w);
    tc_iot_json_writer_object_end(w);
    ret = tc_iot_json_writer_close(w);

    if (ret <= 0) {
        TC_IOT_LOG_INFO("encode json failed ,ret=%d", ret);
    } else {
        tc_iot_hal_printf("[c->s]:%s\n", request);
    }

    // ���� CoAP Э��� RPC ����
    ret = tc_iot_coap_rpc(p_coap_client, TC_IOT_COAP_SERVICE_RPC_PATH, rpc_pub_topic_query_param, 
                          rpc_sub_topic_query_param, request, _coap_con_rpc_handler);
    if (ret < 0) {
        TC_IOT_LOG_ERROR("request failed ,ret=%d", ret);
    }

    ret = tc_iot_coap_yield(p_coap_client, TC_IOT_COAP_MESSAGE_ACK_TIMEOUT_MS);
    if (ret < 0) {
        TC_IOT_LOG_ERROR("coap yield failed ,ret=%d", ret);
    }
    return ret;
}

int coap_process_desired( const char * doc_start, jsmntok_t * json_token, int tok_count, 
                      unsigned char * p_desired_bits, tc_iot_shadow_local_data * p_local_data) {
    int i;
    jsmntok_t  * key_tok = NULL;
    jsmntok_t  * val_tok = NULL;
    char val_buf[MAX_VAL_BUFFER_LEN+1];
    char key_buf[MAX_TEMPLATE_KEY_LEN+1];
    int  key_len = 0, val_len = 0;
    const char * key_start;
    const char * val_start;

    if (!doc_start) {
        TC_IOT_LOG_ERROR("doc_start is null");
        return TC_IOT_NULL_POINTER;
    }

    if (!json_token) {
        TC_IOT_LOG_ERROR("json_token is null");
        return TC_IOT_NULL_POINTER;
    }

    if (!tok_count) {
        TC_IOT_LOG_ERROR("tok_count is invalid");
        return TC_IOT_INVALID_PARAMETER;
    }

    memset(val_buf, 0, sizeof(val_buf));
    memset(key_buf, 0, sizeof(key_buf));

    for (i = 0; i < tok_count/2; i++) {
        /* λ�� 0 ��object��������Ҫ��λ�� 1 ��ʼȡ����*/
        /* 2*i+1 Ϊ key �ֶΣ�2*i + 2 Ϊ value �ֶ�*/
        key_tok = &(json_token[2*i + 1]);
        key_start = doc_start + key_tok->start;
        key_len = key_tok->end - key_tok->start;
        if (key_len > MAX_TEMPLATE_KEY_LEN) {
            memcpy(key_buf, key_start, MAX_TEMPLATE_KEY_LEN);
            key_buf[MAX_TEMPLATE_KEY_LEN] = '\0';
            TC_IOT_LOG_ERROR("key=%s... length oversize.", key_buf);
            continue;
        }

        memcpy(key_buf, key_start, key_len);
        key_buf[key_len] = '\0';

        val_tok = &(json_token[2*i + 2]);
        val_start = doc_start + val_tok->start;
        val_len = val_tok->end - val_tok->start;
        if (val_len > TC_IOT_COAP_MAX_FIELD_LEN) {
            memcpy(val_buf, val_start, MAX_VAL_BUFFER_LEN);
            val_buf[MAX_VAL_BUFFER_LEN] = '\0';
            TC_IOT_LOG_ERROR("val=%s... length oversize.", val_buf);
            continue;
        }

        memcpy(val_buf, val_start, val_len);
        val_buf[val_len] = '\0';
        

        if (strcmp("param_bool", key_buf) == 0 ) {
            TC_IOT_BIT_SET(p_desired_bits, TC_IOT_PROP_param_bool);
            TC_IOT_LOG_TRACE("desired field: %s=%s->%s", key_buf, p_local_data->param_bool?"true":"false", val_buf);
            p_local_data->param_bool = (0 == strcmp(val_buf, "true"));
            continue;
        }

        if (strcmp("param_enum", key_buf) == 0 ) {
            TC_IOT_BIT_SET(p_desired_bits, TC_IOT_PROP_param_enum);
            TC_IOT_LOG_TRACE("desired field: %s=%d->%s", key_buf, p_local_data->param_enum, val_buf);
            p_local_data->param_enum = atoi(val_buf);
            continue;
        }

        if (strcmp("param_number", key_buf) == 0 ) {
            TC_IOT_BIT_SET(p_desired_bits, TC_IOT_PROP_param_number);
            TC_IOT_LOG_TRACE("desired field: %s=%f->%s", key_buf, p_local_data->param_number, val_buf);
            p_local_data->param_number = atof(val_buf);
            continue;
        } 

        if (strcmp("param_string", key_buf) == 0 ) {
            TC_IOT_BIT_SET(p_desired_bits, TC_IOT_PROP_param_string);
            TC_IOT_LOG_TRACE("desired field: %s=%s->%s", key_buf, p_local_data->param_string, val_buf);
            strcpy(p_local_data->param_string, val_buf);
            continue;
        } 


        TC_IOT_LOG_ERROR("unknown desired field: %s=%s", key_buf, val_buf);
        continue;
    }
    return TC_IOT_SUCCESS;
}

int coap_check_and_process_desired(unsigned char * p_desired_bits, tc_iot_shadow_local_data * p_local_data, const char * message) {
    const char * desired_start = NULL;
    int desired_len = 0;
    jsmntok_t  json_token[TC_IOT_MAX_JSON_TOKEN_COUNT];
    int field_index = 0;
    int ret = 0;
    int token_count = 0;

    /* ��Ч�Լ�� */
    ret = tc_iot_json_parse(message, strlen(message), json_token, TC_IOT_ARRAY_LENGTH(json_token));
    if (ret <= 0) {
        TC_IOT_LOG_ERROR("tc_iot_json_parse ret=%d", ret);
        return ret;
    }

    token_count = ret;
    tc_iot_mem_usage_log("json_token[TC_IOT_MAX_JSON_TOKEN_COUNT]", sizeof(json_token), sizeof(json_token[0])*token_count);

    /* ��� desired �ֶ��Ƿ���� */
    field_index = tc_iot_json_find_token(message, json_token, token_count, "payload.state.desired", NULL, 0);
    if (field_index <= 0 ) {
        TC_IOT_LOG_TRACE("payload.state.desired not found, ret=%d", field_index);
    } else {
        desired_start = message + json_token[field_index].start;
        desired_len = json_token[field_index].end - json_token[field_index].start;
        TC_IOT_LOG_TRACE("payload.state.desired found:%s", tc_iot_log_summary_string(desired_start, desired_len));
        /* ���ݿ���̨���� APP �˵�ָ��趨�豸״̬ */
        if (desired_start) {
            ret = tc_iot_json_parse(desired_start, desired_len, json_token, token_count);
            if (ret <= 0) {
                return TC_IOT_FAILURE;
            }
            coap_process_desired( desired_start, json_token, ret, p_desired_bits, p_local_data);
        }
    }

    return TC_IOT_SUCCESS;
}


void _coap_con_default_handler(void * client, tc_iot_coap_message * message ) {
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


static void _coap_con_get_rpc_handler(tc_iot_coap_client * client, tc_iot_coap_con_status_e ack_status, 
                                  tc_iot_coap_message * message , void * session_context) {
    unsigned char * payload = NULL;
    int payload_len;
    unsigned short message_code = 0;
    unsigned char desired_bits[1];
    int ret;

    if (ack_status == TC_IOT_COAP_CON_TIMEOUT) {
        TC_IOT_LOG_ERROR("message timeout");
        return;
    }
    message_code = tc_iot_coap_get_message_code(message);

    tc_iot_coap_get_message_payload(message, &payload_len, &payload);
    if (message_code != COAP_CODE_201_CREATED) {
        if (payload == NULL) {
            payload = (unsigned char *)"";
        }
        TC_IOT_LOG_ERROR("publish failed, response coap code=%s,message=%s",
                         tc_iot_coap_get_message_code_str(message_code),
                         payload
                         );
        return ;
    }

    if (message && payload) {
        tc_iot_hal_printf("[s->c]:%s\n", payload);

        memset(desired_bits, 0, sizeof(desired_bits));
        ret = coap_check_and_process_desired(desired_bits, &g_tc_iot_coap_device_local_data, (char *)payload);
        if (desired_bits[0]) {
            step_log("rpc update: report latest device state", true);
            ret = do_coap_update( &g_coap_client, desired_bits, &g_tc_iot_coap_device_local_data);
            if (ret < 0) {
                return ;
            }
            step_log("rpc update: report latest device state", false);

            step_log("rpc delete: clear desired data after successfully processed.", true);
            ret = do_coap_delete( &g_coap_client, desired_bits);
            if (ret < 0) {
                return ;
            }
            step_log("rpc delete: clear desired data after successfully processed.", false);

            step_log("rpc get: fetch latest state", true);
            ret = do_coap_get( &g_coap_client, false, true);
            if (ret < 0) {
                return ;
            }
            step_log("rpc get: fetch latest state", false);

            TC_IOT_LOG_TRACE("Desired data process finished");

        } else {
            TC_IOT_LOG_TRACE("No desired data since last check.");
        }
    } else {
        TC_IOT_LOG_TRACE("[no payload]");
    }
}

static void _coap_con_rpc_handler(tc_iot_coap_client * client, tc_iot_coap_con_status_e ack_status, 
                                  tc_iot_coap_message * message , void * session_context) {
    unsigned char * payload = NULL;
    int payload_len;
    unsigned short message_code = 0;

    if (ack_status == TC_IOT_COAP_CON_TIMEOUT) {
        TC_IOT_LOG_ERROR("message timeout");
        return;
    }
    message_code = tc_iot_coap_get_message_code(message);

    tc_iot_coap_get_message_payload(message, &payload_len, &payload);
    if (message_code != COAP_CODE_201_CREATED) {
        if (payload == NULL) {
            payload = (unsigned char *)"";
        }
        TC_IOT_LOG_ERROR("publish failed, response coap code=%s,message=%s",
                         tc_iot_coap_get_message_code_str(message_code),
                         payload
                         );
        return ;
    }

    if (message && payload) {
        tc_iot_hal_printf("[s->c]:%s\n", payload);
    } else {
        TC_IOT_LOG_TRACE("[no payload]");
    }
}


void tc_coap_advance_thread(void)
{
    int ret = 0;
    int i = 0;
    /* char pub_topic_query_param[128]; */
    tc_iot_coap_client * p_coap_client = &g_coap_client;
    tc_iot_coap_client_config * p_coap_config = &g_coap_config;


    /* ���������в��� */
    //parse_command(p_coap_config, argc, argv);

    tc_iot_hal_snprintf(rpc_pub_topic_query_param, sizeof(rpc_pub_topic_query_param), TC_IOT_COAP_RPC_PUB_TOPIC_PARM_FMT,
                        p_coap_config->device_info.product_id, 
                        p_coap_config->device_info.device_name);
    tc_iot_hal_snprintf(rpc_sub_topic_query_param, sizeof(rpc_sub_topic_query_param), TC_IOT_COAP_RPC_SUB_TOPIC_PARM_FMT,
                        p_coap_config->device_info.product_id, 
                        p_coap_config->device_info.device_name);

    tc_iot_hal_printf("CoAP Server: %s:%d\n", p_coap_config->host,p_coap_config->port);

    tc_iot_coap_construct(p_coap_client, p_coap_config);
    ret = tc_iot_coap_auth(p_coap_client);
    if (ret != TC_IOT_SUCCESS) {
        tc_iot_hal_printf("CoAP auth failed, ret=%d.\n", ret);
        return;
    }

    while (1) {
        ret = do_coap_get(p_coap_client, false, false);
        for (i = 20; i > 0; i--) {
            tc_iot_hal_printf("%d...", i);
            tc_iot_hal_sleep_ms(1000);
        }
        tc_iot_hal_printf("\n");
    }

    tc_iot_coap_destroy(p_coap_client);


    return;
}


int tc_coap_advance_example(void)
{
    rt_err_t result;
    rt_thread_t tid;
    int stack_size = COAP_THREAD_STACK_SIZE;
    int priority = 20;
    char *stack;

    static int is_coap_started = 0;
	
    if (is_coap_started)
    {
        tc_iot_hal_printf("tc_coap_advance_example has already started!");
        return 0;
    }  

    tid = rt_malloc(RT_ALIGN(sizeof(struct rt_thread), 8) + stack_size);
    if (!tid)
    {
        tc_iot_hal_printf("no memory for thread: tc_coap_advance_example");
        return -1;
    }

	
	tc_iot_hal_printf("tc_coap_advance_example,TC_IOT_SDK_VERSION=%s", TC_IOT_SDK_VERSION);
    stack = (char *)tid + RT_ALIGN(sizeof(struct rt_thread), 8);
    result = rt_thread_init(tid,
                            "coap_advance_example",
                            tc_coap_advance_thread, NULL, // fun, parameter
                            stack, stack_size,        // stack, size
                            priority, 2               //priority, tick
                           );

    if (result == RT_EOK)
    {
    	is_coap_started = 1;
        rt_thread_startup(tid);
    }

    return 0;
}


#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(tc_coap_advance_example, startup tc coap example);
#endif

#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(tc_coap_advance_example, startup tc coap example);
#endif


