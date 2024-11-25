#ifndef TC_IOT_HAL_NETWORK_H
#define TC_IOT_HAL_NETWORK_H

#include "tc_iot_inc.h"

typedef enum _tc_iot_network_type {
    TC_IOT_SOCK_STREAM,
    TC_IOT_SOCK_DGRAM,
}tc_iot_network_type;

typedef enum _tc_iot_network_protocol {
    TC_IOT_PROTO_HTTP,
    TC_IOT_PROTO_MQTT,
    TC_IOT_PROTO_COAP,
} tc_iot_network_protocol;

typedef struct tc_iot_network_t tc_iot_network_t;

#if defined(ENABLE_TLS) || defined(ENABLE_DTLS)
#include "entropy.h"
#include "ctr_drbg.h"
#include "ssl.h"
#include "x509_crt.h"
#include "pk.h"
#include "net_sockets.h"
#include "timing.h"
#include "ssl_cookie.h"


/**
 * @brief TLS ���������Ϣ
 */
typedef struct tc_iot_tls_config_t {
    const char* root_ca_in_mem; /**< �ڴ����Ѽ��صĸ�֤��*/
    const char* root_ca_location; /**< �ļ�ϵͳ�ж�Ӧ��֤��·��*/
    const char* device_cert_location; /**< �豸֤��·��*/
    const char* device_private_key_location; /**< �豸˽Կ·��*/
    uint32_t timeout_ms; /**< TLS ��ʱ����*/
    char verify_server; /**< TLS ����ʱ���Ƿ���֤������֤�� */

#if defined(ENABLE_DTLS)
    unsigned char *psk;
    size_t         psk_len;
    unsigned char *psk_id;
    size_t         psk_id_len;
#endif

} tc_iot_tls_config_t;

/**< TLS��̬�������� */
typedef struct tc_iot_tls_data_t {
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl_context;
    mbedtls_ssl_config conf;
    uint32_t flags;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
    mbedtls_net_context ssl_fd;
    mbedtls_timing_delay_context delay_timer;
    mbedtls_ssl_cookie_ctx cookie_ctx;
} tc_iot_tls_data_t;

#endif

/**
 * @brief �������Ӳ�����ʼ����
 */
typedef struct {
    uint16_t use_tls; /**< �Ƿ���� TLS ��ʽͨѶ*/
    char* host; /**< �������������ַ*/
    uint16_t port; /**< �������˿�*/
    int fd; /**< ���Ӿ��fd */
    int is_connected; /**< �Ƿ�����������*/
    void * extra_context; /**< Ԥ���ֶΣ��û���ֲ����ƽ̨ʱ��������ָ���Զ���ṹ�壬��ȡ����ҵ������ */

#if defined(ENABLE_TLS) || defined(ENABLE_DTLS)
    tc_iot_tls_config_t tls_config; /**< TLS ����*/
#endif

} tc_iot_net_context_init_t;

/**
 * @brief �������Ӳ��������̻Ự����
 */
typedef struct {
    uint16_t use_tls; /**< �Ƿ���� TLS ��ʽͨѶ*/
    char* host; /**< �������������ַ*/
    uint16_t port; /**< �������˿�*/
    int fd; /**< ���Ӿ��fd */
    int is_connected; /**< �Ƿ�����������*/

    void * extra_context; /**< Ԥ���ֶΣ��û���ֲ����ƽ̨ʱ��������ָ���Զ���ṹ�壬��ȡ����ҵ������ */

#if defined(ENABLE_TLS) || defined(ENABLE_DTLS)
    tc_iot_tls_config_t tls_config; /**< TLS ����*/
    tc_iot_tls_data_t tls_data; /**< TLS ��������*/
#endif

} tc_iot_net_context_t;


/**
 * @brief �������Ӷ��󣬰�����ز������ص�����
 */
struct tc_iot_network_t {
    int (*do_read)(struct tc_iot_network_t* network, unsigned char* read_buf,
                   int read_buf_len, int timeout_ms); /**< ���նԶ����緢������*/
    int (*do_write)(struct tc_iot_network_t* network, const unsigned char* write_buf,
                    int write_buf_len, int timeout_ms); /**< ����ָ�����ݵ��Զ�����*/
    int (*do_connect)(tc_iot_network_t* network, const char* host, uint16_t port); /**< ����Զ�̷�����*/
    int (*do_disconnect)(tc_iot_network_t* network); /**< �Ͽ���������*/
    int (*is_connected)(tc_iot_network_t* network); /**< �ж������Ƿ�������*/
    int (*do_destroy)(tc_iot_network_t* network);/**< �����������Ӷ���*/

    tc_iot_net_context_t net_context; /**< �������Ӳ��������̻Ự����*/
} ;

int tc_iot_copy_net_context(tc_iot_net_context_t * dest, tc_iot_net_context_init_t * init);

/**
 * @brief tc_iot_hal_net_init ��ʼ�� TCP ���Ӷ���������ز����Ͷ�Ӧ�ص���
 *
 * @param network �������Ӷ���
 * @param net_context ���Ӳ���
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_net_init(tc_iot_network_t* network,
                        tc_iot_net_context_init_t* net_context);


/**
 * @brief tc_iot_hal_net_connect ���ӷ����
 *
 * @param network �������Ӷ���
 * @param host ������������IP��ַ
 * @param port �������˿�
 *
 * @return ���������
 *	�ɹ����� TC_IOT_SUCCESS (0)
 *	ʧ�ܷ��� TC_IOT_FAILURE(-90) ������������ԭ��(����)
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_net_connect(tc_iot_network_t* network, char* host,
                           uint16_t port);

/**
 * @brief tc_iot_hal_net_read ��������Զ˷��͵�����
 *
 * @param network �������Ӷ���
 * @param buffer ���ջ�����
 * @param len ���ջ�������С
 * @param timeout_ms ���ȴ�ʱ�ӣ���λms
 *
 * @return ����������ɹ���ȡ�ֽ���
 *	����timeout_ms��ʱ��ȡ��0�ֽ�, ���� TC_IOT_NET_NOTHING_READ
 *  ����timeout_ms��ʱ��ȡ�ֽ���û�дﵽ len , ����TC_IOT_NET_READ_TIMEOUT
 *  ����timeout_ms��ʱ�Զ˹ر�����, ���� ʵ�ʶ�ȡ�ֽ���
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_net_read(tc_iot_network_t* network, unsigned char* buffer,
                        int len, int timeout_ms);

/**
 * @brief tc_iot_hal_net_write �������ݵ�����Զ�
 *
 * @param network �������Ӷ���
 * @param buffer ���ͻ�����
 * @param len ���ͻ�������С
 * @param timeout_ms ����͵ȴ�ʱ�ӣ���λms
 *
 * @return ����������ɹ������ֽ���
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_net_write(tc_iot_network_t* network,const  unsigned char* buffer,
                         int len, int timeout_ms);


/**
 * @brief tc_iot_hal_net_is_connected �жϵ�ǰ�Ƿ��ѳɹ�������������
 *
 * @param network �������Ӷ���
 *
 * @return 1 ��ʾ�����ӣ�0 ��ʾδ����
 */
int tc_iot_hal_net_is_connected(tc_iot_network_t* network);


/**
 * @brief tc_iot_hal_net_disconnect �Ͽ���������
 *
 * @param network �������Ӷ���
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_net_disconnect(tc_iot_network_t* network);


/**
 * @brief tc_iot_hal_net_destroy �ͷ����������Դ
 *
 * @param network �������Ӷ���
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_net_destroy(tc_iot_network_t* network);

#if defined(ENABLE_TLS)

/**
 * @brief tc_iot_hal_tls_init ��ʼ�� TLS ��������
 *
 * @param network �������Ӷ���
 * @param net_context ���Ӳ���
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_tls_init(tc_iot_network_t* network,
                        tc_iot_net_context_init_t* net_context);

/**
 * @brief tc_iot_hal_tls_connect ���� TLS ����˲�����������ּ���֤
 *
 * @param network �������Ӷ���
 * @param host ������������IP��ַ
 * @param port �������˿�
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_tls_connect(tc_iot_network_t* network, const char* host,
                           uint16_t port);

/**
 * @brief tc_iot_hal_tls_read ���� TLS �Զ˷��͵�����
 *
 * @param network �������Ӷ���
 * @param buffer ���ջ�����
 * @param len ���ջ�������С
 * @param timeout_ms ���ȴ�ʱ�ӣ���λms
 *
 * @return ����������ɹ���ȡ�ֽ���
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_tls_read(tc_iot_network_t* network, unsigned char* buffer,
                        int len, int timeout_ms) ;

/**
 * @brief tc_iot_hal_tls_write �������ݵ� TLS �Զ�
 *
 * @param network �������Ӷ���
 * @param buffer ���ͻ�����
 * @param len ���ͻ�������С
 * @param timeout_ms ����͵ȴ�ʱ�ӣ���λms
 *
 * @return ����������ɹ������ֽ���
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_tls_write(tc_iot_network_t* network, const unsigned char* buffer,
                         int len, int timeout_ms);

/**
 * @brief tc_iot_hal_tls_is_connected �жϵ�ǰ�Ƿ��ѳɹ����� TLS ����
 *
 * @param network �������Ӷ���
 *
 * @return 1 ��ʾ�����ӣ�0 ��ʾδ����
 */
int tc_iot_hal_tls_is_connected(tc_iot_network_t* network);


/**
 * @brief tc_iot_hal_tls_disconnect �Ͽ� TLS ����
 *
 * @param network �������Ӷ���
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_tls_disconnect(tc_iot_network_t* network);


/**
 * @brief tc_iot_hal_tls_destroy �ͷ� TLS �����Դ
 *
 * @param network �������Ӷ���
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_tls_destroy(tc_iot_network_t* network);

#endif

#if defined(ENABLE_DTLS)

/**
 * @brief tc_iot_hal_dtls_init ��ʼ�� TLS ��������
 *
 * @param network �������Ӷ���
 * @param net_context ���Ӳ���
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_dtls_init(tc_iot_network_t* network,
                         tc_iot_net_context_init_t* net_context);

/**
 * @brief tc_iot_hal_dtls_connect ���� TLS ����˲�����������ּ���֤
 *
 * @param network �������Ӷ���
 * @param host ������������IP��ַ
 * @param port �������˿�
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_dtls_connect(tc_iot_network_t* network, const char* host,
                            uint16_t port);

/**
 * @brief tc_iot_hal_dtls_read ���� TLS �Զ˷��͵�����
 *
 * @param network �������Ӷ���
 * @param buffer ���ջ�����
 * @param len ���ջ�������С
 * @param timeout_ms ���ȴ�ʱ�ӣ���λms
 *
 * @return ����������ɹ���ȡ�ֽ���
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_dtls_read(tc_iot_network_t* network, unsigned char* buffer,
                         int len, int timeout_ms) ;

/**
 * @brief tc_iot_hal_dtls_write �������ݵ� TLS �Զ�
 *
 * @param network �������Ӷ���
 * @param buffer ���ͻ�����
 * @param len ���ͻ�������С
 * @param timeout_ms ����͵ȴ�ʱ�ӣ���λms
 *
 * @return ����������ɹ������ֽ���
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_dtls_write(tc_iot_network_t* network, const unsigned char* buffer,
                          int len, int timeout_ms);

/**
 * @brief tc_iot_hal_dtls_is_connected �жϵ�ǰ�Ƿ��ѳɹ����� TLS ����
 *
 * @param network �������Ӷ���
 *
 * @return 1 ��ʾ�����ӣ�0 ��ʾδ����
 */
int tc_iot_hal_dtls_is_connected(tc_iot_network_t* network);


/**
 * @brief tc_iot_hal_dtls_disconnect �Ͽ� TLS ����
 *
 * @param network �������Ӷ���
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_dtls_disconnect(tc_iot_network_t* network);


/**
 * @brief tc_iot_hal_dtls_destroy �ͷ� TLS �����Դ
 *
 * @param network �������Ӷ���
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_dtls_destroy(tc_iot_network_t* network);

#endif

/**
 * @brief tc_iot_hal_udp_init ��ʼ�� TCP ���Ӷ���������ز����Ͷ�Ӧ�ص���
 *
 * @param network �������Ӷ���
 * @param net_context ���Ӳ���
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_udp_init(tc_iot_network_t* network,
                        tc_iot_net_context_init_t* net_context);


/**
 * @brief tc_iot_hal_udp_connect ���ӷ����
 *
 * @param network �������Ӷ���
 * @param host ������������IP��ַ
 * @param port �������˿�
 *
 * @return ���������
 *	�ɹ����� TC_IOT_SUCCESS (0)
 *	ʧ�ܷ��� TC_IOT_FAILURE(-90) ������������ԭ��(����)
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_udp_connect(tc_iot_network_t* network, const char* host, uint16_t port);
//void *tc_iot_hal_udp_create(char *host, unsigned short port);


/**
 * @brief tc_iot_hal_udp_read ��������Զ˷��͵�����
 *
 * @param network �������Ӷ���
 * @param buffer ���ջ�����
 * @param len ���ջ�������С
 * @param timeout_ms ���ȴ�ʱ�ӣ���λms
 *
 * @return ����������ɹ���ȡ�ֽ���
 *	����timeout_ms��ʱ��ȡ��0�ֽ�, ���� TC_IOT_NET_NOTHING_READ
 *  ����timeout_ms��ʱ��ȡ�ֽ���û�дﵽ len , ����TC_IOT_NET_READ_TIMEOUT
 *  ����timeout_ms��ʱ�Զ˹ر�����, ���� ʵ�ʶ�ȡ�ֽ���
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_udp_read(tc_iot_network_t* network, unsigned char* buffer, int len, int timeout_ms);
//int tc_iot_hal_udp_read(void *p_socket,unsigned char *buffer, unsigned int datalen);
//int tc_iot_hal_udp_readTimeout(void *p_socket, unsigned char *buffer, unsigned int datalen, unsigned int timeout);


/**
 * @brief tc_iot_hal_udp_write �������ݵ�����Զ�
 *
 * @param network �������Ӷ���
 * @param buffer ���ͻ�����
 * @param len ���ͻ�������С
 * @param timeout_ms ����͵ȴ�ʱ�ӣ���λms
 *
 * @return ����������ɹ������ֽ���
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_udp_write(tc_iot_network_t* network,const  unsigned char* buffer,  int len, int timeout_ms);
//int tc_iot_hal_udp_write(void *p_socket, const unsigned char *buffer, unsigned int datalen);



/**
 * @brief tc_iot_hal_udp_is_connected �жϵ�ǰ�Ƿ��ѳɹ�������������
 *
 * @param network �������Ӷ���
 *
 * @return 1 ��ʾ�����ӣ�0 ��ʾδ����
 */
int tc_iot_hal_udp_is_connected(tc_iot_network_t* network);


/**
 * @brief tc_iot_hal_udp_disconnect �Ͽ���������
 *
 * @param network �������Ӷ���
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
//int tc_iot_hal_udp_disconnect(tc_iot_network_t* network);
void tc_iot_hal_udp_close(void *p_socket);


/**
 * @brief tc_iot_hal_udp_destroy �ͷ����������Դ
 *
 * @param network �������Ӷ���
 *
 * @return ���������
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_udp_destroy(tc_iot_network_t* network);

int tc_iot_network_prepare(tc_iot_network_t * p_network, tc_iot_network_type type, tc_iot_network_protocol proto, bool over_tls, void * extra_options);

#endif /* end of include guard */
