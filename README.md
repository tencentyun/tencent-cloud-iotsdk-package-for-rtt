##  Tencent Iot-Kit for RTthread Package 

### 1 介绍

Tencent Iot-Kit for RTthread Package 是基于[腾讯云C-SDK](https://github.com/tencentyun/tencent-cloud-iotsuite-embedded-c.git)在RThread环境开发的软件包，基于该软件包提供的能力，可以实现腾讯云IOT平台的连接及应用。

### 1.1 SDK架构图
![sdk-architecture](https://user-images.githubusercontent.com/990858/46805530-dc9f8e00-cd97-11e8-888b-1dd1171bfc1a.png)

### 1.2 目录结构

| 名称            | 说明 |
| ----            | ---- |
| docs            | 文档目录 |
| tencent-cloud-iotsuite-embedded-c         | 腾讯云iotsuit源码 |
| ports           | 移植文件目录 |
| samples         | 示例文件目录 |
| LICENSE         | 许可证文件 |
| README.md       | 软件包使用说明 |
| SConscript      | RT-Thread 默认的构建脚本 |
### 1.3 SDK接口说明
以下是C SDK 提供的功能和对应 API，用于设备端编写业务逻辑，API 接口暂不支持多线程调用，在多线程环境下，请勿跨线程调用。 更加详细的接口功能说明请查看 [include/tc_iot_export.h](include/tc_iot_export.h) 中的注释。


### 1.4 许可证

MIT License 协议许可。

## 2 移植说明
### 2.1 硬件及操作系统平台抽象层（HAL 层）
SDK 抽象定义了硬件及操作系统平台抽象层（HAL 层），将所依赖的内存、定时器、网络传输交互等功能，
都封装在 HAL 层中，进行跨平台移植时，首先都需要根据对应平台的硬件及操作系统情况，对应适配或实现相关的功能。

按照SDK的设计，HAL层的移植目录放置应该是 tencent-cloud-iotsuite-embedded-c\src\platform\rtthread，但为了适配rtthread package的风格要求，移植文件放置于package根目录ports文件夹，sample是tencent-cloud-iotsuite-embedded-c\examples下示例基于rtthread移植后的示例。

Tencent Iot-Kit for RTthread Package软件包已完成基于Rtthread 平台的移植，移植基于RTthread SAL层提供的BSD 网络编程接口。
相关的头文件及源文件代码结构如下：
```shell
/src/platform
|-- rtthread                 # rtthread平台移植的目录，对应rtthread软件包要求的port目录
    |-- tc_iot_hal_net.c    # TCP 非加密直连方式网络接口实现
    |-- tc_iot_hal_os.c     # 延时、打印机时间戳实现
    |-- tc_iot_hal_timer.c  # 定时器相关实现
    |-- tc_iot_hal_tls.c    # TLS 加密网络接口实现
    |-- tc_iot_hal_udp.c    # UDP 接口实现
    |-- tc_iot_hal_dtls.c   # DTLS 加密网络接口实现
	|-- tc_iot_get_time.c   # NTP实现UTC时间获取
```

C-SDK 中提供的 HAL 层是基于 Linux 等 POSIX 体系系统的参考实现，但并不强耦合要求实现按照 POSIX 接口方式，移植时可根据目标系统的情况，灵活调整。

所有 HAL 层函数都在 include/platform/tc_iot_hal*.h 中进行声明，函数都以 tc_iot_hal为前缀。



## 3 软件包使用
### 3.1 RTthread配置
- RT-Thread env开发工具中使用 `menuconfig` 使能 tencent-iotkit 软件包，配置产品及设备信息，并根据产品需求配置相应选项

```shell
RT-Thread online packages  --->
    IoT - internet of things  --->
        IoT Cloud  --->
			--- Tencent-iotkit:  Tencent Cloud SDK for IoT platform                                                                                               
                   Select Auth Mode (AUTH BY TOKEN)  --->                                                                                              
             (mqtt-1egtdjqza) Config Product Key                                                                                                       
             (rt_thread_dev1) Config Device Name                                                                                                       
             (92b0e676cdd608c4fb56386967613764) Config Device Secret                                                                                   
             [*]   Enable MQTT                                                                                                                         
                     Select MQTT data type (Enable MQTT Advanced)  --->                                                                                
             [ ]     Enable MQTT SSL                                                                                                                   
             [ ]   Enable COAP                                                                                                                         
             [ ]   Enable SCENARIOS                                                                                                                    
             [ ]   Enable HTTP                                                                                                                         
                          Version (latest)  --->
```

- 如果使能TLS功能，还需要选择mbedtls软件包
```shell
RT-Thread online packages  --->
    security packages  --->
         --- mbedtls:An open source, portable, easy to use, readable and flexible SSL library                                                                
		[*]   Store the AES tables in ROM (NEW)                                                                                                             
		(2)   Maximum window size used (NEW)                                                                                                                
		(3584) Maxium fragment length in bytes (NEW)                                                                                                        
		[ ]   Enable a mbedtls client example (NEW)                                                                                                         
		[ ]   Enable Debug log output (NEW)                                                                                                                 
			version (latest)  --->
```

- 使用 `pkgs --update` 命令下载软件包

### 3.2 编译及运行
1. 使用命令 scons --target=xxx 输出对应的工程，编译 


2. 执行示例程序：

### 3.3 运行demo程序
系统启动后，在 MSH 中使用命令执行：

- MQTT数据模板例程

该示例程序演示了如何基于腾讯云数据模板，和云端实现布尔型、枚举型、数值型、字符串型数据交互，MSH 命令如下所示：

```shell
msh />tc_mqtt_shadow_example
TC MQTT Shadow Exmaple,TC_IOT_SDK_VER:2.6
TRACE tc_iot_server_init:375 [c->s] shadow_get{"method":"get","passthrough":{"sid":"89390000"},"metadata":false}
...
TRACE tc_iot_mqtt_refresh_dynamic_sign:1186 usename[rt_thread_dev1] password[productId=iot-1l60dtl0&nonce=1350022348&timestamp=1539578219&signature=d4bMuKE1NVvE1hjAPiXnXR
request username and password for mqtt success.
...
TRACE tc_iot_shadow_check_and_report:1035 report(first time): "param_bool":false
TRACE tc_iot_shadow_check_and_report:1035 report(first time): "param_enum":0
TRACE tc_iot_shadow_check_and_report:1035 report(first time): "param_number":0.000000
TRACE tc_iot_shadow_check_and_report:1035 report(first time): "param_string":""
TRACE tc_iot_shadow_check_and_report:1114 requesting with: {"method":"update","passthrough":{"sid":"196c0001"},"state":{"reported":{"param_bool":false,"param_enum":0,"param_number":0.000000,"param_string":""}}}
...
TRACE _tc_iot_report_message_ack_callback:47 [s->c] {"method":"reply","timestamp":1539578239,"payload":{"code":0,"status":"OK"},"passthrough":{"sid":"196c0001"}}
...
```
- coap 例程
```shell
msh />tc_coap_advance_example
tc_coap_advance_example,TC_IOT_SDK_VERSION=2.9msh 
CoAP Server: gz.coap.tencentcloudapi.com:5683
TRACE tc_iot_coap_construct:482 mqtt client buf_size=1152,readbuf_size=1152,
...........
TRACE tc_iot_coap_rpc:875 send message success, sent size=226
TRACE tc_iot_coap_deserialize:299 received: ver=1,type=ACK,tkl=4,code=2.01-CREATED,message_id=2
TRACE tc_iot_coap_deserialize:336 Option=12-CONTENT_FORMAT
TRACE tc_iot_coap_deserialize:336 Option=14-MAX_AGE
TRACE tc_iot_coap_yield:683 Response for request mid=2, Total timer left=3730ms
[s->c]:{"method":"reply","payload":{"code":0,"status":"OK","state":{"desired":{}}},"timestamp":1540456355,"sequence":2063}
TRACE coap_check_and_process_desired:308 MEM!json_token[TC_IOT_MAX_JSON_TOKEN_COUNT],total=2400,used=340,left=2060
TRACE coap_check_and_process_desired:317 payload.state.desired found:{}
TRACE _coap_con_get_rpc_handler:408 No desired data since last check.
...
```

- http rpc例程

```shell
msh />tc_http_rpc_example
TC_http_rpc_example,TC_IOT_SDK_VERSION=2.9msh 
............
TRACE _get_timestamp_from_ntp:220 Second[1540455961]
............
[client->server]:{"method":"get","metadata":false,"reported":false}
3903d58467e1397c3068012a0cdff4b9|deviceName=collector_1_1&message={"method":"get","metadata":false,"reported":false}&nonce=509772187&productId=iot-eib00wp2&timestamp=1540455961
TRACE tc_iot_create_mqapi_rpc_json:34 MEM!b64_buf,total=46,used=44,left=2
TRACE tc_iot_http_mqapi_rpc:88 MEM!sign_out[TC_IOT_HTTP_TOKEN_REQUEST_FORM_LEN],total=1024,used=1,left=1023
TRACE tc_iot_http_mqapi_rpc:90 signed request form:
{"productId":"iot-eib00wp2","deviceName":"collector_1_1","nonce":509772187,"timestamp":1540455961,"messag
TRACE tc_iot_http_mqapi_rpc:103 http_buffer:
POST /rpc HTTP/1.1
User-Agent: tciotclient/1.0
Host: gz.auth-device-iot.tencentcloudapi.com
Accept: */*
Conte
TRACE tc_iot_network_prepare:64 dirtect tcp network intialized.
..........
TRACE tc_iot_http_is_complete_chunk:64 chunk size = 194
TRACE tc_iot_http_is_complete_chunk:60 last chunk found.
TRACE tc_iot_http_mqapi_rpc:106 MEM!http_buffer[TC_IOT_HTTP_TOKEN_RESPONSE_LEN],total=1024,used=194,left=830
[server->client]:{"method":"reply","payload":{"code":0,"status":"OK","state":{"desired":{}}},"timestamp":1540455961,"sequence"----- end --------rpc get: server desired data---------------
TRACE check_and_process_desired:141 MEM!json_token[TC_IOT_MAX_JSON_TOKEN_COUNT],total=2400,used=340,left=2060
TRACE check_and_process_desired:150 payload.state.desired found:{}
...
```
