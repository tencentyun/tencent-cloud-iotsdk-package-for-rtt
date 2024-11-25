#ifndef TC_IOT_HTTP_CLIENT_H
#define TC_IOT_HTTP_CLIENT_H

#include "tc_iot_inc.h"

#define HTTP_VERSION_1_0 "1.0"
#define HTTP_VERSION_1_1 "1.1"

#define HTTP_PUT "PUT"
#define HTTP_HEAD "HEAD"
#define HTTP_POST "POST"
#define HTTP_GET "GET"

#define HTTP_SPLIT_STR "\r\n"
#define HTTP_REQUEST_LINE_FMT ("%s %s HTTP/%s" HTTP_SPLIT_STR)
#define HTTP_HEADER_FMT "%s: %s\r\n"
#define HTTP_HEADER_HOST "Host"
#define HTTP_HEADER_ACCEPT "Accept"
#define HTTP_HEADER_ACCEPT_ENCODING "Accept-Encoding"
#define HTTP_HEADER_USER_AGENT "User-Agent"
#define HTTP_HEADER_CONTENT_LENGTH "Content-Length"
#define HTTP_HEADER_LOCATION "Location"
#define HTTP_HEADER_CONTENT_TYPE "Content-Type"
#define HTTP_HEADER_TRANSFER_ENCODING "Transfer-Encoding"
#define HTTP_CONTENT_FORM_URLENCODED "application/x-www-form-urlencoded"
#define HTTP_CONTENT_OCTET_STREAM "application/octet-stream"
#define HTTP_CONTENT_FORM_DATA "multipart/form-data"
#define HTTP_CONTENT_JSON "application/json"

#define HTTPS_PREFIX "https"
#define HTTPS_PREFIX_LEN (sizeof(HTTPS_PREFIX) - 1)


/* examples: */
/* HTTP/1.0 200 OK */
/* HTTP/1.1 404 Not Found */
/* HTTP/1.1 503 Service Unavailable */
#define HTTP_RESPONSE_STATE_PREFIX "HTTP/1."
#define HTTP_RESPONSE_STATE_PREFIX_LEN (sizeof(HTTP_RESPONSE_STATE_PREFIX)-1)


typedef int (*tc_iot_http_response_callback)(const void * context, const char * data, int data_len, int offset, int total);

typedef enum _tc_iot_http_response_parse_state {
    _PARSER_START,
    _PARSER_VERSION,
    _PARSER_IGNORE_TO_RETURN_CHAR,
    _PARSER_SKIP_NEWLINE_CHAR,
    _PARSER_HEADER,
    _PARSER_IGNORE_TO_BODY_START,
    _PARSER_END,
} tc_iot_http_response_parse_state;

typedef enum _tc_iot_http_transfer_encoding {
    TRANSFER_ENCODING_IDENTITY,
    TRANSFER_ENCODING_CHUNKED,
    TRANSFER_ENCODING_GZIP,
    TRANSFER_ENCODING_DEFLATE,
    TRANSFER_ENCODING_COMPRESS,
}tc_iot_http_transfer_encoding;

typedef struct _tc_iot_http_response_parser {
    tc_iot_http_response_parse_state state;
    char  version;
    short status_code;
    int  content_length;
    int  transfer_encoding;
    const char * location;
    // const char * body;
}tc_iot_http_response_parser;

void tc_iot_http_parser_init(tc_iot_http_response_parser * parser);
int tc_iot_http_parser_analysis(tc_iot_http_response_parser * parser, const char * buffer, int buffer_len);

typedef struct _tc_iot_http_client {
    const char * method; // http method: get, post, put, head ...
    const char * version; //  http version: 1.0, 1.1 ...
    const char * host; // request host: www.example.com
    const char * content_type;
    const char * abs_path; // url absolute path: /absolute/path
    const char * extra_headers; // extra headers: Range, Content-Type ...

    const char * body; // http request body
} tc_iot_http_client;

int tc_iot_http_client_init(tc_iot_http_client * c, const char * method);
int tc_iot_http_client_set_version(tc_iot_http_client * c, const char * version);
int tc_iot_http_client_set_host(tc_iot_http_client * c, const char * host);
int tc_iot_http_client_set_abs_path(tc_iot_http_client * c, const char * abs_path);
int tc_iot_http_client_set_content_type(tc_iot_http_client * c, const char * content_type);
int tc_iot_http_client_set_extra_headers(tc_iot_http_client * c, const char * extra_headers);
int tc_iot_http_client_set_body(tc_iot_http_client * c, const char * body);

int tc_iot_http_client_format_buffer(char * buffer, int buffer_len, tc_iot_http_client * c);
int tc_iot_http_client_internal_perform(char * buffer, int buffer_used, int buffer_len,
                                        tc_iot_network_t * p_network,tc_iot_http_response_parser * p_parser,
                                        const char * host, uint16_t port,
                                        bool secured, int timeout_ms, tc_iot_http_response_callback resp_callback, const void * callback_context);
int tc_iot_http_client_perform(char * buffer, int buffer_used, int buffer_len,
                               const char * host, uint16_t port,
                               bool secured, int timeout_ms);

#define TC_IOT_USER_AGENT "tciotclient/1.0"
#define TC_IOT_HTTP_MAX_URL_LENGTH     128
#define TC_IOT_HTTP_MAX_HOST_LENGTH    128

#endif /* TC_IOT_HTTP_CLIENT_H */
