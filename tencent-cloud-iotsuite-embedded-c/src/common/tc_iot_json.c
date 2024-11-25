#include "tc_iot_inc.h"


int tc_iot_jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    IF_NULL_RETURN(json, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(tok, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(s, TC_IOT_NULL_POINTER);
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

static void _trace_node(const char *prefix, const char *str, const jsmntok_t *node) {
    /* TC_IOT_LOG_TRACE("---%s type=%d,start=%d,end=%d,size=%d,parent=%d\t %s", */
    /* prefix,  */
    /* node->type, node->start, node->end, node->size, node->parent, */
    /* tc_iot_log_summary_string( str + node->start, node->end - node->start)); */
}

const char * tc_iot_json_token_type_str(int type) {
    switch(type) {
    case JSMN_UNDEFINED:
        return "undefined";
    case JSMN_OBJECT:
        return "object";
    case JSMN_ARRAY:
        return "array";
    case JSMN_STRING:
        return "string";
    case JSMN_PRIMITIVE:
        return "premitive:bool/number/null";
    default:
        return "unknown";
    }
}

void tc_iot_json_print_node(const char *prefix, const char *json, const jsmntok_t *root_node, int node_index) {
    const jsmntok_t * node;

    node = root_node + node_index;
    TC_IOT_LOG_TRACE("%s id=%d,type=%s,start=%d,end=%d,size(child_count)=%d,parent=%d\t %s",
                     prefix, node_index,
                     tc_iot_json_token_type_str(node->type), 
                     node->start, node->end, node->size, node->parent,
                     tc_iot_log_summary_string( json + node->start, node->end - node->start)
        );
}

int tc_iot_jsoneq_len(const char *json, const jsmntok_t *tok, const char *s,
                      int len) {
    IF_NULL_RETURN(json, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(tok, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(s, TC_IOT_NULL_POINTER);
    if (tok->type == JSMN_STRING && (int)len == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

int _unicode_char_to_long(const char *unicode_str, int len,
                          unsigned long *code) {
    int i = 0;
    unsigned long result = 0;
    unsigned char temp = 0;

    IF_NULL_RETURN(unicode_str, TC_IOT_NULL_POINTER);
    IF_LESS_RETURN(8, len, TC_IOT_INVALID_PARAMETER);

    for (i = 0; i < len; i++) {
        temp = unicode_str[i];
        /* transform hex character to the 4bit equivalent number, using the
         * ascii table indexes */
        if (temp >= '0' && temp <= '9') {
            temp = temp - '0';
        } else if (temp >= 'a' && temp <= 'f') {
            temp = temp - 'a' + 10;
        } else if (temp >= 'A' && temp <= 'F') {
            temp = temp - 'A' + 10;
        } else {
            return TC_IOT_INVALID_PARAMETER;
        }
        result = (result << 4) | (temp & 0xF);
    }

    *code = result;
    return TC_IOT_SUCCESS;
}

int tc_iot_unicode_to_utf8(char *output, int output_len, unsigned long code) {
    IF_NULL_RETURN(output, TC_IOT_NULL_POINTER);
    if (code <= 0x7F) {
        IF_LESS_RETURN(output_len, 1, TC_IOT_BUFFER_OVERFLOW);
        /* * U-00000000 - U-0000007F:  0xxxxxxx */
        *output = (code & 0x7F);
        return 1;
    } else if (code >= 0x80 && code <= 0x7FF) {
        IF_LESS_RETURN(output_len, 2, TC_IOT_BUFFER_OVERFLOW);

        /* * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx */
        *(output + 1) = (code & 0x3F) | 0x80;
        *output = ((code >> 6) & 0x1F) | 0xC0;
        return 2;
    } else if (code >= 0x800 && code <= 0xFFFF) {
        IF_LESS_RETURN(output_len, 3, TC_IOT_BUFFER_OVERFLOW);

        /* * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx */
        *(output + 2) = (code & 0x3F) | 0x80;
        *(output + 1) = ((code >> 6) & 0x3F) | 0x80;
        *output = ((code >> 12) & 0x0F) | 0xE0;
        return 3;
    } else if (code >= 0x10000 && code <= 0x1FFFFF) {
        IF_LESS_RETURN(output_len, 4, TC_IOT_BUFFER_OVERFLOW);

        /* * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        *(output + 3) = (code & 0x3F) | 0x80;
        *(output + 2) = ((code >> 6) & 0x3F) | 0x80;
        *(output + 1) = ((code >> 12) & 0x3F) | 0x80;
        *output = ((code >> 18) & 0x07) | 0xF0;
        return 4;
    } else if (code >= 0x200000 && code <= 0x3FFFFFF) {
        IF_LESS_RETURN(output_len, 5, TC_IOT_BUFFER_OVERFLOW);

        /* * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx
         * 10xxxxxx */
        *(output + 4) = (code & 0x3F) | 0x80;
        *(output + 3) = ((code >> 6) & 0x3F) | 0x80;
        *(output + 2) = ((code >> 12) & 0x3F) | 0x80;
        *(output + 1) = ((code >> 18) & 0x3F) | 0x80;
        *output = ((code >> 24) & 0x03) | 0xF8;
        return 5;
    } else if (code >= 0x4000000 && code <= 0x7FFFFFFF) {
        IF_LESS_RETURN(output_len, 6, TC_IOT_BUFFER_OVERFLOW);

        /* * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx
         * 10xxxxxx 10xxxxxx */
        *(output + 5) = (code & 0x3F) | 0x80;
        *(output + 4) = ((code >> 6) & 0x3F) | 0x80;
        *(output + 3) = ((code >> 12) & 0x3F) | 0x80;
        *(output + 2) = ((code >> 18) & 0x3F) | 0x80;
        *(output + 1) = ((code >> 24) & 0x3F) | 0x80;
        *output = ((code >> 30) & 0x01) | 0xFC;
        return 6;
    }

    return 0;
}

int tc_iot_json_unescape(char *dest, int dest_len, const char *src,
                         int src_len) {
    int index = 0;
    int ret;
    int dest_index = 0;
    bool valid_escaped = true;
    unsigned long temp_unicode;

    for (index = 0; src[index] && (index < src_len); index++) {
        if (src[index] == '\\') {
            valid_escaped = true;
            if (index < (src_len - 1)) {
                switch (src[index + 1]) {
                case '"':
                    dest[dest_index++] = '"';
                    index++;
                    break;
                case '\\':
                    dest[dest_index++] = '\\';
                    index++;
                    break;
                case '/':
                    dest[dest_index++] = '/';
                    index++;
                    break;
                case 'b':
                    dest[dest_index++] = '\b';
                    index++;
                    break;
                case 'f':
                    dest[dest_index++] = '\f';
                    index++;
                    break;
                case 'n':
                    dest[dest_index++] = '\n';
                    index++;
                    break;
                case 'r':
                    dest[dest_index++] = '\r';
                    index++;
                    break;
                case 't':
                    dest[dest_index++] = '\t';
                    index++;
                    break;
                case 'u':
                    if (src_len - index >= 5) {
                        ret = _unicode_char_to_long(&src[index + 2], 4,
                                                    &temp_unicode);
                        if (ret != TC_IOT_SUCCESS) {
                            valid_escaped = false;
                            TC_IOT_LOG_WARN("unicode data invalid %s",
                                            tc_iot_log_summary_string( &src[index], src_len - index));
                            break;
                        }
                        ret = tc_iot_unicode_to_utf8(&dest[dest_index],
                                                     dest_len - dest_index,
                                                     temp_unicode);
                        if (ret <= 0) {
                            valid_escaped = false;
                            TC_IOT_LOG_WARN(
                                "unicode %d transform to utf8 failed: "
                                "ret=%d",
                                (int)temp_unicode, ret);
                            break;
                        }
                        dest_index += ret;
                        index += 5;
                    } else {
                        TC_IOT_LOG_WARN("unicode data invalid %s",
                                        tc_iot_log_summary_string( &src[index], src_len - index));
                    }
                    break;
                default:
                    TC_IOT_LOG_WARN("invalid json escape:%s",
                                    tc_iot_log_summary_string( &src[index], src_len - index));
                    valid_escaped = false;
                    break;
                }
            }

            if (valid_escaped) {
                continue;
            }
        }
        dest[dest_index++] = src[index];
    }

    if (index < dest_len) {
        dest[dest_index] = '\0';
    }

    return dest_index;
}

char *tc_iot_json_inline_escape(char *dest, int dest_len, const char *src) {
    tc_iot_json_escape(dest, dest_len, src, strlen(src));
    return dest;
}

int tc_iot_json_escape(char *dest, int dest_len, const char *src, int src_len) {
    int src_index;
    int dest_index;

    for (src_index = 0, dest_index = 0;
         src[src_index] && (src_index < src_len) && (dest_index < dest_len);
         src_index++) {
        switch (src[src_index]) {
        case '\b':
            dest[dest_index++] = '\\';
            dest[dest_index++] = 'b';
            break;
        case '\f':
            dest[dest_index++] = '\\';
            dest[dest_index++] = 'f';
            break;
        case '\n':
            dest[dest_index++] = '\\';
            dest[dest_index++] = 'n';
            break;
        case '\r':
            dest[dest_index++] = '\\';
            dest[dest_index++] = 'r';
            break;
        case '\t':
            dest[dest_index++] = '\\';
            dest[dest_index++] = 't';
            break;
        case '"':
        case '\\':
        case '/':
            dest[dest_index++] = '\\';
        default:
            dest[dest_index++] = src[src_index];
            break;
        }
    }

    if (dest_index <= (dest_len - 1)) {
        dest[dest_index] = '\0';
    }
    return dest_index;
}

int tc_iot_json_find_token(const char *json, const jsmntok_t *root_token,
                           int count, const char *path, char *result,
                           int result_len) {
    const char *name_start = path;
    int tok_index = 0;
    int parent_index = 0;
    int child_count;
    int visited_child;
    int token_name_len = 0;
    int val_len = 0;
    const char *pos;

    IF_NULL_RETURN(json, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(root_token, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(path, TC_IOT_NULL_POINTER);
    IF_LESS_RETURN(count, 1, TC_IOT_INVALID_PARAMETER);

    for (tok_index = 0; tok_index < count;) {
        pos = strstr(name_start, ".");
        if (NULL != pos) {
            token_name_len = pos - name_start;
        } else {
            token_name_len = strlen(name_start);
        }

        _trace_node("check node:", json, &root_token[tok_index]);
        if (root_token[tok_index].type != JSMN_OBJECT) {
            TC_IOT_LOG_ERROR("token %d not object", tok_index);
            return -1;
        }

        parent_index = tok_index;
        child_count = root_token[tok_index].size;
        tok_index++;
        visited_child = 0;

        for (; (visited_child < child_count) && (tok_index < count); tok_index++) {
            _trace_node("compare node:", json, &root_token[tok_index]);
            if (parent_index == root_token[tok_index].parent) {
                if (tc_iot_jsoneq_len(json, &root_token[tok_index], name_start,
                                      token_name_len) == 0) {
                    tok_index++;

                    if (NULL == pos) {
                        _trace_node("match node:", json,
                                    &root_token[tok_index]);

                        if (result && result_len) {
                            val_len = root_token[tok_index].end -
                                root_token[tok_index].start;
                            if (val_len > result_len) {
                                TC_IOT_LOG_ERROR("result buffer not enough val_len=%d, result_len=%d", val_len, result_len);
                                return TC_IOT_BUFFER_OVERFLOW;
                            }

                            tc_iot_json_unescape(result, result_len,
                                                 json + root_token[tok_index].start,
                                                 val_len);
                            if (val_len < result_len) {
                                result[val_len] = 0;
                            }
                        }
                        /* TC_IOT_LOG_TRACE("result=%s, index=%d", tc_iot_log_summary_string(result,val_len), tok_index); */
                        return tok_index;
                    }
                    break;
                } else {
                    _trace_node("node name not match:", json, &root_token[tok_index]); 
                }
                visited_child++;
            } else {
                /* TC_IOT_LOG_TRACE("target parent=%d/current parent=%d", parent_index, root_token[tok_index].parent); */
                _trace_node("node parent not match:", json, &root_token[tok_index]); 
            }
        }

        if (visited_child >= child_count) {
            TC_IOT_LOG_TRACE("%s no match in json.", path);
            return TC_IOT_JSON_PATH_NO_MATCH;
        }

        /* continue search */
        name_start = pos + 1;
        /* TC_IOT_LOG_TRACE("searching sub path: %s", name_start); */
    }

    return TC_IOT_JSON_PATH_NO_MATCH;
}

int tc_iot_json_parse(const char * json, int json_len, jsmntok_t * tokens, int token_count) {
    jsmn_parser p;
    int ret;

    jsmn_init(&p);
    ret = jsmn_parse(&p, json, json_len, tokens, token_count);

    if (ret < 0) {
        if (JSMN_ERROR_NOMEM == ret) {
            TC_IOT_LOG_ERROR("Mem not enough: %s", tc_iot_log_summary_string(json, json_len));
            return  TC_IOT_JSON_PARSE_TOKEN_NO_MEM;
        } else {
            TC_IOT_LOG_ERROR("Failed to parse JSON, ret=%d: %s", ret, tc_iot_log_summary_string(json, json_len));
            return TC_IOT_JSON_PARSE_FAILED;
        }
    }

    if (ret < 1 || tokens[0].type != JSMN_OBJECT) {
        TC_IOT_LOG_ERROR("Failed to JSON format: %s", tc_iot_log_summary_string(json, json_len));
        return TC_IOT_JSON_PARSE_FAILED;
    }

    return ret;
}

int tc_iot_json_find_nth_child(const jsmntok_t *root_token, int count, int parent_index, int nth) {
    int i = 0;
    int child_count = 0;

    IF_NULL_RETURN(root_token, TC_IOT_NULL_POINTER);
    if (parent_index >= count) {
        TC_IOT_LOG_ERROR("parent_index=%d, count=%d", parent_index, count);
        return TC_IOT_INVALID_PARAMETER;
    }

    if (root_token[parent_index].type != JSMN_ARRAY && root_token[parent_index].type != JSMN_OBJECT) {
        TC_IOT_LOG_ERROR("parent node is not an array or array, type=%d",root_token[parent_index].type);
        return TC_IOT_INVALID_PARAMETER;
    }

    if (root_token[parent_index].size <= nth) {
        TC_IOT_LOG_ERROR("nth=%d large than node size=%d", nth,root_token[parent_index].size);
        return TC_IOT_INVALID_PARAMETER;
    }

    for (i = parent_index+1; i < count; i++) {
        if (root_token[i].parent == parent_index) {
            child_count++;
            if (child_count == (nth+1)) {
                return i;
            }
        }
    }

    TC_IOT_LOG_ERROR("nth=%d child not found, node size=%d.", nth, root_token[parent_index].size);
    return TC_IOT_INVALID_PARAMETER;
}

int tc_iot_json_tokenizer_load(tc_iot_json_tokenizer * tokenizer, const char * json_str, int json_str_len, jsmntok_t * tokens, int token_count) {
    int ret = 0;
    jsmn_parser p;

    IF_NULL_RETURN(tokenizer, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(json_str, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(tokens, TC_IOT_NULL_POINTER);

    jsmn_init(&p);
    ret = jsmn_parse(&p, json_str, json_str_len, tokens, token_count);

    if (ret < 0) {
        if (JSMN_ERROR_NOMEM == ret) {
            TC_IOT_LOG_ERROR("Mem not enough: %s", tc_iot_log_summary_string(json_str, json_str_len));
            return  TC_IOT_JSON_PARSE_TOKEN_NO_MEM;
        } else {
            TC_IOT_LOG_ERROR("Failed to parse JSON, ret=%d: %s", ret, tc_iot_log_summary_string(json_str, json_str_len));
            return TC_IOT_JSON_PARSE_FAILED;
        }
    }

    if (ret < 1 || tokens[0].type != JSMN_OBJECT) {
        TC_IOT_LOG_ERROR("Failed to JSON format: %s", tc_iot_log_summary_string(json_str, json_str_len));
        return TC_IOT_JSON_PARSE_FAILED;
    }

    tokenizer->used_count = ret;
    tokenizer->tokens = tokens;
    tokenizer->max_count = token_count;
    tokenizer->json_str = json_str;
    tokenizer->json_str_len = json_str_len;

    return ret;
}


jsmntok_t * tc_iot_json_tokenizer_get_token(tc_iot_json_tokenizer * tokenizer, int index) {
    jsmntok_t * node = NULL;

    if (!tokenizer) {
        TC_IOT_LOG_ERROR("tokenizer is null");
        return NULL;
    }
    node = tokenizer->tokens+index;
    return node;
}

const char * tc_iot_json_tokenizer_get_str_start(tc_iot_json_tokenizer * tokenizer, int index) {
    jsmntok_t * node = tc_iot_json_tokenizer_get_token(tokenizer, index);

    if (!node) {
        return NULL;
    }
    return tokenizer->json_str+node->start;
}

int tc_iot_json_tokenizer_get_str_len(tc_iot_json_tokenizer * tokenizer, int index) {
    jsmntok_t * node = tc_iot_json_tokenizer_get_token(tokenizer, index);
    if (!node) {
        return 0;
    }
    return node->end - node->start;
}

int tc_iot_json_tokenizer_get_child_count(tc_iot_json_tokenizer * tokenizer, int index) {
    jsmntok_t * node = tc_iot_json_tokenizer_get_token(tokenizer, index);
    if (!node) {
        return 0;
    }
    return node->size;
}

int tc_iot_json_tokenizer_nth_child_value(char * value, int value_len, tc_iot_json_tokenizer * tokenizer, int parent_index, int nth) {
    const char * child_start = NULL;
    int child_len = 0;
    int child_index = 0;

    child_index = tc_iot_json_tokenizer_nth_child(tokenizer, parent_index, nth);
    if (child_index <= 0) {
        TC_IOT_LOG_ERROR("find %dth child failed, parent_index=%d", nth, parent_index);
        return child_index;
    }

    child_start = tc_iot_json_tokenizer_get_str_start(tokenizer,child_index);
    child_len = tc_iot_json_tokenizer_get_str_len(tokenizer,child_index);
    if (child_len >= value_len) {
        TC_IOT_LOG_ERROR("child value too large, len=%d(max=%d), data:%s", child_len, value_len, tc_iot_log_summary_string(child_start, child_len));
        return TC_IOT_FAILURE;
    }

    strncpy(value, child_start, child_len);
    value[child_len] = '\0';
    return child_index;
}

int tc_iot_json_tokenizer_nth_child(tc_iot_json_tokenizer * tokenizer, int parent_index, int nth) {
    int i = 0;
    int child_count = 0;
    int count = 0;
    jsmntok_t * root_token = NULL;

    IF_NULL_RETURN(tokenizer, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(tokenizer->tokens, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(tokenizer->json_str, TC_IOT_NULL_POINTER);

    root_token = tokenizer->tokens;
    count = tokenizer->used_count;

    if (parent_index >= count) {
        TC_IOT_LOG_ERROR("parent_index=%d, count=%d", parent_index, count);
        return TC_IOT_INVALID_PARAMETER;
    }

    /* if (root_token[parent_index].type != JSMN_ARRAY && root_token[parent_index].type != JSMN_OBJECT) { */
    /*     TC_IOT_LOG_ERROR("parent node is not an array or array, type=%d",root_token[parent_index].type); */
    /*     return TC_IOT_INVALID_PARAMETER; */
    /* } */

    if (root_token[parent_index].size <= nth) {
        TC_IOT_LOG_ERROR("nth=%d large than node size=%d", nth,root_token[parent_index].size);
        return TC_IOT_INVALID_PARAMETER;
    }

    for (i = parent_index+1; i < count; i++) {
        if (root_token[i].parent == parent_index) {
            child_count++;
            if (child_count == (nth+1)) {
                return i;
            }
        }
    }

    TC_IOT_LOG_ERROR("nth=%d child not found, node size=%d.", nth, root_token[parent_index].size);
    return TC_IOT_INVALID_PARAMETER;
}

int tc_iot_json_tokenizer_find_child(tc_iot_json_tokenizer * tokenizer, int parent_index, const char * child, char * result, int result_len) {
    int i = 0;
    int count = 0;
    jsmntok_t * root_token = NULL;
    int child_len = 0;
    int val_len = 0;
    int tok_index = 0;

    IF_NULL_RETURN(tokenizer, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(tokenizer->tokens, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(tokenizer->json_str, TC_IOT_NULL_POINTER);

    root_token = tokenizer->tokens;
    count = tokenizer->used_count;

    if (parent_index >= count) {
        TC_IOT_LOG_ERROR("parent_index=%d, count=%d", parent_index, count);
        return TC_IOT_INVALID_PARAMETER;
    }

    if (root_token[parent_index].type != JSMN_OBJECT) {
        TC_IOT_LOG_ERROR("parent node is not an array or array, type=%d",root_token[parent_index].type);
        return TC_IOT_INVALID_PARAMETER;
    }

    child_len = strlen(child);
    for (i = parent_index+1; i < count; i++) {
        if (root_token[i].parent == parent_index) {
            if (tc_iot_jsoneq_len(tokenizer->json_str,
                                  &root_token[i], child,
                                  child_len) == 0)
            {
                tok_index = i+1;

                if (result && result_len) {
                    val_len = root_token[tok_index].end -
                        root_token[tok_index].start;
                    if (val_len >= result_len) {
                        TC_IOT_LOG_ERROR("result buffer not enough val_len=%d, result_len=%d", val_len, result_len);
                        return TC_IOT_BUFFER_OVERFLOW;
                    }

                    tc_iot_json_unescape(result, result_len,
                                         tokenizer->json_str + root_token[tok_index].start,
                                         val_len);
                    if (val_len < result_len) {
                        result[val_len] = 0;
                    }
                }
                /* TC_IOT_LOG_TRACE("result=%s, index=%d", tc_iot_log_summary_string(result,val_len), tok_index); */
                return tok_index;
            }
        }
    }

    TC_IOT_LOG_ERROR("child [%s] not found, node size=%d.", child, root_token[parent_index].size);
    return TC_IOT_INVALID_PARAMETER;
}
