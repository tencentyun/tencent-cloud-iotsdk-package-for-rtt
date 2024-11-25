#include "tc_iot_device_config.h"
#include "tc_iot_device_logic.h"
#include "tc_iot_sub_device_logic.h"
#include "tc_iot_export.h"

int _tc_iot_shadow_property_control_callback(tc_iot_event_message *msg, void * client,  void * context);
void operate_device(tc_iot_shadow_local_data * device);

/* 影子数据 Client  */
tc_iot_shadow_client g_tc_iot_shadow_client;

tc_iot_shadow_client * tc_iot_get_shadow_client(void) {
    return &g_tc_iot_shadow_client;
}

tc_iot_sub_device_info g_tc_iot_sub_devices[TC_IOT_GW_MAX_SUB_DEVICE_COUNT];
tc_iot_sub_device_table g_tc_iot_sub_device_table = {
    TC_IOT_GW_MAX_SUB_DEVICE_COUNT,
    0,
    &g_tc_iot_sub_devices[0],
};



/* 设备本地数据类型及地址、回调函数等相关定义 */
tc_iot_shadow_property_def g_tc_iot_shadow_property_defs[] = {
/*${ data_template.property_def_initializer() }*/};


/* 设备当前状态数据 */
tc_iot_shadow_local_data g_tc_iot_device_local_data = {
/*${ data_template.local_data_initializer() }*/};

/* 设备状态控制数据 */
static tc_iot_shadow_local_data g_tc_iot_device_desired_data = {
/*${ data_template.local_data_initializer() }*/};

/* 设备已上报状态数据 */
tc_iot_shadow_local_data g_tc_iot_device_reported_data = {
/*${ data_template.local_data_initializer() }*/};

/* 设备初始配置 */
tc_iot_shadow_config g_tc_iot_shadow_config = {
    {
        {
            /* device info*/
            TC_IOT_CONFIG_DEVICE_SECRET, TC_IOT_CONFIG_DEVICE_PRODUCT_ID,
            TC_IOT_CONFIG_DEVICE_NAME, TC_IOT_CONFIG_DEVICE_CLIENT_ID,
            TC_IOT_CONFIG_DEVICE_USER_NAME, TC_IOT_CONFIG_DEVICE_PASSWORD, 0,
            TC_IOT_CONFIG_AUTH_MODE, TC_IOT_CONFIG_REGION, TC_IOT_CONFIG_AUTH_API_URL,
        },
        TC_IOT_CONFIG_MQ_SERVER_HOST,
        TC_IOT_CONFIG_MQ_SERVER_PORT,
        TC_IOT_CONFIG_COMMAND_TIMEOUT_MS,
        TC_IOT_CONFIG_TLS_HANDSHAKE_TIMEOUT_MS,
        TC_IOT_CONFIG_KEEP_ALIVE_INTERVAL_SEC,
        TC_IOT_CONFIG_CLEAN_SESSION,
        TC_IOT_CONFIG_USE_TLS,
        TC_IOT_CONFIG_AUTO_RECONNECT,
        TC_IOT_CONFIG_ROOT_CA,
        TC_IOT_CONFIG_CLIENT_CRT,
        TC_IOT_CONFIG_CLIENT_KEY,
        NULL,
        NULL,
        0,  /* send will */
        {
            {'M', 'Q', 'T', 'W'}, 0, {NULL, {0, NULL}}, {NULL, {0, NULL}}, 0, 0,
        }
    },
    TC_IOT_SHADOW_SUB_TOPIC_DEF,
    TC_IOT_SHADOW_PUB_TOPIC_DEF,
    tc_iot_device_on_group_message_received,
    TC_IOT_PROPTOTAL,
    &g_tc_iot_shadow_property_defs[0],
    _tc_iot_shadow_property_control_callback,
    &g_tc_iot_device_local_data,
    &g_tc_iot_device_reported_data,
    &g_tc_iot_device_desired_data,
};


static int _tc_iot_property_change( int property_id, void * data) {
/*${data_template.generate_sample_code()}*/
}

int _tc_iot_shadow_property_control_callback(tc_iot_event_message *msg, void * client,  void * context) {
    tc_iot_shadow_property_def * p_property = NULL;
    tc_iot_message_data * md = NULL;
    tc_iot_sub_device_event_data * sde = NULL;
    tc_iot_sub_device_info * sub_device = NULL;
    void * p_data_start = NULL;

    if (!msg) {
        TC_IOT_LOG_ERROR("msg is null.");
        return TC_IOT_FAILURE;
    }

    if (msg->event == TC_IOT_SHADOW_EVENT_SERVER_CONTROL) {
        p_property = (tc_iot_shadow_property_def *)context;
        if (!p_property) {
            TC_IOT_LOG_ERROR("p_property is null.");
            return TC_IOT_FAILURE;
        }

        return _tc_iot_property_change(p_property->id, msg->data);
    } else if (msg->event == TC_IOT_MQTT_EVENT_ERROR_NOTIFY) {
        md = (tc_iot_message_data *)msg->data;
        if (md->error_code == TC_IOT_MQTT_OVERSIZE_PACKET_RECEIVED ) {
            TC_IOT_LOG_ERROR("error 'oversized package received' notified.");
        } else {
            TC_IOT_LOG_ERROR("error notified with code: %d", md->error_code);
        }
    } else if (msg->event == TC_IOT_SUB_DEV_SEQUENCE_RECEIVED) {
        sde = (tc_iot_sub_device_event_data *)msg->data;
        TC_IOT_LOG_TRACE("-----sequence received: product=%s,device_name=%s,%s=%s------",
                         sde->product_id, sde->device_name, sde->name, sde->value);
        sub_device = tc_iot_sub_device_info_find(&g_tc_iot_sub_device_table, sde->product_id, sde->device_name);
        if (sub_device) {
            sub_device->sequence = tc_iot_try_parse_uint(sde->value, NULL);
        }
    } else if (msg->event == TC_IOT_SUB_DEV_SERVER_CONTROL_DEVICE) {
        sde = (tc_iot_sub_device_event_data *)msg->data;
        TC_IOT_LOG_TRACE("%s/%s:%s=%s",
                         sde->product_id, sde->device_name, sde->name, sde->value);
        p_property = tc_iot_sub_device_mark_report_field(&g_tc_iot_sub_device_table, sde->product_id, sde->device_name, sde->name);
        sub_device = tc_iot_sub_device_info_find(&g_tc_iot_sub_device_table, sde->product_id, sde->device_name);
        if (sub_device && p_property) {
            p_data_start = (char *)sub_device->p_data+p_property->offset;
            switch (p_property->type) {
            case TC_IOT_SHADOW_TYPE_BOOL:
                *(tc_iot_shadow_bool *)p_data_start = atoi(sde->value);
                break;
            case TC_IOT_SHADOW_TYPE_ENUM:
                *(tc_iot_shadow_enum *)p_data_start = atoi(sde->value);
                break;
            case TC_IOT_SHADOW_TYPE_INT:
                *(tc_iot_shadow_int *)p_data_start = atoi(sde->value);
                break;
            case TC_IOT_SHADOW_TYPE_NUMBER:
                *(tc_iot_shadow_number *)p_data_start = atof(sde->value);
                break;
            case TC_IOT_SHADOW_TYPE_STRING:
                strncpy(p_data_start, sde->value, p_property->len);
                break;
            default:
                TC_IOT_LOG_ERROR("%s type=%d invalid.", p_property->name, p_property->type);
                break;
            }
        }
        tc_iot_sub_device_mark_confirm_field(&g_tc_iot_sub_device_table, sde->product_id, sde->device_name, sde->name);
    } else if (msg->event == TC_IOT_SUB_DEV_SERVER_CONTROL_DEVICE_FINISHED) {
        sde = (tc_iot_sub_device_event_data *)msg->data;
        TC_IOT_LOG_TRACE("-----device control finished: product=%s,device_name=%s---",
                         sde->product_id, sde->device_name);
    } else if (msg->event == TC_IOT_SUB_DEV_SERVER_CONTROL_PRODUCT_FINISHED) {
        sde = (tc_iot_sub_device_event_data *)msg->data;
        TC_IOT_LOG_TRACE("product control finished: product=%s", sde->product_id);
    } else if (msg->event == TC_IOT_SUB_DEV_SERVER_CONTROL_ALL_FINISHED) {
        TC_IOT_LOG_TRACE("all control finished.");
        tc_iot_sub_device_report(client, &g_tc_iot_sub_device_table.items[0],  g_tc_iot_sub_device_table.used);
        tc_iot_sub_device_confirm(client, &g_tc_iot_sub_device_table.items[0],  g_tc_iot_sub_device_table.used);
    } else {
        TC_IOT_LOG_TRACE("unkown event received, event=%d", msg->event);
    }
    return TC_IOT_SUCCESS;
}
