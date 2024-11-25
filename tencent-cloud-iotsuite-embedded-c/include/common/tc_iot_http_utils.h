#ifndef IOT_HTTP_UTILS_H
#define IOT_HTTP_UTILS_H

#include "tc_iot_inc.h"


/**
 * @brief 计算 Token 请求签名
 *
 * @param sign_out 签名( Base64 编码)结果
 * @param max_sign_len 签名( Bsse64 编码)结果区长度
 * @param secret 签名密钥
 * @param client_id Client Id
 * @param device_name Device Name
 * @param expire Token有效期
 * @param nonce 随机数
 * @param product_id Product Id
 * @param timestamp 时间戳
 *
 * @return >=0 签名结果实际长度，<0 错误码
 * @see tc_iot_sys_code_e
 */
int tc_iot_calc_auth_sign(char* sign_out, int max_sign_len, const char* secret,
                          const char* client_id,
                          const char* device_name,
                          unsigned int expire, unsigned int nonce,
                          const char* product_id, 
                          unsigned int timestamp);


/**
 * @brief 构造 Token HTTP 签名请求 form
 *
 * @param form 结果缓存区
 * @param max_form_len 结果缓存区最大大小
 * @param secret 签名密钥
 * @param client_id Client Id
 * @param device_name Device Name
 * @param expire Token有效期
 * @param nonce 随机数
 * @param product_id Product Id
 * @param timestamp 时间戳
 *
 * @return >=0 签名结果实际长度，<0 错误码
 * @see tc_iot_sys_code_e
 */
int tc_iot_create_auth_request_form(char* form,  int max_form_len,
                                    const char* secret, 
                                    const char* client_id, 
                                    const char* device_name,
                                    unsigned int expire,
                                    unsigned int nonce,
                                    const char* product_id,
                                    unsigned int timestamp);

/**
 * @brief 构造 get device 设备激活 HTTP 签名请求 form
 *
 * @param form 结果缓存区
 * @param max_form_len 结果缓存区最大大小
 * @param secret 签名密钥, 请使用控制台的 product password
 * @param device_name Device Name
 * @param product_id Product Id , 例子 : "iot-dalqbv1g"	
 * @param nonce 随机数
 * @param timestamp 时间戳 *
 * @return >=0 签名结果实际长度，<0 错误码
 * @see tc_iot_sys_code_e
 */
int tc_iot_create_active_device_form(char* form, int max_form_len,
                                     const char* product_secret, 
                                     const char* device_name,  
                                     const char* product_id,
                                     unsigned int nonce, unsigned int timestamp);
/**
 * @brief 构造 query 请求云端配置接口 HTTP form
 *
 * @param form 结果缓存区
 * @param max_form_len 结果缓存区最大大小
 * @param product_id Product Id , 例子 : "iot-dalqbv1g"	
 * @return >=0 签名结果实际长度，<0 错误码
 * @see tc_iot_sys_code_e
 */
int tc_iot_create_query_request_form(char* form, int max_form_len,
                                     const char* product_id);
int tc_iot_add_url_encoded_field(tc_iot_yabuffer_t* buffer,
                                        const char* prefix, const char* val,
                                 int val_len);
int tc_iot_add_url_uint_field(tc_iot_yabuffer_t* buffer, const char* prefix,
                       unsigned int val);
#endif /* end of include guard */
