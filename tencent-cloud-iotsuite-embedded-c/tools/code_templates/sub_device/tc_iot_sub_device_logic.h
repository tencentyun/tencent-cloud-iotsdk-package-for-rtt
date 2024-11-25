#ifndef TC_IOT_SUB_DEVICE_LOGIC_H
#define TC_IOT_SUB_DEVICE_LOGIC_H

#include "tc_iot_inc.h"

#define TC_IOT_GW_MAX_SUB_DEVICE_COUNT  10

/* 数据模板本地存储结构定义 local data struct define */
/*${data_template.declare_subdev_local_data_struct()}*/

/* 数据模板字段 ID 宏定义*/
/*${data_template.declare_subdev_local_data_field_id()}*/

/*${data_template.subdev_property_def_declare()}*/;
/*${data_template.subdev_local_data_declare()}*/;

#endif /* TC_IOT_SUB_DEVICE_LOGIC_H */
