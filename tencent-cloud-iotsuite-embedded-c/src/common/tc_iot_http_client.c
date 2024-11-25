#include "tc_iot_inc.h"

/**
 * @brief    判断当前位置开始之后的字符串，是否有回车符，即剩余待处理数据是否包含完整的 HTTP Header。
 *
 * @param    str 待判断的字符串
 *
 * @return   bool true 表示有回车符；false 表示无回车符
 */
static bool tc_iot_http_has_line_ended(const char * str) {
    while (*str) {
        if (*str == '\r') {
            return true;
        }
        str ++;
    }
    return false;
}

/**
 * @brief    初始化 HTTP 响应解析结构体。
 *
 * @details  置初始状态。
 *
 * @param    parser 待初始化的结构体指针。
 *
 * @return   无
 */
void tc_iot_http_parser_init(tc_iot_http_response_parser * parser) {
    if (parser) {
        parser->state = _PARSER_START;
        parser->version = 0;
        parser->status_code = 0;
        parser->content_length = 0;
        parser->transfer_encoding = TRANSFER_ENCODING_IDENTITY;
        parser->location = NULL;
    }
}

bool tc_iot_http_is_complete_chunk(char * buffer, int buffer_len) {
    int num_char_count = 0;
    int chunk_size = 0;
    char * pos = buffer;

    IF_NULL_RETURN(buffer, TC_IOT_NULL_POINTER);

    while (pos <= (buffer+buffer_len)) {
       /*  Chunked-Body   = *chunk */
       /*                  last-chunk */
       /*                  trailer */
       /*                  CRLF */

       /* chunk          = chunk-size [ chunk-extension ] CRLF */
       /*                  chunk-data CRLF */
       /* chunk-size     = 1*HEX */
       /* last-chunk     = 1*("0") [ chunk-extension ] CRLF */
        num_char_count = 0;
        chunk_size = tc_iot_try_parse_hex(pos, &num_char_count);
        if (chunk_size == 0 && num_char_count == 1) {
            TC_IOT_LOG_TRACE("last chunk found.");
            return true;
        }

        TC_IOT_LOG_TRACE("chunk size = %d", chunk_size);

        // +2 means should contain CRLF followed by the chunk-data
        if (buffer+buffer_len < pos + chunk_size+2) {
            return false;
        }
        pos += num_char_count + 2; // skip chunk header
        pos += chunk_size + 2; // skip chunk data
    }

    return false;
}

/**
 * @brief    解析 HTTP 响应的 chunked 格式数据。
 *
 * @details  根据 buffer 输入数据，进行解析，合并掉 chunk size 数据，仅保留合并后的数据。
 *
 * @param    buffer 输入及输出缓存区
 *
 * @return   已处理的长度：
 *              1.当接收的包是完整的包时，返回值一般等于 buffer_len，表示全部都已解析处理;
 *              2.当接收的包不是完整的包时，返回值为已处理部分的长度，一般小于 buffer_len，
 *                表示已处理部分的长度，剩余部分未处理数据，需要接收更多数据后，合并后再次调用本函数解析；
 *              3.数据不满足解析要求，无法解析，需要继续接收更多内容时，返回值为 0，调用端处理逻辑同 2；
 *              4.出现异常，无法处理时，返回错误码，取值 < 0。
 * @see tc_iot_sys_code_e
 */

int tc_iot_http_merge_chunk(char * buffer, int buffer_len, int * total_chunk_size, bool * chunk_ended) {
    int num_char_count = 0;
    int chunk_size = 0;
    char * pos = buffer;

    IF_NULL_RETURN(buffer, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(total_chunk_size, TC_IOT_NULL_POINTER);

    *total_chunk_size = 0;

    while (pos <= (buffer+buffer_len)) {
       /*  Chunked-Body   = *chunk */
       /*                  last-chunk */
       /*                  trailer */
       /*                  CRLF */

       /* chunk          = chunk-size [ chunk-extension ] CRLF */
       /*                  chunk-data CRLF */
       /* chunk-size     = 1*HEX */
       /* last-chunk     = 1*("0") [ chunk-extension ] CRLF */
        num_char_count = 0;
        chunk_size = tc_iot_try_parse_hex(pos, &num_char_count);
        pos += num_char_count + 2;
        if (chunk_size == 0 && num_char_count == 1) {
            *chunk_ended = true;
            pos += 2; // last chunk has CRLF too.
            return pos-buffer;
        }

        // +2 means should contain CRLF followed by the chunk-data
        if (buffer+buffer_len < pos + chunk_size+2) {
            TC_IOT_LOG_ERROR("chunk not complete:buffer_len=%d, last chunk_size=%d", buffer_len, chunk_size);
            *chunk_ended = false;
            return pos-buffer;
        }

        memmove(buffer+ *total_chunk_size, pos, chunk_size);
        pos += chunk_size + 2;
        *total_chunk_size += chunk_size;
        buffer[*total_chunk_size] = '\0';
    }

    *chunk_ended = false;
    return pos-buffer;
}

/**
 * @brief    解析已接收的 HTTP 响应数据。
 *
 * @details  根据接收的数据，进行数据解析，主要解析 HTTP 头部信息，将相关数据保存到
 *           version、status_code、content_length 等信息，其他 HTTP 头部信息，默认忽略。
 *
 * @param    parser 解析器指针
 * @param    buffer 待解析的 HTTP 数据。
 *
 * @return   已处理的长度：
 *              1.当接收的包是完整的包时，返回值一般等于 buffer_len，表示全部都已解析处理;
 *              2.当接收的包不是完整的包时，返回值为已处理部分的长度，一般小于 buffer_len，
 *                表示已处理部分的长度，剩余部分未处理数据，需要接收更多数据后，合并后再次调用本函数解析；
 *              3.数据不满足解析要求，无法解析，需要继续接收更多内容时，返回值为 0，调用端处理逻辑同 2；
 *              4.出现异常，无法处理时，返回错误码，取值 < 0。
 * @see tc_iot_sys_code_e
 */
int tc_iot_http_parser_analysis(tc_iot_http_response_parser * parser, const char * buffer, int buffer_len) {
    bool header_complete = false;
    const char * pos = NULL;
    int buffer_parsed = 0;
    int i = 0;

    IF_NULL_RETURN(parser, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(buffer, TC_IOT_NULL_POINTER);

    pos = buffer;
start:
    if (buffer_parsed >= buffer_len) {
        return buffer_parsed;
    }

    switch(parser->state) {
        case _PARSER_START:
            /* Head should alwarys start with: HTTP/1.x COD */
		    if ( buffer_len < sizeof("HTTP/1.x M0N")) {
                return 0;
            }

            if (!tc_iot_str7equal(pos, 'H', 'T', 'T', 'P', '/', '1', '.')) {
                TC_IOT_LOG_ERROR("Response header not start with HTTP/1.[x] .");
                return TC_IOT_FAILURE;
            } else {
                buffer_parsed += 7;
                pos = buffer + buffer_parsed;
            }

            if (pos[0] != '0' && pos[0] != '1') {
                TC_IOT_LOG_ERROR("HTTP version 1.%c not supported", pos[0]);
                return TC_IOT_FAILURE;
            } else {
                parser->version = pos[0] - '0';
            }

            if (' ' != pos[1]) {
                TC_IOT_LOG_ERROR("space not found");
                return TC_IOT_FAILURE;
            }

            buffer_parsed += 2;
            pos = buffer + buffer_parsed;
            parser->status_code = 0;
            for (i = 0; i < 3; i++) {
                if (pos[i] > '9' || pos[i] < '0') {
                    return TC_IOT_HTTP_INVALID_STATUS_CODE;
                } else {
                    parser->status_code = parser->status_code * 10 + pos[i] - '0';
                }
            }
            TC_IOT_LOG_TRACE("version: 1.%d, status code: %d",parser->version, parser->status_code);
            buffer_parsed += 3;
            pos = buffer + buffer_parsed;
            parser->state = _PARSER_IGNORE_TO_RETURN_CHAR;
            goto start;
        case _PARSER_IGNORE_TO_RETURN_CHAR:
            while (buffer_parsed < buffer_len) {
                if ('\r' == (*pos)) {
                    buffer_parsed++;
                    pos++;
                    parser->state = _PARSER_SKIP_NEWLINE_CHAR;
                    goto start;
//                    break;
                }
                buffer_parsed++;
                pos++;
            }
            return buffer_parsed;
        case _PARSER_SKIP_NEWLINE_CHAR:
            if ('\n' != *pos) {
                TC_IOT_LOG_ERROR("expecting \\n");
                return TC_IOT_FAILURE;
            }
            buffer_parsed++;
            pos++;
            parser->state = _PARSER_HEADER;
            goto start;
        case _PARSER_HEADER:
            /* TC_IOT_LOG_TRACE("pos=%s",pos); */
            if ('\r' == (*pos)) {
                /* status line\r\n */
                /* http headers\r\n */
                /* \r\nbody */
                /* ^ */
                /* | we are here */
                buffer_parsed += 1;
                pos = buffer + buffer_parsed;
                parser->state = _PARSER_IGNORE_TO_BODY_START;
                goto start;
            } else {
                for (i = 0; i < (buffer_len-buffer_parsed); i++) {
                    if (':' == pos[i]) {
                        if ((i == tc_iot_const_str_len(HTTP_HEADER_CONTENT_LENGTH))
                                && (0 == memcmp(pos, HTTP_HEADER_CONTENT_LENGTH, i))) {
                            header_complete = tc_iot_http_has_line_ended(pos+i+1);
                            if (header_complete) {
                                /* TC_IOT_LOG_TRACE("%s found:%s",HTTP_HEADER_CONTENT_LENGTH, pos+i+2); */
                                parser->content_length = tc_iot_try_parse_int(pos+i+2, NULL);
                            } else {
                                TC_IOT_LOG_TRACE("%s not complete, continue reading:%s",HTTP_HEADER_CONTENT_LENGTH, pos+i+2);
                                return buffer_parsed;
                            }

                        } else if ((i == tc_iot_const_str_len(HTTP_HEADER_LOCATION))
                                && (0 == memcmp(pos, HTTP_HEADER_LOCATION, i))) {
                            header_complete = tc_iot_http_has_line_ended(pos+i+1);
                            if (header_complete) {
                                TC_IOT_LOG_TRACE("%s found:%s",HTTP_HEADER_LOCATION, pos+i+2);
                                parser->location = pos+i+2;
                            } else {
                                TC_IOT_LOG_TRACE("%s not complete, continue reading:%s",HTTP_HEADER_LOCATION, pos+i+2);
                                return buffer_parsed;
                            }
                        } else if ((i == tc_iot_const_str_len(HTTP_HEADER_CONTENT_TYPE))
                                && (0 == memcmp(pos, HTTP_HEADER_CONTENT_TYPE, i))) {
                            /* TC_IOT_LOG_TRACE("%s found:%s",HTTP_HEADER_CONTENT_TYPE, pos+i+2); */
                        } else if ((i == tc_iot_const_str_len(HTTP_HEADER_TRANSFER_ENCODING))
                                && (0 == memcmp(pos, HTTP_HEADER_TRANSFER_ENCODING, i))) {
                            header_complete = tc_iot_http_has_line_ended(pos+i+1);
                            if (header_complete) {
                                /* chunked, compress, deflate, gzip, identity. */
                                // 请求的时候 指定 了 identity，响应只会是 identity 或 chunked
                                if (*(pos+i+2) == 'i') {
                                    TC_IOT_LOG_TRACE("%s recorgnized as : identity",HTTP_HEADER_TRANSFER_ENCODING);
                                    parser->transfer_encoding = TRANSFER_ENCODING_IDENTITY;
                                } else if (*(pos+i+2) == 'c' && *(pos+i+3) == 'h') {
                                    TC_IOT_LOG_TRACE("%s recorgnized as : chunked",HTTP_HEADER_TRANSFER_ENCODING);
                                    parser->transfer_encoding = TRANSFER_ENCODING_CHUNKED;
                                } else {
                                    TC_IOT_LOG_ERROR("%s value invalid :%s",HTTP_HEADER_TRANSFER_ENCODING, pos+i+2);
                                }
                            } else {
                                TC_IOT_LOG_TRACE("%s not complete, continue reading:%s",HTTP_HEADER_TRANSFER_ENCODING, pos+i+2);
                                return buffer_parsed;
                            }
                        } else {
                            /* TC_IOT_LOG_TRACE("ignore i=%d,pos=%s",i, pos); */
                        }
                        buffer_parsed += i+1;
                        pos = buffer + buffer_parsed;
                        parser->state = _PARSER_IGNORE_TO_RETURN_CHAR;
                        goto start;
                    }
                }
                TC_IOT_LOG_TRACE("buffer_parsed=%d, buffer_len=%d,left=%s", buffer_parsed, buffer_len, pos);
                return buffer_parsed;
            }
            break;
        case _PARSER_IGNORE_TO_BODY_START:
            if ('\n' != *pos) {
                TC_IOT_LOG_ERROR("expecting \\n");
                return TC_IOT_FAILURE;
            }
            buffer_parsed++;
            pos++;
            /* TC_IOT_LOG_TRACE("body=%s", pos); */
            parser->state = _PARSER_END;
            /* parser->body = pos; */
            return buffer_parsed;
        case _PARSER_END:
            return buffer_len;
        default:
            TC_IOT_LOG_ERROR("invalid parse state=%d", parser->state);
            return TC_IOT_FAILURE;
    }
}


/**
 * @brief    HTTP Client 初始化
 *
 * @details  初始化 HTTP Client，指定请求的 HTTP Method及默认的版本号，格式等。
 *
 * @param    c 待初始化的 HTTP Client 结构
 * @param    method HTTP 请求方法：HEAD、GET、POST ...
 *
 *  @return 结果返回码
 * @see tc_iot_sys_code_e
 */

int tc_iot_http_client_init(tc_iot_http_client * c, const char * method) {
    IF_NULL_RETURN(c, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(method, TC_IOT_NULL_POINTER);

    memset(c, 0, sizeof(*c));
    if (!method) {
        c->method = HTTP_POST;
    } else {
        c->method = method;
    }
    c->version = HTTP_VERSION_1_1;
    c->abs_path = "/";
    c->extra_headers = "";
    c->content_type = HTTP_CONTENT_FORM_URLENCODED;

    return TC_IOT_SUCCESS;
}


/**
 * @brief    设置 HTTP 请求版本号
 *
 * @details  设置 HTTP 请求时，携带的版本号，一般为 1.0、1.1 等。
 *
 * @param    c HTTP Client 结构指针
 * @param    version 待设定的版本号：1.0，1.1
 *
 *  @return 结果返回码
 * @see tc_iot_sys_code_e
 */
int tc_iot_http_client_set_version(tc_iot_http_client * c, const char * version) {
    IF_NULL_RETURN(c, TC_IOT_NULL_POINTER);
    c->version = version;
    return TC_IOT_SUCCESS;
}

/**
 * @brief    设置 HTTP 请求 Host
 *
 * @details  设置 HTTP 请求时，头部携带的 Host 参数。
 *
 * @param    c HTTP Client 结构指针
 * @param    host 服务端域名或IP
 *
 *  @return 结果返回码
 * @see tc_iot_sys_code_e
 */
int tc_iot_http_client_set_host(tc_iot_http_client * c, const char * host) {
    IF_NULL_RETURN(c, TC_IOT_NULL_POINTER);
    c->host = host;
    return TC_IOT_SUCCESS;
}

/**
 * @brief    设置请求路径
 *
 * @details  设置请求路径地址，例如，/root/some/path
 *
 * @param    c HTTP Client 结构指针
 * @param    abs_path 请求路径
 *
 *  @return 结果返回码
 * @see tc_iot_sys_code_e
 *
 */
int tc_iot_http_client_set_abs_path(tc_iot_http_client * c, const char * abs_path) {
    IF_NULL_RETURN(c, TC_IOT_NULL_POINTER);
    c->abs_path = abs_path;
    return TC_IOT_SUCCESS;
}


/**
 * @brief    设置请求格式
 *
 * @details  设置请求格式
 *
 * @param    c HTTP Client 结构指针
 * @param    content_type 请求格式，例如： application/json, application/x-www-form-urlencoded
 *
 *  @return 结果返回码
 * @see tc_iot_sys_code_e
 *
 */
int tc_iot_http_client_set_content_type(tc_iot_http_client * c, const char * content_type) {
    IF_NULL_RETURN(c, TC_IOT_NULL_POINTER);
    c->content_type = content_type;
    return TC_IOT_SUCCESS;
}

/**
 * @brief    设置其他自定义 HTTP Header 参数。
 *
 * @details  设置除 Host、Content Type、Content Length、User Agent、Accept、Accept-Encoding外的，其他自定义 HTTP Header 参数，多次调用时，以最后一次为有效。
 *
 * @param    c HTTP Client 结构指针
 * @param    extra_headers 待指定的 HTTP Header，每个Header要固定以 \r\n 作为行结束符，格式为：
 "Header1: Value1\r\nHeader2: Value2\r\n...HeaderN:ValueN\r\n"
 *
 *  @return 结果返回码
 * @see tc_iot_sys_code_e
 *
 */
int tc_iot_http_client_set_extra_headers(tc_iot_http_client * c, const char * extra_headers) {
    IF_NULL_RETURN(c, TC_IOT_NULL_POINTER);
    c->extra_headers = extra_headers;
    return TC_IOT_SUCCESS;
}

/**
 * @brief    设置请求 Body
 *
 * @details  设置请求 Body
 *
 * @param    c HTTP Client 结构指针
 * @param    body 请求内容，例如，Json文档或者URL Encode的表格数据等。
 *
 *  @return 结果返回码
 * @see tc_iot_sys_code_e
 *
 */
int tc_iot_http_client_set_body(tc_iot_http_client * c, const char * body) {
    IF_NULL_RETURN(c, TC_IOT_NULL_POINTER);
    c->body = body;
    return TC_IOT_SUCCESS;
}


/**
 * @brief    格式化请求数据，写入到 buffer 中。
 *
 * @details  格式化请求数据，写入到 buffer 中。
 *
 * @param    buffer 结果写入到改指针指向区域
 * @param    buffer_len 缓存区最大大小
 * @param    c HTTP Client 结构指针
 *
 *  @return 结果返回码 大于 0 时，表示有效数据长度，小于 0 时，则表示处理出错。
 * @see tc_iot_sys_code_e
 *
 */
int tc_iot_http_client_format_buffer(char * buffer, int buffer_len, tc_iot_http_client * c) {
    int ret = 0;
    int content_length = 0;

    IF_NULL_RETURN(c, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(buffer, TC_IOT_NULL_POINTER);
    IF_LESS_RETURN(buffer_len, 0, TC_IOT_INVALID_PARAMETER);

    if (c->body) {
        content_length = strlen(c->body);
    } else {
        c->body = "";
    }

    ret = tc_iot_hal_snprintf(buffer, buffer_len,
                              "%s %s HTTP/%s\r\n"
                              "User-Agent: tciotclient/1.0\r\n"
                              "Host: %s\r\n"
                              "Accept: */*\r\n"
                              "Content-Type: %s\r\n"
                              "Accept-Encoding: identity\r\n"
                              "Content-Length: %d\r\n%s\r\n%s",
                              c->method, c->abs_path, c->version,
                              c->host,c->content_type,content_length,
                              c->extra_headers,c->body);
    if (ret > 0) {
        return ret;
    } else {
        return TC_IOT_BUFFER_OVERFLOW;
    }
}



/**
 * @brief    处理将准备好的 HTTP 请求数据发送出去，并接受响应，对于超大数据，通过回调函数通知给调用方。
 *
 * @details  将 buffer 中指定的数据（buffer_used 表示有效长度），通过网络发送给服务端，
 *           并复用 buffer 接收响应数据。最终返回时，buffer 仅包含响应中的 body 部分数据。
 *
 * @param    buffer 待发送数据
 * @param    buffer_used 待发送数据的有效长度
 * @param    buffer_len buffer 区域最大可用长度，用来接收响应数据时判断
 * @param    p_network 网络接口
 * @param    p_parser HTTP 响应解析结构体
 * @param    host 服务器IP或域名地址
 * @param    port 服务器端口
 * @param    secured  是否采用 HTTPS
 * @param    timeout_ms 请求最大等待时延，单位为毫秒
 * @param    resp_callback 请求响应回调，当接收缓存区太小，无法一次性收完时，通过指定回调，可分批接收数据（OTA 场景）。默认填 NULL。
 * @param    callback_context 和 resp_callback 配合使用，用来指定 resp_callback 回调时，回传的 context 参数。默认填 NULL。
 *
 * @return   return > 0 时，表示已有效接收的 body 长度， < 0 时，表示出错。
 * @see tc_iot_sys_code_e
 */
int tc_iot_http_client_internal_perform(char * buffer, int buffer_used, int buffer_len,
                                        tc_iot_network_t * p_network, tc_iot_http_response_parser * p_parser,
                                        const char * host, uint16_t port,
                                        bool secured, int timeout_ms,
                                        tc_iot_http_response_callback resp_callback, const void * callback_context) {
    bool head_only = false;
    const int timer_tick = 100;
    int ret = 0;
    int write_ret = 0;
    int read_ret  = 0;
    int parse_ret = 0;
    int callback_ret = 0;
    tc_iot_timer timer;
    int parse_left = 0;
    int content_length = 0;
    int received_bytes = 0;
    bool chunk_end = false;
    int total_chunk_size = 0;

    ret = tc_iot_network_prepare(p_network, TC_IOT_SOCK_STREAM, TC_IOT_PROTO_HTTP, secured, NULL);
    if (ret < 0) {
        return ret;
    }

    head_only = tc_iot_str4equal(buffer, 'H', 'E', 'A', 'D');

    tc_iot_hal_timer_init(&timer);
    tc_iot_hal_timer_countdown_ms(&timer,timeout_ms);
    ret = p_network->do_connect(p_network, host, port);
    write_ret = p_network->do_write(p_network, (unsigned char *)buffer, buffer_used, timeout_ms);
    if (write_ret != buffer_used) {
        TC_IOT_LOG_ERROR("send packet failed: expect len=%d, write return=%d", buffer_used, write_ret);
        return TC_IOT_SEND_PACK_FAILED;
    }

    tc_iot_http_parser_init(p_parser);
    do {
        read_ret = p_network->do_read(p_network, (unsigned char *)buffer+parse_left, buffer_len-parse_left, timer_tick);
        if (read_ret > 0) {
            TC_IOT_LOG_TRACE("read_ret=%d,parse_left=%d", read_ret, parse_left);

            read_ret += parse_left;
            if (read_ret < buffer_len) {
                buffer[read_ret] = '\0';
            }

            parse_ret = tc_iot_http_parser_analysis(p_parser, buffer, read_ret);
            if (parse_ret < 0) {
                TC_IOT_LOG_ERROR("read from request host=%s:%d failed, ret=%d", host, port, ret);
                p_network->do_disconnect(p_network);
                return parse_ret;
            }

            if (parse_ret > read_ret) {
                TC_IOT_LOG_ERROR("tc_iot_http_parser_analysis parse_ret=%d too large, ret=%d", parse_ret, ret);
                p_network->do_disconnect(p_network);
                return TC_IOT_FAILURE;
            }

            parse_left = read_ret - parse_ret;
            // 将未解析的数据前移，继续接收数据并解析
            if (parse_left > 0) {
                memmove(buffer, buffer+parse_ret, parse_left);
                buffer[parse_left] = '\0';
            }

            if (p_parser->status_code != 200 && p_parser->status_code != 206) {
                TC_IOT_LOG_ERROR("http resoponse parser.status_code = %d", p_parser->status_code);
                p_network->do_disconnect(p_network);
                return TC_IOT_ERROR_HTTP_REQUEST_FAILED;
            }

            // HTTP Header 全部解析完成
            if (_PARSER_END == p_parser->state) {
                if (head_only) {
                    // HEAD 请求无包体，可直接返回
                    TC_IOT_LOG_TRACE("this is a head request, quit body parsing.");
                    return TC_IOT_SUCCESS;
                }
                content_length = p_parser->content_length;

                // 至此，未解析的数据都是响应包体
                received_bytes = parse_left;
                TC_IOT_LOG_TRACE("ver=1.%d, code=%d,content_length=%d, received_bytes=%d",
                                 p_parser->version, p_parser->status_code, p_parser->content_length, received_bytes);
                if (content_length > buffer_len) {
                    if (resp_callback == NULL) {
                        TC_IOT_LOG_ERROR("buffer not enough: content_length=%d, buffer_len=%d", content_length, buffer_len);
                        p_network->do_disconnect(p_network);
                        return TC_IOT_BUFFER_OVERFLOW;
                    } else {

                        // 回调通知接收数据
                        callback_ret = resp_callback(callback_context, (const char *)buffer, parse_left, received_bytes , content_length);
                        if (callback_ret != TC_IOT_SUCCESS) {
                            TC_IOT_LOG_ERROR("callback failed ret=%d, abort.", callback_ret);
                            return TC_IOT_FAILURE;
                        }

                        while (received_bytes < content_length) {
                            // buffer_len-1 是为了预留一个字节，用来添加字符串结束符 '\0' 。
                            read_ret = p_network->do_read(p_network, (unsigned char *)buffer, buffer_len-1, timer_tick);
                            if (read_ret > 0) {
                                received_bytes += read_ret;
                                TC_IOT_LOG_TRACE("read=%d,total_read=%d, total=%d", read_ret, received_bytes, content_length);

                                buffer[read_ret] = '\0';
                                callback_ret = resp_callback(callback_context, (const char *)buffer, read_ret, received_bytes , content_length);
                                if (callback_ret != TC_IOT_SUCCESS) {
                                    TC_IOT_LOG_ERROR("callback failed ret=%d, abort.", callback_ret);
                                    return TC_IOT_FAILURE;
                                }
                                received_bytes += ret;

                                if (received_bytes >= content_length) {
                                    TC_IOT_LOG_TRACE("%s=%d, received_bytes=%d", HTTP_HEADER_CONTENT_LENGTH, content_length, received_bytes);
                                    p_network->do_disconnect(p_network);
                                    return TC_IOT_SUCCESS;
                                }
                            } else if (read_ret == TC_IOT_NET_NOTHING_READ) {
                                continue;
                            } else {
                                TC_IOT_LOG_ERROR("read buffer error:%d", read_ret);
                                p_network->do_disconnect(p_network);
                                return read_ret;
                            }
                        }
                        p_network->do_disconnect(p_network);
                        return received_bytes;
                    }
                }

                if (p_parser->transfer_encoding == TRANSFER_ENCODING_IDENTITY) {
                    while (received_bytes < content_length) {
                        read_ret = p_network->do_read(p_network, (unsigned char *)buffer+received_bytes, buffer_len-received_bytes, timer_tick);
                        if (read_ret > 0) {
                            received_bytes += read_ret;
                            TC_IOT_LOG_TRACE("read=%d,total_read=%d, total=%d", read_ret, received_bytes, content_length);
                        } else if (read_ret == TC_IOT_NET_NOTHING_READ) {
                            continue;
                        } else {
                            TC_IOT_LOG_ERROR("read buffer error:%d", read_ret);
                            p_network->do_disconnect(p_network);
                            return read_ret;
                        }
                    }
                } else {
                    /* TC_IOT_LOG_TRACE("chunk processing  %d:%s", received_bytes, buffer); */
                    while (!tc_iot_http_is_complete_chunk(buffer, received_bytes)) {
                        read_ret = p_network->do_read(p_network, (unsigned char *)buffer+received_bytes, buffer_len-received_bytes, timer_tick);
                        if (read_ret > 0) {
                            received_bytes += read_ret;
                            TC_IOT_LOG_TRACE("read=%d,total_read=%d, total=%d", read_ret, received_bytes, content_length);
                        } else if (read_ret == TC_IOT_NET_NOTHING_READ) {
                            continue;
                        } else {
                            TC_IOT_LOG_ERROR("read buffer error:%d", read_ret);
                            p_network->do_disconnect(p_network);
                            return read_ret;
                        }
                    }

                    ret = tc_iot_http_merge_chunk(buffer, received_bytes, &total_chunk_size, &chunk_end);
                    if (ret != received_bytes || total_chunk_size <= 0 || !chunk_end) {
                        TC_IOT_LOG_ERROR("ret(%d) != received_bytes(%d) || total_chunk_size(%d) <= 0 || !chunk_end(%d)",
                                         ret, received_bytes,total_chunk_size ,chunk_end );
                        p_network->do_disconnect(p_network);
                        return TC_IOT_INVALID_HTTP_CHUNK_FORMAT;
                    }
                }
                p_network->do_disconnect(p_network);
                return received_bytes;
            }
        } else if (read_ret == TC_IOT_NET_NOTHING_READ) {
            continue;
        } else {
            TC_IOT_LOG_ERROR("read buffer error:%d", read_ret);
            p_network->do_disconnect(p_network);
            return read_ret;
        }

        if (read_ret <= 0) {
            TC_IOT_LOG_TRACE("ret=%d, len=%d", read_ret, buffer_len);
            p_network->do_disconnect(p_network);
            return TC_IOT_SUCCESS;
        }

    } while(!tc_iot_hal_timer_is_expired(&timer) && (received_bytes < buffer_len));

    p_network->do_disconnect(p_network);

    if (received_bytes > 0) {
        return received_bytes;
    } else {
        return read_ret;
    }
}

/**
 * @brief    处理将准备好的 HTTP 请求数据发送出去，并接受响应，对于超大数据，通过回调函数通知给调用方。
 *
 * @details  将 buffer 中指定的数据（buffer_used 表示有效长度），通过网络发送给服务端，
 *           并复用 buffer 接收响应数据。最终返回时，buffer 仅包含响应中的 body 部分数据。
 *
 * @param    buffer 待发送数据
 * @param    buffer_used 待发送数据的有效长度
 * @param    buffer_len buffer 区域最大可用长度，用来接收响应数据时判断
 * @param    host 服务器IP或域名地址
 * @param    port 服务器端口
 * @param    secured  是否采用 HTTPS
 * @param    timeout_ms 请求最大等待时延，单位为毫秒
 *
 * @return   return > 0 时，表示已有效接收的 body 长度， < 0 时，表示出错。
 * @see tc_iot_sys_code_e
 */
int tc_iot_http_client_perform(char * buffer, int buffer_used, int buffer_len,
                            const char * host, uint16_t port,
                            bool secured, int timeout_ms) {
    tc_iot_network_t network;
    tc_iot_http_response_parser parser;
    return tc_iot_http_client_internal_perform(buffer, buffer_used, buffer_len,
                                               &network, &parser,  host, port,
                                               secured, timeout_ms, NULL, NULL);
}
