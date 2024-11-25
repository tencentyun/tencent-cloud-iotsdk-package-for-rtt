#include "tc_iot_inc.h"

int tc_iot_create_mqapi_rpc_json(char* form, int max_form_len,
                                 const char* secret,
                                 const char* device_name,
                                 const char* message,
                                 long nonce,
                                 const char* product_id,
                                 long timestamp
    ) {
    unsigned char sha256_digest[TC_IOT_SHA256_DIGEST_SIZE];
    int ret;
    char b64_buf[TC_IOT_BASE64_ENCODE_OUT_LEN(TC_IOT_SHA256_DIGEST_SIZE)];
    tc_iot_json_writer writer;
    tc_iot_json_writer * w = &writer;

    IF_NULL_RETURN(form, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(secret, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(message, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(device_name, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(product_id, TC_IOT_NULL_POINTER);

    ret = tc_iot_calc_sign(
        sha256_digest, sizeof(sha256_digest),
        secret,
        "deviceName=%s&message=%s&nonce=%d&productId=%s&timestamp=%d",
        device_name,message,nonce,product_id,timestamp
        );

    ret = tc_iot_base64_encode((unsigned char *)sha256_digest, sizeof(sha256_digest), b64_buf,
                               sizeof(b64_buf));
    if (ret < sizeof(b64_buf) && ret > 0) {
        b64_buf[ret] = '\0';
        tc_iot_mem_usage_log("b64_buf", sizeof(b64_buf), ret);
    } else {
        TC_IOT_LOG_ERROR("b64_buf for sha256 digest overflow, ret=%d.", ret);
        if (ret < 0) {
            return ret;
        } else {
            return TC_IOT_BUFFER_OVERFLOW;
        }
    }
    tc_iot_json_writer_open(w, form, max_form_len);
    tc_iot_json_writer_string(w ,"productId", product_id);
    tc_iot_json_writer_string(w ,"deviceName", device_name);
    tc_iot_json_writer_int(w ,"nonce", nonce);
    tc_iot_json_writer_int(w ,"timestamp", timestamp);
    tc_iot_json_writer_string(w ,"message", message);
    tc_iot_json_writer_string(w ,"signature", b64_buf);
    ret = tc_iot_json_writer_close(w);

    return ret;
}

int tc_iot_http_mqapi_rpc( char * result, int result_len,
                           const char* api_url, char* root_ca_path, long timestamp, long nonce,
                           tc_iot_device_info* p_device_info, const char * message) {

    char sign_out[TC_IOT_HTTP_MQAPI_REQUEST_FORM_LEN];
    char http_buffer[TC_IOT_HTTP_MQAPI_RESPONSE_LEN];
    int sign_len;
    int ret;
    char* rsp_body;
    tc_iot_http_client *p_http_client, http_client;

    jsmn_parser p;
    jsmntok_t t[20];

    char temp_buf[TC_IOT_HTTP_MAX_URL_LENGTH];
    int r = 0;
    int json_field_index = 0;
    const int timeout_ms = TC_IOT_API_TIMEOUT_MS;
    bool secured = false;
    uint16_t port = HTTP_DEFAULT_PORT;
#if defined(ENABLE_TLS)
    secured = true;
    port = HTTPS_DEFAULT_PORT;
#endif

    ret = tc_iot_create_mqapi_rpc_json( sign_out, sizeof(sign_out),
                                        p_device_info->secret,
                                        p_device_info->device_name,
                                        message,
                                        nonce,
                                        p_device_info->product_id,
                                        timestamp);
    sign_len = ret;
    tc_iot_mem_usage_log("sign_out[TC_IOT_HTTP_TOKEN_REQUEST_FORM_LEN]", sizeof(sign_out), sign_len);

    TC_IOT_LOG_TRACE("signed request form:\n%s", sign_out);
    tc_iot_hal_snprintf(temp_buf, sizeof(temp_buf), TC_IOT_API_HOST_FORMAT, p_device_info->region);	
	

    p_http_client = &http_client;
    tc_iot_http_client_init(p_http_client, HTTP_POST);
    tc_iot_http_client_set_body(p_http_client, sign_out);
    tc_iot_http_client_set_host(p_http_client, temp_buf);
    tc_iot_http_client_set_abs_path(p_http_client, TC_IOT_API_RPC_PATH);
    tc_iot_http_client_set_content_type(p_http_client, HTTP_CONTENT_JSON);

    tc_iot_http_client_format_buffer(http_buffer, sizeof(http_buffer), p_http_client);

    TC_IOT_LOG_TRACE("http_buffer:\n%s", http_buffer);
    ret = tc_iot_http_client_perform(http_buffer,strlen(http_buffer), sizeof(http_buffer),
                                     temp_buf, port, secured, timeout_ms);
    tc_iot_mem_usage_log("http_buffer[TC_IOT_HTTP_TOKEN_RESPONSE_LEN]", sizeof(http_buffer), strlen(http_buffer));

    if (ret < 0) {
        return ret;
    }

    rsp_body = http_buffer;

    if (rsp_body) {
        jsmn_init(&p);
        r = jsmn_parse(&p, rsp_body, strlen(rsp_body), t,
                       sizeof(t) / sizeof(t[0]));
        if (r < 0) {
            TC_IOT_LOG_ERROR("Failed to parse JSON: %s", rsp_body);
            return TC_IOT_JSON_PARSE_FAILED;
        }

        if (r < 1 || t[0].type != JSMN_OBJECT) {
            TC_IOT_LOG_ERROR("Invalid JSON: %s", rsp_body);
            return TC_IOT_JSON_PARSE_FAILED;
        }

        json_field_index = tc_iot_json_find_token(rsp_body, t, r, "returnCode",
                                                  temp_buf, sizeof(temp_buf));
        if (json_field_index <= 0 || strlen(temp_buf) != 1 ||
            temp_buf[0] != '0') {
            TC_IOT_LOG_ERROR("failed to fetch token %d/%s: %s", json_field_index,
                             temp_buf, rsp_body);
            return TC_IOT_HTTP_RPC_FAILED;
        }

        json_field_index = tc_iot_json_find_token(rsp_body, t, r, "data.message",
                                                  result, result_len);
        if (json_field_index <= 0) {
            TC_IOT_LOG_ERROR("failed to fetch token %d/%s: %s", json_field_index,
                             temp_buf, rsp_body);
            return TC_IOT_HTTP_RPC_FAILED;
        }

        return strlen(result);
    } else {
        return TC_IOT_ERROR_HTTP_REQUEST_FAILED;
    }
}
