#include "tc_iot_inc.h"

int tc_iot_get_device_secret(const char* api_url, char* root_ca_path, long timestamp, long nonce,
        tc_iot_device_info* p_device_info) {
    char sign_out[TC_IOT_HTTP_ACTIVE_REQUEST_FORM_LEN];
    char http_buffer[TC_IOT_HTTP_ACTIVE_RESPONSE_LEN];
    int sign_len;
    int ret;
    char* rsp_body;
    tc_iot_http_client *p_http_client, http_client;

    jsmn_parser p;
    jsmntok_t t[20];

    char temp_buf[TC_IOT_HTTP_MAX_URL_LENGTH];
    int returnCodeIndex = 0;
    int password_index;
    int r;
    const int timeout_ms = TC_IOT_API_TIMEOUT_MS;
    bool secured = false;
    uint16_t port = HTTP_DEFAULT_PORT;
#if defined(ENABLE_TLS)
    secured = true;
    port = HTTPS_DEFAULT_PORT;
#endif

    sign_len = tc_iot_create_active_device_form(
        sign_out, sizeof(sign_out), p_device_info->password,
        p_device_info->device_name,
        p_device_info->product_id,
        nonce, timestamp);

    tc_iot_mem_usage_log("sign_out[TC_IOT_HTTP_ACTIVE_REQUEST_FORM_LEN]", sizeof(sign_out), sign_len);

    if (sign_len < sizeof(sign_out)) {
        sign_out[sign_len] = '\0';
        TC_IOT_LOG_TRACE("signed request form:\n%s", sign_out);
    }

    TC_IOT_LOG_TRACE("signed request form:\n%s", sign_out);

    tc_iot_hal_snprintf(temp_buf, sizeof(temp_buf), TC_IOT_API_HOST_FORMAT, p_device_info->region);

    p_http_client = &http_client;
    tc_iot_http_client_init(p_http_client, HTTP_POST);
    tc_iot_http_client_set_body(p_http_client, sign_out);
    tc_iot_http_client_set_host(p_http_client, temp_buf);
    tc_iot_http_client_set_abs_path(p_http_client, TC_IOT_API_TOKEN_PATH);
    tc_iot_http_client_set_content_type(p_http_client, HTTP_CONTENT_FORM_URLENCODED);

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
        TC_IOT_LOG_TRACE("\nbody:\n%s\n", rsp_body);

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

        returnCodeIndex = tc_iot_json_find_token(rsp_body, t, r, "returnCode",
                                                 temp_buf, sizeof(temp_buf));
        if (returnCodeIndex <= 0 || strlen(temp_buf) != 1 ||
            temp_buf[0] != '0') {
            TC_IOT_LOG_ERROR("failed to fetch secrect %d/%s: %s", returnCodeIndex,
                      temp_buf, rsp_body);
            return TC_IOT_REFRESH_TOKEN_FAILED;
        }
        /*
        username_index = tc_iot_json_find_token(rsp_body, t, r, "data.device_name",
                                                p_device_info->username,
                                                TC_IOT_MAX_USER_NAME_LEN);
        if (username_index <= 0) {
            TC_IOT_LOG_TRACE("data.id not found in response.");
            return TC_IOT_REFRESH_TOKEN_FAILED;
        }
        */
        password_index = tc_iot_json_find_token(rsp_body, t, r, "data.device_secret",
                                                p_device_info->secret,
                                                sizeof(p_device_info->secret));
        if (password_index <= 0) {
            TC_IOT_LOG_TRACE("data.device_secret not found in response.");
            return TC_IOT_REFRESH_TOKEN_FAILED;
        }


        return TC_IOT_SUCCESS;
    } else {
        return TC_IOT_ERROR_HTTP_REQUEST_FAILED;
    }
}
