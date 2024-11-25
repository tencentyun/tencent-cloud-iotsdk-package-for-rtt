# 操作指南
## 准备工作
1. 参见 [开发准备](https://github.com/tencentyun/tencent-cloud-iotsuite-embedded-c/blob/master/README.md) ；
2. 创建网关产品，“数据协议”选择“数据模板”，进入【数据模板】页，为产品定义数据模板，可根据实际应用场景，自行定义所需的数据点及数据类型。例如，本示例默认定义了如下参数；

| 名称         | 类型       | 读写       | 取值范围             |
| ----------   | ---------- | ---------- | ----------           |
| param_bool   | 布尔       | 可写       | 无需填写             |
| param_enum   | 枚举       | 可写       | enum_a,enum_b,enum_c |
| param_number | 数值       | 可写       | 0,4095               |
| param_string | 字符串     | 可写       | 64                   |

3. 进入【基本信息】，点击【导出】，导出 iot-xxxxx.json 文档，将 iot-xxxxx.json 文档放到 examples/advanced_edition/sub_dev 目录下，覆盖 iot-product.json 文件。

4. 创建子设备产品，“数据协议”选择“数据模板”，进入【数据模板】页，为产品定义数据模板，可根据实际应用场景，自行定义所需的数据点及数据类型。例如，本示例默认定义了如下参数；

| 名称         | 类型       | 读写       | 取值范围             |
| ----------   | ---------- | ---------- | ----------           |
| param_bool   | 布尔       | 可写       | 无需填写             |
| param_enum   | 枚举       | 可写       | enum_a,enum_b,enum_c |
| param_number | 数值       | 可写       | 0,4095               |
| param_string | 字符串     | 可写       | 64                   |

5. 进入【基本信息】，点击【导出】，导出 iot-xxxxx.json 文档，将 iot-xxxxx.json 文档放到 examples/advanced_edition/sub_dev 目录下，覆盖 iot-product-subdev01.json 文件。有多种子设备时，依次命名为 subdev02, subdev03 ... subdevNN.

6. 通过脚本自动生成设备的逻辑框架及业务数据配置代码。

```shell
# 进入工具脚本目录
# ** 单一产品子设备 **
cd tools
python tc_iot_code_generator.py -c ../examples/advanced_edition/sub_device/iot-product.json code_templates/sub_device/app_main.c code_templates/sub_device/tc_iot_device_*
python tc_iot_code_generator.py -c ../examples/advanced_edition/sub_device/iot-product-subdev.json code_templates/sub_device/tc_iot_sub_device_logic.*

# 多类型产品子设备代码生成，可 参见 subdev_generate.sh ，修改配置后，执行 subdev_generate.sh 脚本

```

执行成功后会看到有如下提示信息：
```shell
加载 ../examples/advanced_edition/sub_device/iot-product.json 文件成功
文件 ../examples/advanced_edition/sub_device/app_main.c 生成成功
文件 ../examples/advanced_edition/sub_device/tc_iot_device_config.h 生成成功
文件 ../examples/advanced_edition/sub_device/tc_iot_device_logic.c 生成成功
文件 ../examples/advanced_edition/sub_device/tc_iot_device_logic.h 生成成功
加载 ../examples/advanced_edition/sub_device/iot-product-subdev.json 文件成功
文件 ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic.c 生成成功
文件 ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic.h 生成成功

```

5. 修改 tc_iot_device_config.h 配置，设置 Device Name 和 Device Secret：
```c
/* 设备密钥，可以在产品“设备管理”->“设备证书”->“Device Secret”位置找到*/
#define TC_IOT_CONFIG_DEVICE_SECRET "00000000000000000000000000000000"

/* 设备名称，可以在产品“设备管理”->“设备名称”位置找到*/
#define TC_IOT_CONFIG_DEVICE_NAME "device_name"
```

6. 修改 app_main.c，增加子设备注册逻辑：

```c
    /** TODO: 子设备注册 begin */
    /*
    tc_iot_sub_device_register(&g_tc_iot_sub_device_table, 
            // 修改为指定的子设备产品 ID、设备名、设备密钥
            "subdev01_product_id", "subdev01_device_name", "subdev01_device_secret",
            TC_IOT_ARRAY_LENGTH(g_tc_iot_shadow_property_defs_subdev01),
            &g_tc_iot_shadow_property_defs_subdev01[0],
            &g_tc_iot_shadow_local_data_subdev01[0]);
    //...

    */
    /** TODO: 子设备注册 end */
```

## 编译程序
代码及配置生成成功后，进入 build 目录，开始编译。

```shell
cd ../build
make
```

## 运行程序

编译完成后，在 build/bin/ 目录下，会产生一个 advanced_sub_device 程序。

```shell
# 运行 advanced_sub_device 设备端应用，
# 此种方式运行，可以有效查看日志及抓包定位问题
# 备注：
# -d device_xxxx 参数是指定当前连接使用的设备名
# 如果已经在 tc_iot_device_config.h 中，为TC_IOT_CONFIG_DEVICE_NAME 指定了
# 正确的设备名称，则命令行执行时，可以不用指定 -d device_xxx 参数。
#
# -s 指定Device Secret
# 如果已经在 tc_iot_device_config.h 中，为TC_IOT_CONFIG_DEVICE_SECRET 指定了
# 正确的Device Secret，则命令行执行时，可以不用指定 -s secret_abc 参数。
#
# -A "product_id,device_name,device_secret" 指定添加的子设备信息（只能注册subdev01）
# 参数由子设备的 Product Id、Device Name、Device Secret 合并而成，需要添加多个子设备时，可多次用 -A 指定添加。
#
# ./advanced_sub_device --trace -p 1883

./advanced_sub_device -d device_xxxx -s secret_abc --trace -p 1883

# 如运行正常未见异常
# 也可用默认模式来执行，避免日志干扰
./advanced_sub_device -d device_xxxx

```

## 模拟APP控制端发送控制指令
在控制台的【设备管理】中，点击【下发指令】，即可修改控制参数，下发推送给设备。

## SDK API 样例及说明

### 1. 初始化 
根据设备配置参数，初始化服务。此函数调用后：
1. 初始化相关结构变量，和服务端建立 MQTT 连接；
2. 默认订阅数据模板相关 Topic；

#### 样例

```c
    int ret = 0;
    ret = tc_iot_shadow_construct(tc_iot_get_shadow_client(), &g_tc_iot_shadow_config);
```

#### 函数原型及说明

```c
/**
 * @brief tc_iot_shadow_construct 构造设备影子或网关对象
 *
 * @param p_shadow_client 设备影子或网关对象
 * @param p_config 初始化设备影子或网关对象所需参数配置
 *
 * @return 结果返回码
 * @see tc_iot_sys_code_e
 */
int tc_iot_shadow_construct(tc_iot_shadow_client * p_shadow_client,
                            tc_iot_shadow_config *p_config);
```

### 2. 主循环 
tc_iot_shadow_yield 服务任务主循环函数，接收服务推送及响应数据。 此函数调用后：
1. 接收服务端下发的控制指令或响应消息，并调用业务回调函数处理；
2. 定时触发心跳逻辑，保持连接；
3. 检测网络连接状态，异常时自动重连；

#### 样例

```c
    while (!stop) {
        tc_iot_shadow_yield(tc_iot_get_shadow_client(), 200);
    }
```

#### 函数原型及说明

```c
/**
 * @brief tc_iot_shadow_yield 在当前线程为底层服务，让出一定 CPU 执
 * 行时间，接收服务推送及响应数据
 *
 * @param  p_shadow_client 设备影子或网关对象
 * @param timeout_ms 等待时延，单位毫秒
 *
 * @return 结果返回码
 * @see tc_iot_sys_code_e
 */
int tc_iot_shadow_yield(tc_iot_shadow_client *p_shadow_client, int timeout_ms);
```

### 3. 析构资源
 tc_iot_shadow_destroy 数据模板服务析构处理，释放资源。

#### 样例

```c
    tc_iot_shadow_destroy(tc_iot_get_shadow_client());
```

#### 函数原型及说明

```c

/**
 * @brief tc_iot_shadow_destroy 关闭 Shadow client 连接，并销毁 Shadow client
 *
 * @param p_shadow_client 设备影子对象
 */
int tc_iot_shadow_destroy(tc_iot_shadow_client *p_shadow_client);
```

### 4. 子设备上下线
 tc_iot_sub_device_onoffline 对指定范围的子设备，进行上线或下线处理。

#### 样例

```c
    // 子设备上线，第4个参数带 true 表示上线。
    ret = tc_iot_sub_device_onoffline(tc_iot_get_shadow_client(),&g_tc_iot_sub_device_table.items[0],g_tc_iot_sub_device_table.used,true);

    // 子设备下线，第4个参数带 false 表示下线。
    ret = tc_iot_sub_device_onoffline(tc_iot_get_shadow_client(),&g_tc_iot_sub_device_table.items[0],g_tc_iot_sub_device_table.used,false);
```

#### 函数原型及说明

```c
/**
 * @brief 对指定范围的子设备，进行上线或下线处理。
 *
 * @param p_shadow_client 设备影子或网关对象
 * @param sub_devices 子设备列表
 * @param sub_devices_count 子设备数量
 * @param is_online 是否上线，true 表示上线，false 表示下线
 *
 * @return 结果返回码
 * @see tc_iot_sys_code_e
 */
int tc_iot_sub_device_onoffline(tc_iot_shadow_client * c, tc_iot_sub_device_info * sub_devices, int sub_devices_count, bool is_online);
```

### 5. 子设备数据上报
tc_iot_sub_device_report 对指定范围的子设备，进行上线或下线处理。

#### 样例

```c
    // 设定每个子设备需要上报的字段
    tc_iot_sub_device_mark_report_field(&g_tc_iot_sub_device_table, "product_id", "device_name", "field name");
    
    // 批量上报子设备数据
    tc_iot_sub_device_report(client, &g_tc_iot_sub_device_table.items[0],  g_tc_iot_sub_device_table.used);

    // 设定每个子设备已完成同步，需要清除服务端对应状态的字段。
    tc_iot_sub_device_mark_confirm_field(&g_tc_iot_sub_device_table, "product_id", "device_name", "field name");

    // 批量确定设备数据
    tc_iot_sub_device_confirm(client, &g_tc_iot_sub_device_table.items[0],  g_tc_iot_sub_device_table.used);
```

#### 函数原型及说明

```c

/**
 * @brief 标记某个子设备需要上报的数据点。
 *
 * @param t 网关子设备列表
 * @param product_id 子设备 Product Id
 * @param device_name 子设备 Device Name
 * @param field_name 子设备数据点名称
 *
 * @return 字段属性，为 NULL 则表示标记失败。
 */
tc_iot_shadow_property_def * tc_iot_sub_device_mark_report_field(tc_iot_sub_device_table * t,
                                                                  const char * product_id,
                                                                  const char * device_name,
                                                                  const char * field_name);

/**
 * @brief 标记某个子设备需要确认的数据点。
 *
 * @param t 网关子设备列表
 * @param product_id 子设备 Product Id
 * @param device_name 子设备 Device Name
 * @param field_name 子设备数据点名称
 *
 * @return 字段属性，为 NULL 则表示标记失败。
 */
tc_iot_shadow_property_def * tc_iot_sub_device_mark_confirm_field(tc_iot_sub_device_table * t,
                                                                  const char * product_id,
                                                                  const char * device_name,
                                                                  const char * field_name);
/**
 * @brief 对指定范围的子设备，检查 reported_bits 位域设置，上报对应字段的数据。
 *
 * @param p_shadow_client 设备影子或网关对象
 * @param sub_devices 子设备列表
 * @param sub_devices_count 子设备数量
 *
 * @return 结果返回码
 * @see tc_iot_sys_code_e
 */
int tc_iot_sub_device_report(tc_iot_shadow_client * c, tc_iot_sub_device_info * sub_devices, int sub_devices_count);

/**
 * @brief 对指定范围的子设备，检查 desired_bits 位域设置，确认对应字段的数据，清除服务端 desired 状态。
 *
 * @param p_shadow_client 设备影子或网关对象
 * @param sub_devices 子设备列表
 * @param sub_devices_count 子设备数量
 *
 * @return 结果返回码
 * @see tc_iot_sys_code_e
 */
int tc_iot_sub_device_confirm(tc_iot_shadow_client * c, tc_iot_sub_device_info * sub_devices, int sub_devices_count);
```
