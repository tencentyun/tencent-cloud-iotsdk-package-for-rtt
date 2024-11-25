#include "tc_iot_inc.h"

int tc_iot_add_url_encoded_field(tc_iot_yabuffer_t* buffer,
                                        const char* prefix, const char* val,
                                        int val_len) {
    int total = 0;
    int ret = 0;

    IF_NULL_RETURN(buffer, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(prefix, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(val, TC_IOT_NULL_POINTER);

    total = tc_iot_yabuffer_append(buffer, prefix);
    ret = tc_iot_url_encode(val, val_len, tc_iot_yabuffer_current(buffer),
                            tc_iot_yabuffer_left(buffer));

    tc_iot_yabuffer_forward(buffer, ret);
    total += ret;
    return total;
}

int tc_iot_add_url_uint_field(tc_iot_yabuffer_t* buffer, const char* prefix,
                              unsigned int val) {
    int total = 0;
    int ret;
    char* current;
    int buffer_left;

    IF_NULL_RETURN(buffer, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(prefix, TC_IOT_NULL_POINTER);

    total = tc_iot_yabuffer_append(buffer, prefix);
    current = tc_iot_yabuffer_current(buffer);
    buffer_left = tc_iot_yabuffer_left(buffer);

    ret = tc_iot_hal_snprintf(current, buffer_left, "%u", val);

    if (ret > 0) {
        tc_iot_yabuffer_forward(buffer, ret);
        total += ret;
        return total;
    } else {
        return TC_IOT_BUFFER_OVERFLOW;
    }
}
int tc_iot_create_query_request_form(char* form, int max_form_len,
                                    const char* product_id) {
    tc_iot_yabuffer_t form_buf;
    int total = 0;

    IF_NULL_RETURN(form, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(product_id, TC_IOT_NULL_POINTER);

    tc_iot_yabuffer_init(&form_buf, form, max_form_len);
    total += tc_iot_add_url_encoded_field(&form_buf, "productId=", product_id,
                                          strlen(product_id));
    return total;
}

int tc_iot_create_auth_request_form(char* form, int max_form_len,
                                    const char* secret,
                                    const char* client_id,
                                    const char* device_name,
                                    unsigned int expire,
                                    unsigned int nonce,
                                    const char* product_id,
                                    unsigned int timestamp) {
    tc_iot_yabuffer_t form_buf;
    int total = 0;

    IF_NULL_RETURN(form, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(secret, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(client_id, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(device_name, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(product_id, TC_IOT_NULL_POINTER);

    tc_iot_yabuffer_init(&form_buf, form, max_form_len);
    total += tc_iot_add_url_encoded_field(&form_buf, "clientId=", client_id,
                                          strlen(client_id));
    total += tc_iot_add_url_encoded_field(&form_buf, "&deviceName=",
                                          device_name, strlen(device_name));
    total += tc_iot_add_url_uint_field(&form_buf, "&expire=", expire);
    total += tc_iot_add_url_uint_field(&form_buf, "&nonce=", nonce);
    total += tc_iot_add_url_encoded_field(&form_buf, "&productId=", product_id,
                                          strlen(product_id));
    total += tc_iot_add_url_uint_field(&form_buf, "&timestamp=", timestamp);
    total += tc_iot_add_url_encoded_field(&form_buf, "&signature=", "", 0);

    total += tc_iot_calc_auth_sign(
        tc_iot_yabuffer_current(&form_buf), tc_iot_yabuffer_left(&form_buf),
        secret, client_id, device_name,
        expire, nonce, product_id, timestamp);
    return total;
}

int tc_iot_create_active_device_form(char* form, int max_form_len,
                                    const char* product_secret,
                                    const char* device_name,
                                    const char* product_id,
                                    unsigned int nonce, unsigned int timestamp) {
    tc_iot_yabuffer_t form_buf;
    int total = 0;

    IF_NULL_RETURN(form, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(product_secret, TC_IOT_NULL_POINTER);

    IF_NULL_RETURN(device_name, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(product_id, TC_IOT_NULL_POINTER);

    tc_iot_yabuffer_init(&form_buf, form, max_form_len);

    total += tc_iot_add_url_encoded_field(&form_buf, "productId=", product_id,
                                      strlen(product_id));
    total += tc_iot_add_url_encoded_field(&form_buf, "&deviceName=",
                                          device_name, strlen(device_name));

    total += tc_iot_add_url_uint_field(&form_buf, "&nonce=", nonce);

    total += tc_iot_add_url_uint_field(&form_buf, "&timestamp=", timestamp);
    total += tc_iot_add_url_encoded_field(&form_buf, "&signature=", "", 0);

    total += tc_iot_calc_active_device_sign(
        tc_iot_yabuffer_current(&form_buf), tc_iot_yabuffer_left(&form_buf),
        product_secret,
        device_name, product_id,
        nonce, timestamp);
    return total;
}
