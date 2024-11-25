#ifndef TC_IOT_SUB_DEVICE_H
#define TC_IOT_SUB_DEVICE_H


/*--- begin 子设备请求 method 字段取值----*/
/* 请求类 */
/**< 子设备上线
 *
 {
	"method": "device_online",
	"passthrough": {
		"sid": "c58a000e"
	},
	"timestamp": 12212121212,
	"sub_dev_state": [{
			"product": "iot-ammeter",
			"device_list": [{
					"dev_name": "ammeter_3",
					"sign": "4+idpxXyGvCPg4BcWZzCqaWFN1QQwvkrFtQT2KjHHvc="
				}
			]
		},
		{
			"product": "iot-locker",
			"device_list": [{
					"dev_name": "lock_3",
					"sign": "asdfasdfasdfasdfasdfasdfasdf"

				}
			]
		}
	]
}
 * */
#define TC_IOT_SUB_DEVICE_ONLINE           "device_online"

#define TC_IOT_SUB_DEVICE_OFFLINE          "device_offline"

#define TC_IOT_SUB_DEVICE_GROUP_UPDATE     "group_update"
#define TC_IOT_SUB_DEVICE_GROUP_GET        "group_get"
#define TC_IOT_SUB_DEVICE_GROUP_DELETE     "group_delete"
#define TC_IOT_SUB_DEVICE_GROUP_CONTROL     "group_control"

/*--- end 子设备请求 method 字段取值----*/

#define TC_IOT_GROUP_DOC_ROOT_DEPTH      1
#define TC_IOT_GROUP_DOC_PRODUCT_DEPTH   2
#define TC_IOT_GROUP_DOC_DEVICE_DEPTH    4

typedef struct _tc_iot_sub_device_info {
    char product_id[TC_IOT_MAX_PRODUCT_ID_LEN]; /**< 设备 Product Id*/
    char device_name[TC_IOT_MAX_DEVICE_NAME_LEN];  /**< 设备 Device Name*/
    char device_secret[TC_IOT_MAX_SECRET_LEN];  /**< 设备签名秘钥*/
    char desired_bits[(TC_IOT_MAX_PROPERTY_COUNT/8)+1];
    char reported_bits[(TC_IOT_MAX_PROPERTY_COUNT/8)+1];
    int property_total;
    tc_iot_shadow_property_def * properties;
    void * p_data;
    unsigned int sequence;
} tc_iot_sub_device_info;


typedef struct _tc_io_sub_device_table{
    int total;
    int used;
    tc_iot_sub_device_info * items;
} tc_iot_sub_device_table;

typedef struct _tc_iot_sub_device_event_data {
    const char * product_id;
    const char * device_name;
    const char * name;
    const char * value;
}tc_iot_sub_device_event_data;

int tc_iot_sub_device_onoffline(tc_iot_shadow_client * c, tc_iot_sub_device_info * sub_devices, int sub_devices_count, bool is_online);
void tc_iot_group_req_message_ack_callback(tc_iot_command_ack_status_e ack_status, tc_iot_message_data * md , void * session_context);
void tc_iot_group_get_message_ack_callback(tc_iot_command_ack_status_e ack_status, tc_iot_message_data * md , void * session_context);

void tc_iot_device_on_group_message_received(tc_iot_message_data* md);
int tc_iot_group_doc_parse(tc_iot_shadow_client * p_shadow_client, tc_iot_json_tokenizer * tokenizer);
int tc_iot_group_control_process(tc_iot_shadow_client * c, tc_iot_json_tokenizer * tokenizer, int product_index);

int tc_iot_sub_device_group_doc_init(tc_iot_shadow_client * c, char * buffer, int buffer_len, const char * method);
int tc_iot_sub_device_group_doc_add_product(char * buffer, int buffer_len, const char * product_id);
int tc_iot_sub_device_group_doc_add_device(char * buffer, int buffer_len, const char * device_name, unsigned int sequence);
int tc_iot_sub_device_group_doc_add_state_holder(char * buffer, int buffer_len, const char * state_holder);
int tc_iot_sub_device_group_doc_add_data(char * buffer, int buffer_len, int depth, const char * name,
                                         tc_iot_shadow_data_type_e type , const void * value);
int tc_iot_sub_device_group_doc_pub(tc_iot_shadow_client * c, char * buffer, int buffer_len,
                                     message_ack_handler callback,
                                    int timeout_ms, void * session_context);

// for member id begin
#define TC_IOT_DECLARE_LOCAL_MEMBER_DEF(product,member,type) {      \
    #member, TC_IOT_PROP_ ## product ## _ ## member, type,offsetof(tc_iot_shadow_local_data_ ##product, member), \
        TC_IOT_MEMBER_SIZE(tc_iot_shadow_local_data_ ##product, member) \
}
// for member id end


int tc_iot_sub_device_confirm(tc_iot_shadow_client * c, tc_iot_sub_device_info * sub_devices, int sub_devices_count);
int tc_iot_sub_device_report(tc_iot_shadow_client * c, tc_iot_sub_device_info * sub_devices, int sub_devices_count);
bool tc_iot_sub_device_info_need_report(tc_iot_sub_device_info * current);
bool tc_iot_sub_device_info_need_confirm(tc_iot_sub_device_info * current);

tc_iot_sub_device_info * tc_iot_sub_device_register(tc_iot_sub_device_table * t,
                                                            const char * product_id,
                                                            const char * device_name,
                                                            const char * device_secret,
                                                            int property_total,
                                                            tc_iot_shadow_property_def * properties,
                                                            void * p_data);
tc_iot_sub_device_info * tc_iot_sub_device_info_find(tc_iot_sub_device_table * t,const char * product_id, const char * device_name);
tc_iot_shadow_property_def * tc_iot_sub_device_mark_report_field(tc_iot_sub_device_table * t,const char * product_id,
                                                                  const char * device_name,
                                                                  const char * field_name);
tc_iot_shadow_property_def * tc_iot_sub_device_mark_confirm_field(tc_iot_sub_device_table * t,const char * product_id,
                                                                 const char * device_name,
                                                                 const char * field_name);

#endif /* TC_IOT_SUB_DEVICE_H */
