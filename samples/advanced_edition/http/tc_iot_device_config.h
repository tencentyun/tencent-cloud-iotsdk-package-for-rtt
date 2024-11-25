#ifndef TC_IOT_DEVICE_CONFIG_H
#define TC_IOT_DEVICE_CONFIG_H

#include "tc_iot_config.h"
#include "rtconfig.h"

/**********************************���������� begin ********************************/
/* �豸�����ȡ secret �ӿڣ���ַ��ʽΪ��<������ʶ>.auth-device-iot.tencentcloudapi.com/secret */
/* Token�ӿڣ���ַ��ʽΪ��<������ʶ>.auth-device-iot.tencentcloudapi.com/token */
/* ������ʶ��
    ���ݻ���=gz
    ��������=bj
    ...
*/
#define TC_IOT_CONFIG_REGION  "gz"

/* ����������Ҫ���ڹ���������Ʒ���豸��Ȼ���ȡ�����Ϣ����*/
/* MQ�����ַ�������ڲ�Ʒ��������Ϣ��->��mqtt���ӵ�ַ��λ���ҵ���*/
#define TC_IOT_CONFIG_MQ_SERVER_HOST "mqtt-1egtdjqza.ap-guangzhou.mqtt.tencentcloudmq.com"


#ifdef  PKG_USING_TENCENT_IOTKIT

/* ��Ʒid�������ڲ�Ʒ��������Ϣ��->����Ʒid��λ���ҵ�*/
#define TC_IOT_CONFIG_DEVICE_PRODUCT_ID PKG_USING_TENCENT_IOTKIT_PRODUCT_ID

/* ��Ʒid�������ڲ�Ʒ��������Ϣ��->����Ʒkey��λ���ҵ�*/
#define TC_IOT_CONFIG_DEVICE_PRODUCT_KEY PKG_USING_TENCENT_IOTKIT_PRODUCT_KEY

/* �豸��Կ�������ڲ�Ʒ���豸������->���豸֤�顱->��Device Secret��λ���ҵ�*/
#define TC_IOT_CONFIG_DEVICE_SECRET  PKG_USING_TENCENT_IOTKIT_DEVICE_SECRET
									 

/* �豸���ƣ������ڲ�Ʒ���豸������->���豸���ơ�λ���ҵ�*/
#define TC_IOT_CONFIG_DEVICE_NAME PKG_USING_TENCENT_IOTKIT_DEVICE_NAME

/* ��Ȩģʽ�������ڲ�Ʒ�ġ�������Ϣ��->����Ȩģʽ��λ���ҵ�
 * 1:��̬����ģʽ
 * 2:ǩ����֤ģʽ
 * */

#ifdef PKG_USING_TENCENT_IOTKIT_IS_TOKEN
#define TC_IOT_CONFIG_AUTH_MODE   1
#else
#define TC_IOT_CONFIG_AUTH_MODE   2    //PKG_USING_TENCENT_IOTKIT_IS_SING
#endif


/**********************************���������� end ********************************/


/**********************************ѡ�������� begin ********************************/
/*
 * ���Ǵ��ڵ��Ի�����Ӧ�ó�������������һ������²���Ҫ�ֶ��޸ġ�
 * */

/*------------------MQTT begin---------------------*/
#ifdef PKG_USING_TENCENT_IOTKIT_MQTT_TLS 
// ���� API ʱʹ�õ�Э�飬https ���� http
#define TC_IOT_CONFIG_API_HTTP_PROTOCOL "https"

// MQ ����� MQTT �Ƿ�ͨ�� TLS Э��ͨѶ��1 Ϊʹ�ã�0 ��ʾ��ʹ��
#define TC_IOT_CONFIG_USE_TLS 1 
/* MQ�����TLS�˿�һ��Ϊ8883*/
#define TC_IOT_CONFIG_MQ_SERVER_PORT 8883

#else
// ���� API ʱʹ�õ�Э�飬https ���� http
#define TC_IOT_CONFIG_API_HTTP_PROTOCOL "http"

// MQ ����� MQTT �Ƿ�ͨ�� TLS Э��ͨѶ��1 Ϊʹ�ã�0 ��ʾ��ʹ��
#define TC_IOT_CONFIG_USE_TLS 0
/* MQ�����Ĭ�϶˿�һ��Ϊ1883*/
#define TC_IOT_CONFIG_MQ_SERVER_PORT 1883
#endif



#else
/* ����������Ҫ���ڹ���������Ʒ���豸��Ȼ���ȡ�����Ϣ����*/
/* MQ�����ַ�������ڲ�Ʒ��������Ϣ��->��mqtt���ӵ�ַ��λ���ҵ���*/
#define TC_IOT_CONFIG_MQ_SERVER_HOST "mqtt-1egtdjqza.ap-guangzhou.mqtt.tencentcloudmq.com"



/* ��Ʒid�������ڲ�Ʒ��������Ϣ��->����Ʒid��λ���ҵ�*/
#define TC_IOT_CONFIG_DEVICE_PRODUCT_ID "iot-1l60dtl0"

/* ��Ʒid�������ڲ�Ʒ��������Ϣ��->����Ʒkey��λ���ҵ�*/
#define TC_IOT_CONFIG_DEVICE_PRODUCT_KEY "mqtt-1egtdjqza"

/* �豸��Կ�������ڲ�Ʒ���豸������->���豸֤�顱->��Device Secret��λ���ҵ�*/
#define TC_IOT_CONFIG_DEVICE_SECRET "92b0e676cdd608c4fb56386967613764"
									 

/* �豸���ƣ������ڲ�Ʒ���豸������->���豸���ơ�λ���ҵ�*/
#define TC_IOT_CONFIG_DEVICE_NAME "rt_thread_dev1"

/* ��Ȩģʽ�������ڲ�Ʒ�ġ�������Ϣ��->����Ȩģʽ��λ���ҵ�
 * 1:��̬����ģʽ
 * 2:ǩ����֤ģʽ
 * */
#define TC_IOT_CONFIG_AUTH_MODE   2

/**********************************���������� end ********************************/


/**********************************ѡ�������� begin ********************************/
/*
 * ���Ǵ��ڵ��Ի�����Ӧ�ó�������������һ������²���Ҫ�ֶ��޸ġ�
 * */

/*------------------MQTT begin---------------------*/
#ifdef ENABLE_TLS
// ���� API ʱʹ�õ�Э�飬https ���� http
#define TC_IOT_CONFIG_API_HTTP_PROTOCOL "https"

// MQ ����� MQTT �Ƿ�ͨ�� TLS Э��ͨѶ��1 Ϊʹ�ã�0 ��ʾ��ʹ��
#define TC_IOT_CONFIG_USE_TLS 1 
/* MQ�����TLS�˿�һ��Ϊ8883*/
#define TC_IOT_CONFIG_MQ_SERVER_PORT 8883

#else
// ���� API ʱʹ�õ�Э�飬https ���� http
#define TC_IOT_CONFIG_API_HTTP_PROTOCOL "http"

// MQ ����� MQTT �Ƿ�ͨ�� TLS Э��ͨѶ��1 Ϊʹ�ã�0 ��ʾ��ʹ��
#define TC_IOT_CONFIG_USE_TLS 0
/* MQ�����Ĭ�϶˿�һ��Ϊ1883*/
#define TC_IOT_CONFIG_MQ_SERVER_PORT 1883
#endif

#endif

/* client id ����������ɣ������ʽΪ��ProductKey@DeviceName�� */
#define TC_IOT_CONFIG_DEVICE_CLIENT_ID TC_IOT_CONFIG_DEVICE_PRODUCT_KEY "@" TC_IOT_CONFIG_DEVICE_NAME


/* ����username��password��*/
/* 1)�����ͨ��TC_IOT_CONFIG_AUTH_API_URL�ӿڣ���̬��ȡ�ģ��������������ɲ�����д*/
/* 2)�����Ԥ������õĹ̶�username��password�����԰ѻ�ȡ���Ĺ̶�������д������λ��*/
#if TC_IOT_CONFIG_AUTH_MODE == 1
/* ��̬����ģʽ */
#define TC_IOT_CONFIG_DEVICE_USER_NAME ""
#define TC_IOT_CONFIG_DEVICE_PASSWORD ""
#define TC_IOT_AUTH_FUNC   tc_iot_refresh_auth_token
#elif TC_IOT_CONFIG_AUTH_MODE == 2
/*ǩ����֤ģʽ*/
#define TC_IOT_AUTH_FUNC   tc_iot_mqtt_refresh_dynamic_sign
#define TC_IOT_CONFIG_DEVICE_USER_NAME ""
#define TC_IOT_CONFIG_DEVICE_PASSWORD ""
#else
/* ֱ��ģʽ */
#define TC_IOT_AUTH_FUNC(a,b,c,d)   
#define TC_IOT_CONFIG_DEVICE_USER_NAME "your_device_name"
#define TC_IOT_CONFIG_DEVICE_PASSWORD "your_device_password"
#endif

// begin: DEBUG use only 
#define TC_IOT_CONFIG_ACTIVE_API_URL_DEBUG   "http://" TC_IOT_SERVER_REGION ".auth.iot.cloud.tencent.com/secret"
#define TC_IOT_CONFIG_AUTH_API_URL_DEBUG	 "http://" TC_IOT_SERVER_REGION ".auth.iot.cloud.tencent.com/token"
// end: DEBUG use only 

// API ������������ַ
#define TC_IOT_CONFIG_API_DOMAIN_ROOT TC_IOT_CONFIG_API_HTTP_PROTOCOL "://" TC_IOT_CONFIG_REGION ".auth-device-iot.tencentcloudapi.com"

// �豸��̬���������ַ
#define TC_IOT_CONFIG_ACTIVE_API_URL  TC_IOT_CONFIG_API_DOMAIN_ROOT "/secret"

// �豸��̬���������ַ
#define TC_IOT_CONFIG_AUTH_API_URL    TC_IOT_CONFIG_API_DOMAIN_ROOT "/token"

/* connect��publish��subscribe��unsubscribe */
/* ������ִ�г�ʱʱ������λ�Ǻ���*/
#define TC_IOT_CONFIG_COMMAND_TIMEOUT_MS  10000
/* TLS ����ִ�г�ʱʱ������λ�Ǻ���*/
#define TC_IOT_CONFIG_TLS_HANDSHAKE_TIMEOUT_MS  10000
/* keep alive ���ʱ������λ����*/
#define TC_IOT_CONFIG_KEEP_ALIVE_INTERVAL_SEC  60
/* ������ϻ����˳�ʱ���Ƿ��Զ�����*/
#define TC_IOT_CONFIG_AUTO_RECONNECT 1

#define TC_IOT_CONFIG_CLEAN_SESSION 1

/* �Զ���Э��ʱ������������Ϣtopic��mq����˵���Ӧ���������ͣ�*/
/* ���ᷢ���� "<product id>/<device name>/cmd" ���topic���� Topic
 * ��Ҫ�û�ʹ��ǰ�������ڡ�Topic ��������д���*/
/* �ͻ���ֻ��Ҫ�������topic����*/
#define TC_IOT_MQTT_SUB_TOPIC_FMT "%s/%s/cmd"
#define TC_IOT_MQTT_SUB_TOPIC_DEF TC_IOT_CONFIG_DEVICE_PRODUCT_ID "/" TC_IOT_CONFIG_DEVICE_NAME "/cmd"

/* �Զ���Э��ʱ������������Ϣtopic���ͻ����������˵���Ϣ�����������topic����*/
/* topic��ʽ"<product id>/<device name>/update" ���topic���� Topic
 * ��Ҫ�û�ʹ��ǰ�������ڡ�Topic ��������д���*/
#define TC_IOT_MQTT_PUB_TOPIC_FMT "%s/%s/update"
#define TC_IOT_MQTT_PUB_TOPIC_DEF TC_IOT_CONFIG_DEVICE_PRODUCT_ID "/" TC_IOT_CONFIG_DEVICE_NAME "/update"

/* shadow������Ϣtopic��mq����˵���Ӧ���������ͣ�*/
/* ���ᷢ���� "shadow/get/<product id>/<device name>" ���topic*/
/* �ͻ���ֻ��Ҫ�������topic����*/
#define TC_IOT_SHADOW_SUB_TOPIC_PREFIX "shadow/get/"
#define TC_IOT_SHADOW_SUB_TOPIC_FMT TC_IOT_SHADOW_SUB_TOPIC_PREFIX "%s/%s"
#define TC_IOT_SHADOW_SUB_TOPIC_DEF TC_IOT_SHADOW_SUB_TOPIC_PREFIX TC_IOT_CONFIG_DEVICE_PRODUCT_ID "/" TC_IOT_CONFIG_DEVICE_NAME

/* shadow������Ϣtopic���ͻ����������˵���Ϣ�����������topic����*/
/* topic��ʽ"shadow/update/<product id>/<device name>"*/
#define TC_IOT_SHADOW_PUB_TOPIC_PREFIX "shadow/update/"
#define TC_IOT_SHADOW_PUB_TOPIC_FMT TC_IOT_SHADOW_PUB_TOPIC_PREFIX "%s/%s"
#define TC_IOT_SHADOW_PUB_TOPIC_DEF TC_IOT_SHADOW_PUB_TOPIC_PREFIX TC_IOT_CONFIG_DEVICE_PRODUCT_ID "/" TC_IOT_CONFIG_DEVICE_NAME

/*------------------MQTT end---------------------*/


/*------------------COAP begin---------------------*/

// COAP �ϱ���ַ��tp= Ϊ�̶���������ǰ׺��/xxxx/yyyy/update Ϊʵ�� Topic
#define  TC_IOT_COAP_PUB_TOPIC_PARM_FMT   "tp=/%s/%s/update"

// COAP RPC �������е�ַ��st= Ϊ�̶���������ǰ׺��shadow/get/xxx Ϊʵ�� Topic
#define  TC_IOT_COAP_RPC_SUB_TOPIC_PARM_FMT   "st=shadow/get/%s/%s"

// COAP RPC ������������е�ַ��pt= Ϊ�̶���������ǰ׺��shadow/get/xxx Ϊʵ�� Topic
#define  TC_IOT_COAP_RPC_PUB_TOPIC_PARM_FMT   "pt=shadow/update/%s/%s"

#define TC_IOT_COAP_DTLS_PSK "secretPSK"
#define TC_IOT_COAP_DTLS_PSK_ID TC_IOT_CONFIG_DEVICE_NAME


#define TC_IOT_CONFIG_COAP_SERVER_HOST TC_IOT_CONFIG_REGION ".coap.tencentcloudapi.com"

#ifdef ENABLE_DTLS
/* �Ƿ�����DTLS����CoAP����*/
#define TC_IOT_CONFIG_USE_DTLS 1
#else
#define TC_IOT_CONFIG_USE_DTLS 0
#endif


/* TLS ����ִ�г�ʱʱ������λ�Ǻ���*/
#define TC_IOT_CONFIG_DTLS_HANDSHAKE_TIMEOUT_MS  10000

#if TC_IOT_CONFIG_USE_DTLS
/* CoAP�����DTLS�˿�һ��Ϊ5684*/
#define TC_IOT_CONFIG_COAP_SERVER_PORT 5684
#else
/* MQ�����Ĭ�϶˿�һ��Ϊ5683*/
#define TC_IOT_CONFIG_COAP_SERVER_PORT 5683
#endif

/*------------------COAP end---------------------*/

/*------------------HTTP begin---------------------*/
#define TC_IOT_CONFIG_RPC_API_URL  TC_IOT_CONFIG_API_DOMAIN_ROOT "/rpc"
/*------------------HTTP end---------------------*/

/*-----------------COMMON begin----------------------*/
/* tls �������*/
/* ��֤��·��*/
#define TC_IOT_CONFIG_ROOT_CA NULL
/* �ͻ���֤��·��*/
#define TC_IOT_CONFIG_CLIENT_CRT NULL
/* �ͻ���˽Կ·��*/
#define TC_IOT_CONFIG_CLIENT_KEY NULL

#define TC_IOT_TROUBLE_SHOOTING_URL "https://git.io/vN9le"
/*-----------------COMMON end----------------------*/

/**********************************ѡ�������� end ********************************/

#endif /* end of include guard */
