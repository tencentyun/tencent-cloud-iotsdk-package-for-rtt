#include "tc_iot_inc.h"

int tc_iot_ota_download(const char* api_url, int partial_start, tc_iot_http_response_callback download_callback, const void * context) {
    tc_iot_network_t network;
    char http_buffer[TC_IOT_HTTP_OTA_REQUEST_LEN];
    char host[TC_IOT_HTTP_MAX_URL_LENGTH];
    int ret;
    int timeout_ms = 2000;
    tc_iot_http_response_parser parser;
    uint16_t port = HTTP_DEFAULT_PORT;
    bool secured = false;
    tc_iot_http_client *p_http_client, http_client;
    tc_iot_url_parse_result_t result;

    IF_NULL_RETURN(api_url, TC_IOT_NULL_POINTER);

    TC_IOT_LOG_TRACE("request url=%s", api_url);
    ret = tc_iot_url_parse(api_url, strlen(api_url), &result);
    if (ret < 0 ) {
        TC_IOT_LOG_ERROR("parse url failed: %s", api_url);
        return TC_IOT_INVALID_PARAMETER;
    }

    if (result.host_len >= sizeof(host)) {
        TC_IOT_LOG_ERROR("host buffer not enough: host len=%d,buffer len=%d", result.host_len, (int)sizeof(host));
        return TC_IOT_INVALID_PARAMETER;
    }

    memcpy(host, api_url+result.host_start, result.host_len);
    host[result.host_len] = '\0';
    port = result.port;
    secured = result.over_tls;

    p_http_client = &http_client;
    tc_iot_http_client_init(p_http_client, HTTP_GET);
    tc_iot_http_client_set_host(p_http_client, host);
    tc_iot_http_client_set_abs_path(p_http_client, api_url+result.path_start);
    tc_iot_http_client_set_content_type(p_http_client, HTTP_CONTENT_FORM_URLENCODED);

    tc_iot_http_client_format_buffer((char *)http_buffer, sizeof(http_buffer), p_http_client);

    TC_IOT_LOG_TRACE("http_buffer=%s", http_buffer);
    ret = tc_iot_http_client_internal_perform(http_buffer, strlen(http_buffer), sizeof(http_buffer),
                                              &network,&parser, 
                                              host, port,secured, timeout_ms, download_callback, context);
    if (ret < 0) {
        return ret;
    }

    tc_iot_mem_usage_log("http_buffer[TC_IOT_HTTP_TOKEN_RESPONSE_LEN]", sizeof(http_buffer), strlen(http_buffer));
    TC_IOT_LOG_TRACE("content length = %d", parser.content_length);

    return ret;
}

int tc_iot_ota_request_content_length(const char* api_url) {
    tc_iot_network_t network;
    char http_buffer[TC_IOT_HTTP_OTA_REQUEST_LEN];
    char host[TC_IOT_HTTP_MAX_URL_LENGTH];
    int ret;
    int timeout_ms = 2000;
    tc_iot_http_response_parser parser;
    uint16_t port = HTTP_DEFAULT_PORT;
    bool secured = false;
    tc_iot_http_client *p_http_client, http_client;
    tc_iot_url_parse_result_t result;

    IF_NULL_RETURN(api_url, TC_IOT_NULL_POINTER);

    TC_IOT_LOG_TRACE("request url=%s", api_url);
    ret = tc_iot_url_parse(api_url, strlen(api_url), &result);
    if (ret < 0 ) {
        TC_IOT_LOG_ERROR("parse url failed: %s", api_url);
        return TC_IOT_INVALID_PARAMETER;
    }

    if (result.host_len >= sizeof(host)) {
        TC_IOT_LOG_ERROR("host buffer not enough: host len=%d,buffer len=%d", result.host_len, (int)sizeof(host));
        return TC_IOT_INVALID_PARAMETER;
    }

    memcpy(host, api_url+result.host_start, result.host_len);
    host[result.host_len] = '\0';
    port = result.port;
    secured = result.over_tls;

    p_http_client = &http_client;
    tc_iot_http_client_init(p_http_client, HTTP_HEAD);
    tc_iot_http_client_set_host(p_http_client, host);
    tc_iot_http_client_set_abs_path(p_http_client, api_url+result.path_start);
    tc_iot_http_client_set_content_type(p_http_client, HTTP_CONTENT_FORM_URLENCODED);

    tc_iot_http_client_format_buffer((char *)http_buffer, sizeof(http_buffer), p_http_client);

    TC_IOT_LOG_TRACE("http_buffer=%s", http_buffer);
    ret = tc_iot_http_client_internal_perform(http_buffer, strlen(http_buffer), sizeof(http_buffer),
                                              &network,&parser, 
                                              host, port,secured, timeout_ms, NULL, NULL);
    if (ret < 0) {
        return ret;
    }

    tc_iot_mem_usage_log("http_buffer[TC_IOT_HTTP_TOKEN_RESPONSE_LEN]", sizeof(http_buffer), strlen(http_buffer));
    TC_IOT_LOG_TRACE("content length = %d", parser.content_length);

    return parser.content_length;
}


