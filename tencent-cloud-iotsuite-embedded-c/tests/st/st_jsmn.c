#include "tc_iot_export.h"

const char * jsmn_type_name(int type) {

    switch (type) {
    case JSMN_UNDEFINED:
        return "UNDEFINED";
    case JSMN_OBJECT:
        return "OBJECT";
    case JSMN_ARRAY:
        return "ARRAY";
    case JSMN_STRING:
        return "STRING";
    case JSMN_PRIMITIVE:
        return "PRIMITIVE";
    default:
        return "UNKNOWN";
    }
}

int main(int argc, char** argv) {
    int ret = 0;
    int i = 0;
    jsmntok_t tokens[1024];
    jsmntok_t * node = NULL;
    char buffer[1024];
    const char * json =
        "{\"code\":0,\"message\":\"Example message.\",\"subdev\":[{\"product\":\"iot-abc\",\"list\":[{\"d\":\"a\",\"seq\":1}]}]}";

    ret = tc_iot_json_parse(json, strlen(json), tokens, TC_IOT_ARRAY_LENGTH(tokens));
    if (ret < 0) {
        printf("failed to parse: %s\n", json);
        return 0;
    }

    for (i = 0; i < ret; i++) {
        node = &tokens[i];
        strncpy(buffer, json + node->start, node->end - node->start);
        buffer[node->end - node->start] = '\0';
        printf("%2d/parent=%2d,type=%10s,start=%3d,end=%3d,size=%2d %s\n",
               i,node->parent,jsmn_type_name(node->type), node->start, node->end, node->size,
               buffer);
    }
    return 0;
}
