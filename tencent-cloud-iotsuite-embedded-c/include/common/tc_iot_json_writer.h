#ifndef TC_IOT_JSON_WRITER_H
#define TC_IOT_JSON_WRITER_H

#define TC_IOT_JSON_NULL "null"
#define TC_IOT_JSON_TRUE "true"
#define TC_IOT_JSON_FALSE "false"


/*
 * tc_iot_json_writer 用来往指定 buffer 中写入 json 数据，
 * 写入数据时，自动对数据进行转义及 buffer 溢出判断处理。
 * 简化 json 组包逻辑。
 * */
typedef struct _tc_iot_json_writer {
    char * buffer;
    int buffer_len;
    int pos;
}tc_iot_json_writer;


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
int tc_iot_json_writer_load(tc_iot_json_writer * w, char * buffer, int buffer_len);

/**
 * @brief 初始化 json writer 数据，指定待写入的 buffer
 *
 * @param w writer 对象
 * @param buffer 待写入的 buffer 区域
 * @param buffer_len buffer 长度
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_open(tc_iot_json_writer * w, char * buffer, int buffer_len);

/**
 * @brief 结束 json writer 写入。
 *
 * @param w writer 对象
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_close(tc_iot_json_writer * w);


/**
 * @brief 开始写入 object 对象
 *
 * @param w writer 对象
 * @param name object 对象名称
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_object_begin(tc_iot_json_writer * w, const char * name);

/**
 * @brief 结束写入 object 对象
 *
 * @param w writer 对象
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_object_end(tc_iot_json_writer * w);

/**
 * @brief 开始写入 array 数组
 *
 * @param w writer 对象
 * @param name array 对象名称
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_array_begin(tc_iot_json_writer * w, const char * name);

/**
 * @brief 结束写入 array 数组
 *
 * @param w writer 对象
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_array_end(tc_iot_json_writer * w);

/**
 * @brief 获得 writer 缓存区地址
 *
 * @param w writer 对象
 *
 * @return buffer 地址
 */
char * tc_iot_json_writer_buffer(tc_iot_json_writer * w);

/**
 * @brief 写入字符串类型数据
 *
 * @param w writer 对象
 * @param name 名称
 * @param value 取值
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_string(tc_iot_json_writer * w, const char * name, const char * value);

/**
 * @brief 写入符号整型数据
 *
 * @param w writer 对象
 * @param name 名称
 * @param value 取值
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_int(tc_iot_json_writer * w, const char * name, int value);

/**
 * @brief 写入无符号整型数据
 *
 * @param w writer 对象
 * @param name 名称
 * @param value 取值
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_uint(tc_iot_json_writer * w, const char * name, unsigned int value);

/**
 * @brief 写入浮点数据
 *
 * @param w writer 对象
 * @param name 名称
 * @param value 取值
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_decimal(tc_iot_json_writer * w, const char * name, double value);

/**
 * @brief 写入布尔类型数据
 *
 * @param w writer 对象
 * @param name 名称
 * @param value 取值
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_bool(tc_iot_json_writer * w, const char * name, bool value);

/**
 * @brief 写入 null 数据
 *
 * @param w writer 对象
 * @param name 名称
 * @param value 取值
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_null(tc_iot_json_writer * w, const char * name);

/**
 * @brief 写入已格式化的有效 json 对象数据，对 value 不做任何转义
 *
 * @param w writer 对象
 * @param name 名称
 * @param value 取值
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_raw_data(tc_iot_json_writer * w, const char * name, const char * value);

/**
 * @brief 写入已格式化的有效 json 对象数据，对 value 不做任何转义，value 格式化支付穿
 *
 * @param w writer 对象
 * @param name 名称
 * @param format 写入格式, %s 以字符串格式写入，%d 以有符号整型写入 ， %f 以 double 类型写入
 * @param value 取值
 *
 * @return 结果返回码，> 0 时 为本次调用使用的buffer长度。
 * @see tc_iot_sys_code_e
 */
int tc_iot_json_writer_format_data(tc_iot_json_writer * w, const char * name, const char * format, const void * value_ptr);

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
int tc_iot_json_writer_raw_end_quote(tc_iot_json_writer * w, const char * end_quotes);

#endif /* TC_IOT_JSON_WRITER_H */
