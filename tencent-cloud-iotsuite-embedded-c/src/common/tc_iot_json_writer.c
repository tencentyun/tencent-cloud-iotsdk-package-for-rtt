#include "tc_iot_inc.h"

/**
 * @brief    加载临时缓存到 json_writer
 *
 * @details  为json writer 指定数据缓存区，及初始相关状态。
 *
 * @param    w 待初始化的 json writer
 * @param    buffer 数据缓存区
 * @param    buffer_len 数据缓存区长度
 *
 * @return   返回码
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_load(tc_iot_json_writer * w, char * buffer, int buffer_len) {
    int ret = 0;

    IF_NULL_RETURN(w, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(buffer, TC_IOT_NULL_POINTER);
    if (buffer_len <= 0) {
        TC_IOT_LOG_ERROR("buffer_len=%d", buffer_len);
        return TC_IOT_INVALID_PARAMETER;
    }
    w->buffer = buffer;
    w->buffer_len = buffer_len;
    w->pos = 0;

    return ret;
}

/**
 * @brief    初始化 json_writer
 *
 * @details  为json writer 指定数据缓存区，及初始相关状态。
 *
 * @param    w 待初始化的 json writer
 * @param    buffer 数据缓存区
 * @param    buffer_len 数据缓存区长度
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_open(tc_iot_json_writer * w, char * buffer, int buffer_len) {
    int ret = 0;

    ret = tc_iot_json_writer_load(w, buffer, buffer_len);
    if (ret < 0) {
        return ret;
    }

    ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, "{");
    w->pos += ret;
    return ret;
}

/**
 * @brief    关闭 json writer
 *
 * @details  关闭 json writer，主要是补齐json结束符 "}" 及字符串结束符 '\0'。
 *
 * @param    w 待关闭的 json writer
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_close(tc_iot_json_writer * w) {
    int ret = 0;

    IF_NULL_RETURN(w, TC_IOT_NULL_POINTER);
    IF_LESS_RETURN(w->buffer_len, w->pos+1, TC_IOT_BUFFER_OVERFLOW);

    ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, "}");
    w->pos += ret;
    if (w->pos >= w->buffer_len) {
        return TC_IOT_BUFFER_OVERFLOW;
    } else {
        w->buffer[w->pos] = '\0';
        return ret;
    }
}

/**
 * @brief    开始写入 json object 对象。
 *
 * @details  开始写入 json object 对象，主要是写入： "name":{ 。
 *           tc_iot_json_writer_object_begin 和 tc_iot_json_writer_object_end 要成对出现。
 *
 * @param    w json writer
 * @param    name 对象名称
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_object_begin(tc_iot_json_writer * w, const char * name) {
    return tc_iot_json_writer_raw_data(w, name, "{");
}

/**
 * @brief    完成 json object 对象写入。
 *
 * @details  完成 json object 对象写入，主要是写入：}
 *
 * @param    w json writer
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_object_end(tc_iot_json_writer * w) {
    int ret = 0;

    IF_NULL_RETURN(w, TC_IOT_NULL_POINTER);
    IF_LESS_RETURN(w->buffer_len, w->pos+1, TC_IOT_BUFFER_OVERFLOW);

    ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, "}");
    w->pos += ret;
    if (w->pos == w->buffer_len) {
        return TC_IOT_BUFFER_OVERFLOW;
    } else {
        w->buffer[w->pos] = '\0';
        return ret;
    }
}

/**
 * @brief    开始写入 json array 数组。
 *
 * @details  开始写入 json array 数组，主要是写入： "name":[ 。
 *           tc_iot_json_writer_array_begin 和 tc_iot_json_writer_array_end 要成对出现。
 *
 * @param    w json writer
 * @param    name 数组名称
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_array_begin(tc_iot_json_writer * w, const char * name) {
    return tc_iot_json_writer_raw_data(w, name, "[");
}

/**
 * @brief    完成 json array 数组写入。
 *
 * @details  完成 json array 数组写入，主要是写入：]
 *
 * @param    w json writer
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_array_end(tc_iot_json_writer * w) {
    int ret = 0;

    IF_NULL_RETURN(w, TC_IOT_NULL_POINTER);
    IF_LESS_RETURN(w->buffer_len, w->pos+1, TC_IOT_BUFFER_OVERFLOW);

    ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, "]");
    w->pos += ret;
    if (w->pos == w->buffer_len) {
        return TC_IOT_BUFFER_OVERFLOW;
    } else {
        w->buffer[w->pos] = '\0';
        return ret;
    }
}

/**
 * @brief    获得 json writer 的 buffer 缓冲区
 *
 * @details  获得 json writer 的 buffer 缓冲区，返回地址。
 *
 * @param    w json writer
 *
 * @return   buffer 地址
 */
char * tc_iot_json_writer_buffer(tc_iot_json_writer * w) {
    if (!w) {
        return NULL;
    } else {
        return w->buffer;
    }
}

/**
 * @brief    写入一个字符类型的数据。
 *
 * @details  写入字符类型数据，"name":"value" 。
 *
 * @param    w json writer
 * @param    name 名称
 * @param    value 取值
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_string(tc_iot_json_writer * w, const char * name, const char * value) {
    return tc_iot_json_writer_format_data(w, name, "\"%J\"", value);
}

/**
 * @brief    写入一个整型的数据。
 *
 * @details  写入整型数据，"name":value 。
 *
 * @param    w json writer
 * @param    name 名称
 * @param    value 取值
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_int(tc_iot_json_writer * w, const char * name, int value) {
    return tc_iot_json_writer_format_data(w, name, "%d", &value);
}

/**
 * @brief    写入一个无符号整型的数据。
 *
 * @details  写入无符号整型数据，"name":value 。
 *
 * @param    w json writer
 * @param    name 名称
 * @param    value 取值
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_uint(tc_iot_json_writer * w, const char * name, unsigned int value) {
    return tc_iot_json_writer_format_data(w, name, "%u", &value);
}

/**
 * @brief    写入一个浮点型的数据。
 *
 * @details  写入浮点型数据，"name":value 。
 *
 * @param    w json writer
 * @param    name 名称
 * @param    value 取值
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_decimal(tc_iot_json_writer * w, const char * name, double value) {
    return tc_iot_json_writer_format_data(w, name, "%f", &value);
}

/**
 * @brief    写入一个布尔型的数据。
 *
 * @details  写入布尔型数据，"name":true/false 。
 *
 * @param    w json writer
 * @param    name 名称
 * @param    value 取值
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_bool(tc_iot_json_writer * w, const char * name, bool value) {
    if (value) {
        return tc_iot_json_writer_raw_data(w, name, "true");
    } else {
        return tc_iot_json_writer_raw_data(w, name, "false");
    }
}

/**
 * @brief    写入一个空对象数据。
 *
 * @details  写入空对象数据，"name":null 。
 *
 * @param    w json writer
 * @param    name 名称
 * @param    value 取值
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_null(tc_iot_json_writer * w, const char * name) {
    return tc_iot_json_writer_raw_data(w, name, "null");
}

/**
 * @brief    写入一个原始字符串数据。
 *
 * @details  写入原始字符串数据，"name":value 。一般用来写入已经预处理好的，格式化的符合类型 JSON 数据。
 *           数据必须是预先格式化并做好转义的。
 *
 * @param    w json writer
 * @param    name 名称
 * @param    value 取值
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_raw_data(tc_iot_json_writer * w, const char * name, const char * value) {
    return tc_iot_json_writer_format_data(w,name,"%s", value);
}

/**
 * @brief    写入一连串的 JSON 反括号，完整序列化 JSON 串。
 *
 * @details  写入原始字符串数据，例如：]}]}。一般用来写入已经预处理好的，格式化的符合类型 JSON 数据。
 *           数据必须是预先格式化并做好转义的。
 *
 * @param    w json writer
 * @param    name 名称
 * @param    value 取值
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_raw_end_quote(tc_iot_json_writer * w, const char * end_quotes) {
    int ret = 0;

    ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, "%s", end_quotes);
    w->pos += ret;
    if (w->pos == w->buffer_len) {
        TC_IOT_LOG_ERROR("buffer overflow processing:%s", end_quotes);
        return TC_IOT_BUFFER_OVERFLOW;
    }

    return ret;
}

/**
 * @brief    格式化写入数据。
 *
 * @details  格式化写入数据，根据 format 中指定的格式，来写入数据。例如，
 *           要写入整型数据：
 *              tc_iot_json_writer_format_data(w,"name_integer", "%d", &intvalue);
 *           要写入浮点数据：
 *              tc_iot_json_writer_format_data(w,"name_float", "%f", &double_value);
 *           要写字符型数据：
 *              tc_iot_json_writer_format_data(w,"name_string", "\"%s\"", str);
 *
 * @param    w json writer
 * @param    name 名称
 * @param    format 数据格式化字符串。
 * @param    value_ptr 数据地址。
 *
 * @return   > 0 时，表示处理过程中，往 buffer 区域写入的字节数。 < 0，表示出错
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_format_data(tc_iot_json_writer * w, const char * name, const char * format, const void * value_ptr) {
    int ret = 0;
    const char * type_field = format;
    int old_pos = 0;

    IF_NULL_RETURN(w, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(format, TC_IOT_NULL_POINTER);
    IF_LESS_RETURN(w->buffer_len, w->pos+1, TC_IOT_BUFFER_OVERFLOW);
    IF_LESS_RETURN(w->pos, 0, TC_IOT_JSON_PARSE_FAILED);

    old_pos = w->pos;
    if (w->buffer[w->pos-1] != '{' && w->buffer[w->pos-1] != '[' ) {
        ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, ",");
        w->pos += ret;
    } else {
        /* TC_IOT_LOG_TRACE("%c|name=%s,v=%s,0x%x", w->buffer[w->pos-1], name, w->buffer-5, w->buffer); */
    }

    if (name) {
        ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, "\"");
        w->pos += ret;
        if (w->pos == w->buffer_len) {
            TC_IOT_LOG_ERROR("buffer overflow processing:%s", name);
            return TC_IOT_BUFFER_OVERFLOW;
        }
        ret = tc_iot_json_escape(&w->buffer[w->pos], w->buffer_len-w->pos, name, strlen(name));
        w->pos += ret;
        if (w->pos == w->buffer_len) {
            TC_IOT_LOG_ERROR("buffer overflow processing:%s", name);
            return TC_IOT_BUFFER_OVERFLOW;
        }

        ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, "\":");
        w->pos += ret;
        if (w->pos == w->buffer_len) {
            TC_IOT_LOG_ERROR("buffer overflow processing:%s", name);
            return TC_IOT_BUFFER_OVERFLOW;
        }
    }

    while(*type_field) {
        if ('%' == *type_field) {
            break;
        } else {
            ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, "%c", *type_field);
            w->pos += ret;
            if (w->pos == w->buffer_len) {
                TC_IOT_LOG_ERROR("buffer overflow processing:%s", name);
                return TC_IOT_BUFFER_OVERFLOW;
            }
        }
        type_field++;
    }

    switch(type_field[1]) {
    case 's':
        // print raw string to buffer
        ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, "%s", (char *)value_ptr);

        // if has extra char after %s, append to buffer too.
        if (type_field[2]) {
            w->pos += ret;
            if (w->pos == w->buffer_len) {
                TC_IOT_LOG_ERROR("buffer overflow processing:%s", name);
                return TC_IOT_BUFFER_OVERFLOW;
            }
            ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, "%s", type_field+2);
        }
        break;
    case 'J':
        // escape string as json format
        ret = tc_iot_json_escape(&w->buffer[w->pos], w->buffer_len-w->pos, value_ptr, strlen(value_ptr));

        // if has extra char after %s, append to buffer too.
        if (type_field[2]) {
            w->pos += ret;
            if (w->pos == w->buffer_len) {
                TC_IOT_LOG_ERROR("buffer overflow processing:%s", name);
                return TC_IOT_BUFFER_OVERFLOW;
            }
            ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, "%s", type_field+2);
        }
        break;
    case 'd':
    case 'i':
        ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, format, *(int *)value_ptr);
        break;
    case 'o':
    case 'u':
    case 'x':
    case 'X':
        ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, format, *(unsigned int *)value_ptr);
        break;
    case 'l':
        if (type_field[2] == 'l') {
            ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, format, *(long long *)value_ptr);
        } else {
            ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, format, *(long *)value_ptr);
        }
        break;
    case 'f':
    case 'g':
    case 'F':
    case 'G':
        ret = tc_iot_hal_snprintf(&w->buffer[w->pos], w->buffer_len-w->pos, format, *(double *)value_ptr);
        break;
    default:
        TC_IOT_LOG_ERROR("invalid format string=%s", format);
        return TC_IOT_INVALID_PARAMETER;
    }

    w->pos += ret;
    if (w->pos == w->buffer_len) {
        TC_IOT_LOG_ERROR("buffer overflow processing:%s", name);
        return TC_IOT_BUFFER_OVERFLOW;
    }

    return w->pos - old_pos;
}
