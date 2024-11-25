#ifndef TC_IOT_SUB_DEVICE_LOGIC_H
#define TC_IOT_SUB_DEVICE_LOGIC_H

#include "tc_iot_inc.h"

#define TC_IOT_GW_MAX_SUB_DEVICE_COUNT  10

/* 数据模板本地存储结构定义 local data struct define */
typedef struct _tc_iot_shadow_local_data_subdev01 {
    tc_iot_shadow_bool param_bool;
    tc_iot_shadow_enum param_enum;
    tc_iot_shadow_number param_number;
    tc_iot_shadow_number Temperature;
    char param_string[64+1];
}tc_iot_shadow_local_data_subdev01;


/* 数据模板字段 ID 宏定义*/
typedef enum _tc_iot_shadow_local_enum_subdev01 {
    TC_IOT_PROP_subdev01_param_bool,
    TC_IOT_PROP_subdev01_param_enum,
    TC_IOT_PROP_subdev01_param_number,
    TC_IOT_PROP_subdev01_Temperature,
    TC_IOT_PROP_subdev01_param_string,
    TC_IOT_PROP_TOTAL_subdev01,
} tc_iot_shadow_local_enum_subdev01;


tc_iot_shadow_property_def g_tc_iot_shadow_property_defs_subdev01[TC_IOT_PROP_TOTAL_subdev01];
tc_iot_shadow_local_data_subdev01 g_tc_iot_shadow_local_data_subdev01[TC_IOT_GW_MAX_SUB_DEVICE_COUNT];


/* 数据模板本地存储结构定义 local data struct define */
typedef struct _tc_iot_shadow_local_data_subdev02 {
    tc_iot_shadow_bool param_bool;
    tc_iot_shadow_enum param_enum;
    tc_iot_shadow_number param_number;
    tc_iot_shadow_number Temperature;
    char param_string[64+1];
}tc_iot_shadow_local_data_subdev02;


/* 数据模板字段 ID 宏定义*/
typedef enum _tc_iot_shadow_local_enum_subdev02 {
    TC_IOT_PROP_subdev02_param_bool,
    TC_IOT_PROP_subdev02_param_enum,
    TC_IOT_PROP_subdev02_param_number,
    TC_IOT_PROP_subdev02_Temperature,
    TC_IOT_PROP_subdev02_param_string,
    TC_IOT_PROP_TOTAL_subdev02,
} tc_iot_shadow_local_enum_subdev02;


tc_iot_shadow_property_def g_tc_iot_shadow_property_defs_subdev02[TC_IOT_PROP_TOTAL_subdev02];
tc_iot_shadow_local_data_subdev02 g_tc_iot_shadow_local_data_subdev02[TC_IOT_GW_MAX_SUB_DEVICE_COUNT];

#endif /* TC_IOT_SUB_DEVICE_LOGIC_H */
