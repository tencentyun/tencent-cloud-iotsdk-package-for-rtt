#ifndef TC_IOT_STRING_UTILS_H
#define TC_IOT_STRING_UTILS_H

char * tc_iot_util_byte_to_hex( const unsigned char * bytes, int bytes_len, char * output, int max_output_len );
int tc_iot_util_hex_to_byte( const char * bytes, char * output, int max_output_len );
int tc_iot_try_parse_int(const char * str, int * num_char_count);
unsigned int tc_iot_try_parse_uint(const char * str, int * num_char_count);
int tc_iot_try_parse_hex(const char * str, int * num_char_count);

#endif /* end of include guard */
