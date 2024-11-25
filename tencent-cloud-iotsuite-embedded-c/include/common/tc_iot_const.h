#ifndef IOT_CONST_H
#define IOT_CONST_H

/**
 * @brief 系统常见错误码定义，基本定义原则：
 * 1. =0 表示SUCCESS状态，即函数调用执行成功
 * 2. <0 表示系统出现异常，返回值为对应如下错误码
 * 3. >0 用于读写、收发类函数，表示当前操作对应执行成功的字节数。
 */
typedef enum _tc_iot_sys_code_e {
    TC_IOT_SUCCESS = 0,               /**< 执行成功 */
    TC_IOT_FAILURE = -90,             /**< 执行失败*/
    TC_IOT_INVALID_PARAMETER = -91,   /**< 参数取值非法 */
    TC_IOT_NULL_POINTER = -92,        /**< 参数指针为空 */
    TC_IOT_TLS_NOT_SUPPORTED = -93,   /**< 系统不支持 TLS 连接方式*/
    TC_IOT_BUFFER_OVERFLOW = -94,     /**< 函数输出参数内存空间不足，无法完成数据输出*/
    TC_IOT_FUCTION_NOT_IMPLEMENTED = -99,     /**< 适配函数未做迁移实现 */

    TC_IOT_NETWORK_ERROR_BASE = -100, /**< 网络类错误*/
    TC_IOT_NET_UNKNOWN_HOST = -101, /**< 域名解析失败*/
    TC_IOT_NET_CONNECT_FAILED = -102, /**< 网络连接失败*/
    TC_IOT_NET_SOCKET_FAILED = -103, /**< Socket操作失败*/
    TC_IOT_SEND_PACK_FAILED = -104, /**< 发送数据失败*/
    TC_IOT_NET_NOTHING_READ = -105, /**< 未读取到数据*/
    TC_IOT_NET_READ_TIMEOUT = -106, /**< 读取数据超时*/
    TC_IOT_NET_READ_ERROR = -107, /**< 读取数据失败*/

    TC_IOT_MQTT_RECONNECT_TIMEOUT = -150, /**< MQTT 重连超时*/
    TC_IOT_MQTT_RECONNECT_IN_PROGRESS = -151, /**< MQTT 正在尝试重连*/
    TC_IOT_MQTT_RECONNECT_FAILED = -152, /**< 重连失败*/
    TC_IOT_MQTT_NETWORK_UNAVAILABLE = -153,/**< 网络不可用*/
    TC_IOT_MQTT_WAIT_ACT_TIMEOUT = -154, /**< 等待ACK响应操作*/
    TC_IOT_MQTT_OVERSIZE_PACKET_RECEIVED = -155, /**< 收到超大 Pulish 数据包，导致接收缓存无法处理 */

    TC_IOT_MQTT_CONNACK_ERROR = -160, /**< 连接失败，未知错误*/
    TC_IOT_MQTT_CONNACK_PROTOCOL_UNACCEPTABLE = -161, /**< 连接失败，不支持请求的协议版本*/
    TC_IOT_MQTT_CONNACK_CLIENT_ID_INVALID = -162, /**< 连接失败，Client Id 判定非法*/
    TC_IOT_MQTT_CONNACK_SERVICE_UNAVAILABLE = -163,  /**< 连接失败，后端服务不可用*/
    TC_IOT_MQTT_CONNACK_BAD_USER_OR_PASSWORD = -164,  /**< 连接失败，用户名或密码校验不通过*/
    TC_IOT_MQTT_CONNACK_NOT_AUTHORIZED = -165,  /**< 连接失败，用户访问未授权*/
    TC_IOT_MQTT_SUBACK_FAILED = -170,  /**< 订阅失败，有可能 Topic 不存在，或者无权限等*/

    TC_IOT_MBED_TLS_ERROR_BASE = -200, /**< TLS 错误*/
    TC_IOT_CTR_DRBG_SEED_FAILED = -201, /**< TLS 初始化失败*/
    TC_IOT_X509_CRT_PARSE_FILE_FAILED = -202, /**< 解析证书文件失败*/
    TC_IOT_PK_PRIVATE_KEY_PARSE_ERROR = -203, /**< 解析私钥文件失败*/
    TC_IOT_PK_PARSE_KEYFILE_FAILED = -204, /**< 解析证书失败 */
    TC_IOT_TLS_NET_SET_BLOCK_FAILED = -205,/**< 设置 TLS Block状态失败*/
    TC_IOT_TLS_SSL_CONFIG_DEFAULTS_FAILED = -206,/**< 配置 TLS 参数失败*/
    TC_IOT_TLS_SSL_CONF_OWN_CERT_FAILED = -207, /**< 设置客户端证书失败*/
    TC_IOT_TLS_SSL_SETUP_FAILED = -208,/**< TLS 配置失败*/
    TC_IOT_TLS_SSL_SET_HOSTNAME_FAILED = -209,/**< TLS 设置服务端域名失败*/
    TC_IOT_TLS_SSL_HANDSHAKE_FAILED = -210, /**< TLS 握手失败*/
    TC_IOT_TLS_X509_CRT_VERIFY_FAILED = -211, /**< 证书验证失败*/
    TC_IOT_TLS_SSL_WRITE_FAILED = -212, /**< TLS 写入操作失败*/
    TC_IOT_TLS_SSL_WRITE_TIMEOUT = -213,/**< TLS 写入操作超时*/
    TC_IOT_TLS_SSL_READ_FAILED = -214,/**< TLS 读取失败*/
    TC_IOT_TLS_SSL_READ_TIMEOUT = -215,/**< TLS 读取超时*/

    TC_IOT_COAP_PACK_INVALID = -300, /**< COAP包解析失败*/
    TC_IOT_COAP_MSG_VER_INVALID = -301, /**< COAP包ver字段取值非法*/
    TC_IOT_COAP_MSG_TYPE_INVALID = -302, /**< COAP包type字段取值非法*/
    TC_IOT_COAP_MSG_TOKEN_LEN_INVALID = -303, /**< COAP包TKL取值非法*/
    TC_IOT_COAP_MSG_OPTION_DELTA_INVALID = -304, /**< COAP包Option Delta取值非法*/
    TC_IOT_COAP_MSG_OPTION_LENGTH_INVALID = -305, /**< COAP包Option Length取值非法*/
    TC_IOT_COAP_MSG_OPTION_COUNT_TOO_MUCH = -306, /**< COAP包Option 数量超大*/

    TC_IOT_LOGIC_ERROR_BASE = -1000, /**< 业务逻辑错误 */
    TC_IOT_NETWORK_PTR_NULL = -1001,/**< network 对象指针为空*/
    TC_IOT_NETCONTEXT_PTR_NULL = -1002, /**< netcontext 对象指针为空*/
    TC_IOT_JSON_PATH_NO_MATCH = -1003, /**< json path 检索失败，未找到指定对象*/
    TC_IOT_JSON_PARSE_FAILED = -1004, /**< json 解析失败*/
    TC_IOT_ERROR_HTTP_REQUEST_FAILED = -1005, /**<  http 请求失败*/
    TC_IOT_REFRESH_TOKEN_FAILED = -1006, /**< 获取用户Token失败 */
    TC_IOT_HTTP_RESPONSE_INVALID = -1007, /**< 服务端相应的 HTTP 数据格式非法 */
    TC_IOT_SHADOW_SESSION_NOT_ENOUGH = -1008, /**< 影子回调会话空间不够，请检查是否发送过多并发请求，
                                                合理设置 TC_IOT_MAX_SESSION_COUNT 的值*/
    TC_IOT_HTTP_REDIRECT_TOO_MANY = -1009, /**< HTTP 跳转次数过多，有可能是跳转循环 */
    TC_IOT_REPORT_SKIPPED_FOR_NO_CHANGE = -1010, /**< 数据无变动，无需上报*/
    TC_IOT_FIRM_INFO_NAME_TOO_LONG = -1011, /**< 上报的设备信息字段名称太长*/
    TC_IOT_FIRM_INFO_VALUE_TOO_LONG = -1012, /**< 上报的设备信息字段取值太长*/
    TC_IOT_HTTP_RESPONSE_NO_CONTENT_LENGTH = -1013, /**< HTTP 响应中未包含Content-Length字段*/
    TC_IOT_JSON_PARSE_TOKEN_NO_MEM = -1014, /**< json 解析Token数不足*/
    TC_IOT_HTTP_RPC_FAILED = -1015, /**< HTTP rpc 接口请求失败*/
    TC_IOT_INVALID_HTTP_CHUNK_FORMAT = -1016, /** HTTP 返回的 chunk 数据格式非法 */

    TC_IOT_HTTP_INVALID_STATUS_CODE = -1201, /** 返回码错误，不是 1xx ~ 5xx 返回码 */
    TC_IOT_HTTP_BODY_START_FOUND = -1204, /** 发现 HTTP 包体 */

} tc_iot_sys_code_e;

#define TC_IOT_ARRAY_LENGTH(a)  (sizeof(a)/sizeof(a[0]))


typedef enum _tc_iot_event_e {
    TC_IOT_EVENT_UNKNOWN = 0,
    TC_IOT_SYS_EVENT_BASE = 1,
    TC_IOT_MQTT_EVENT_BASE = 50,
    TC_IOT_MQTT_EVENT_ERROR_NOTIFY = 51,
    TC_IOT_SHADOW_EVENT_BASE = 100,
    TC_IOT_SHADOW_EVENT_SERVER_CONTROL,
    TC_IOT_SUB_DEV_SEQUENCE_RECEIVED = 110,
    TC_IOT_SUB_DEV_SERVER_CONTROL_DEVICE,
    TC_IOT_SUB_DEV_SERVER_CONTROL_DEVICE_FINISHED,
    TC_IOT_SUB_DEV_SERVER_CONTROL_PRODUCT_FINISHED,
    TC_IOT_SUB_DEV_SERVER_CONTROL_ALL_FINISHED,
    TC_IOT_USER_EVENT_BASE = 200,
} tc_iot_event_e;


typedef struct _tc_iot_code_map {
    int code;
    const char * str;
} tc_iot_code_map;

typedef struct _tc_iot_event_message {
    tc_iot_event_e  event;
    void * data;
} tc_iot_event_message;

/**
* @brief tc_iot_event_handler 通用事件回调原型
*
* @param msg 消息内容
* @param src 触发事件的来源
* @param context 附加 context 信息。
*
* @return @see tc_iot_sys_code_e
*/
typedef int (*tc_iot_event_handler)(tc_iot_event_message *msg, void * client,  void * context);

#define TC_IOT_CONTAINER_OF(ptr, type, member)		\
				(type *)((char *)ptr - offsetof(type, member))

#define tc_iot_const_str_len(a)  (sizeof(a)-1)

#define tc_iot_str2_equal(m, c0, c1) \
(m[0] == c0 && m[1] == c1)

#define tc_iot_str3_equal(m, c0, c1, c2) \
(m[0] == c0 && m[1] == c1 && m[2] == c2)

#define tc_iot_str4equal(m, c0, c1, c2, c3) \
(m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3)

#define tc_iot_str5equal(m, c0, c1, c2, c3, c4) \
(m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 && m[4] == c4)

#define tc_iot_str6equal(m, c0, c1, c2, c3, c4, c5) \
(m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 \
    && m[4] == c4 && m[5] == c5)

#define tc_iot_str7equal(m, c0, c1, c2, c3, c4, c5, c6) \
(m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 \
    && m[4] == c4 && m[5] == c5 && m[6] == c6)

#define tc_iot_str8equal(m, c0, c1, c2, c3, c4, c5, c6, c7) \
(m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3       \
    && m[4] == c4 && m[5] == c5 && m[6] == c6 && m[7] == c7)

#define tc_iot_str9equal(m, c0, c1, c2, c3, c4, c5, c6, c7, c8) \
(m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3            \
    && m[4] == c4 && m[5] == c5 && m[6] == c6 && m[7] == c7 && m[8] == c8)
#define TC_IOT_MEMBER_SIZE(type, member) sizeof(((type *)0)->member)

#endif /* end of include guard */
