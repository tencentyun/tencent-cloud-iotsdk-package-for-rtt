#include "tc_iot_device_config.h"
#include "tc_iot_device_logic.h"
#include "tc_iot_sub_device_logic.h"
#include "tc_iot_export.h"

tc_iot_shadow_local_data_subdev01 g_tc_iot_shadow_local_data_subdev01[TC_IOT_GW_MAX_SUB_DEVICE_COUNT];
tc_iot_shadow_property_def g_tc_iot_shadow_property_defs_subdev01[TC_IOT_PROP_TOTAL_subdev01] = {
    TC_IOT_DECLARE_LOCAL_MEMBER_DEF(subdev01, param_bool,    TC_IOT_SHADOW_TYPE_BOOL),
    TC_IOT_DECLARE_LOCAL_MEMBER_DEF(subdev01, param_enum,    TC_IOT_SHADOW_TYPE_ENUM),
    TC_IOT_DECLARE_LOCAL_MEMBER_DEF(subdev01, param_number,    TC_IOT_SHADOW_TYPE_NUMBER),
    TC_IOT_DECLARE_LOCAL_MEMBER_DEF(subdev01, Temperature,    TC_IOT_SHADOW_TYPE_NUMBER),
    TC_IOT_DECLARE_LOCAL_MEMBER_DEF(subdev01, param_string,    TC_IOT_SHADOW_TYPE_STRING),
};


tc_iot_shadow_local_data_subdev02 g_tc_iot_shadow_local_data_subdev02[TC_IOT_GW_MAX_SUB_DEVICE_COUNT];
tc_iot_shadow_property_def g_tc_iot_shadow_property_defs_subdev02[TC_IOT_PROP_TOTAL_subdev02] = {
    TC_IOT_DECLARE_LOCAL_MEMBER_DEF(subdev02, param_bool,    TC_IOT_SHADOW_TYPE_BOOL),
    TC_IOT_DECLARE_LOCAL_MEMBER_DEF(subdev02, param_enum,    TC_IOT_SHADOW_TYPE_ENUM),
    TC_IOT_DECLARE_LOCAL_MEMBER_DEF(subdev02, param_number,    TC_IOT_SHADOW_TYPE_NUMBER),
    TC_IOT_DECLARE_LOCAL_MEMBER_DEF(subdev02, Temperature,    TC_IOT_SHADOW_TYPE_NUMBER),
    TC_IOT_DECLARE_LOCAL_MEMBER_DEF(subdev02, param_string,    TC_IOT_SHADOW_TYPE_STRING),
};

