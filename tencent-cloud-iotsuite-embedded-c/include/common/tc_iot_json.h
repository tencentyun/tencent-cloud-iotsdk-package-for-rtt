#ifndef TC_IOT_JSON_H
#define TC_IOT_JSON_H

#include "tc_iot_inc.h"

int tc_iot_jsoneq(const char *json, jsmntok_t *tok, const char *s);
int tc_iot_jsoneq_len(const char *json, const jsmntok_t *tok, const char *s,
                      int len);
int tc_iot_json_unescape(char *dest, int dest_len, const char *src, int src_len);
char * tc_iot_json_inline_escape(char *dest, int dest_len, const char *src);

int tc_iot_json_escape(char *dest, int dest_len, const char *src, int src_len);
int tc_iot_json_parse(const char * json, int json_len, jsmntok_t * tokens, int token_count);
const char * tc_iot_json_token_type_str(int type);
void tc_iot_json_print_node(const char *prefix, const char *json, const jsmntok_t *node_root, int node_index);

int tc_iot_json_find_token(const char *json, const jsmntok_t *root_token,
                           int count, const char *path, char *result,
                           int result_len);

typedef struct _tc_iot_json_tokenizer {
    int max_count;
    int used_count;
    jsmntok_t * tokens;
    const char * json_str;
    int json_str_len;
} tc_iot_json_tokenizer;

int tc_iot_json_tokenizer_load(tc_iot_json_tokenizer * tokenizer, const char * json, int json_len, jsmntok_t * tokens, int token_count);
int tc_iot_json_tokenizer_nth_child(tc_iot_json_tokenizer * tokenizer, int parent_index, int nth);
int tc_iot_json_tokenizer_nth_child_value(char * value, int value_len, tc_iot_json_tokenizer * tokenizer, int parent_index, int nth);
int tc_iot_json_tokenizer_find_child(tc_iot_json_tokenizer * tokenizer, int parent_index, const char * child, char * result, int result_len);
jsmntok_t * tc_iot_json_tokenizer_get_token(tc_iot_json_tokenizer * tokenizer, int index);
const char * tc_iot_json_tokenizer_get_str_start(tc_iot_json_tokenizer * tokenizer, int index);
int tc_iot_json_tokenizer_get_str_len(tc_iot_json_tokenizer * tokenizer, int index);
int tc_iot_json_tokenizer_get_child_count(tc_iot_json_tokenizer * tokenizer, int index);

#endif /* end of include guard */
