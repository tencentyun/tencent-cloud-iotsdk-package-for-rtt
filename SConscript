import os
from building import *
import rtconfig

cwd  = GetCurrentDir()

src  = []
sample_mqtt_shadow_src  = []
sample_http_rpc_src  = []
sample_coap_advance_src  = []
sample_mqtt_basic_src  = []
sample_coap_basic_src  = []
CPPPATH = []
CPPDEFINES = []
LOCAL_CCFLAGS = ''

#Example
if GetDepend(['PKG_USING_TENCENT_IOTKIT_MQTT_ADVANCED']):
	sample_mqtt_shadow_src += Glob('samples/advanced_edition/mqtt/app_main.c')
	sample_mqtt_shadow_src += Glob('samples/advanced_edition/mqtt/tc_iot_device_logic.c')
	CPPPATH += [cwd + '/samples/advanced_edition/mqtt']
	
group = DefineGroup('tc_sample_shadow', sample_mqtt_shadow_src, depend = ['PKG_USING_TENCENT_IOTKIT_MQTT_ADVANCED'], CPPPATH = CPPPATH, LOCAL_CCFLAGS = LOCAL_CCFLAGS, CPPDEFINES = CPPDEFINES)
	
	
if GetDepend(['PKG_USING_TENCENT_IOTKIT_MQTT_BASIC']):
	sample_mqtt_basic_src += Glob('samples/basic_edition/mqtt/demo_mqtt.c')
	CPPPATH += [cwd + '/samples/basic_edition/mqtt']
group = DefineGroup('tc_sample_mqtt_basic', sample_mqtt_basic_src, depend = ['PKG_USING_TENCENT_IOTKIT_MQTT_BASIC'], CPPPATH = CPPPATH, LOCAL_CCFLAGS = LOCAL_CCFLAGS, CPPDEFINES = CPPDEFINES)	

if GetDepend(['PKG_USING_TENCENT_IOTKIT_COAP_ADVANCED']):
	sample_coap_advance_src += Glob('samples/advanced_edition/coap/coap_app_main.c')
	CPPPATH += [cwd + '/samples/advanced_edition/coap']	

group = DefineGroup('tc_sample_coap_advance', sample_coap_advance_src, depend = ['PKG_USING_TENCENT_IOTKIT_COAP_ADVANCED'], CPPPATH = CPPPATH, LOCAL_CCFLAGS = LOCAL_CCFLAGS, CPPDEFINES = CPPDEFINES)		

if GetDepend(['PKG_USING_TENCENT_IOTKIT_COAP_BASIC']):
	sample_coap_basic_src += Glob('samples/basic_edition/coap/coap_app_main.c')
	CPPPATH += [cwd + '/samples/basic_edition/coap']
group = DefineGroup('tc_sample_coap_basic', sample_coap_basic_src, depend = ['PKG_USING_TENCENT_IOTKIT_COAP_BASIC'], CPPPATH = CPPPATH, LOCAL_CCFLAGS = LOCAL_CCFLAGS, CPPDEFINES = CPPDEFINES)			

if GetDepend(['PKG_USING_TENCENT_IOTKIT_HTTP']):
	sample_http_rpc_src += Glob('samples/advanced_edition/http/http_rpc_main.c')
	src += Glob('tencent-cloud-iotsuite-embedded-c/src/http/*.c')
	CPPPATH += [cwd + '/samples/advanced_edition/http']   

group = DefineGroup('tc_sample_http_rpc', sample_http_rpc_src, depend = ['PKG_USING_TENCENT_IOTKIT_HTTP'], CPPPATH = CPPPATH, LOCAL_CCFLAGS = LOCAL_CCFLAGS, CPPDEFINES = CPPDEFINES)	
	
if GetDepend(['PKG_USING_TENCENT_IOTKIT_OTA']):
	src += Glob('samples/scenarios/ota/app_main.c')
	src += Glob('samples/scenarios/ota/tc_iot_ota_logic.c')
	CPPPATH += [cwd + '/samples/scenarios/ota'] 
	
if GetDepend(['PKG_USING_TENCENT_IOTKIT_SMARTBOX']):
	src += Glob('samples/scenarios/smartbox/app_main.c')
	CPPPATH += [cwd + '/samples/scenarios/smartbox'] 


#include headfile
CPPPATH += [cwd + '/tencent-cloud-iotsuite-embedded-c/include']
CPPPATH += [cwd + '/tencent-cloud-iotsuite-embedded-c/include/iotsuite']
CPPPATH += [cwd + '/tencent-cloud-iotsuite-embedded-c/include/mqtt']
CPPPATH += [cwd + '/tencent-cloud-iotsuite-embedded-c/include/http']	
CPPPATH += [cwd + '/tencent-cloud-iotsuite-embedded-c/include/common']
CPPPATH += [cwd + '/tencent-cloud-iotsuite-embedded-c/include/coap']
CPPPATH += [cwd + '/tencent-cloud-iotsuite-embedded-c/include/platform']
CPPPATH += [cwd + '/ports/rtthread/include']	
	
#tencent-cloud-iotsuite-embedded-c/src/mqtt tencent-cloud-iotsuite-embedded-c/external/paho
if GetDepend(['PKG_USING_TENCENT_IOTKIT_MQTT']):
	src += Glob('tencent-cloud-iotsuite-embedded-c/src/mqtt/*.c')
	src += Glob('tencent-cloud-iotsuite-embedded-c/src/iotsuite/*.c')
	src += Glob('tencent-cloud-iotsuite-embedded-c/src/http/*.c')
	src += Glob('tencent-cloud-iotsuite-embedded-c/external/paho.mqtt.embedded-c/MQTTPacket/src/*.c')
	SrcRemove(src, 'tencent-cloud-iotsuite-embedded-c/src/iotsuite/tc_iot_sub_device.c')
	CPPPATH += [cwd + '/tencent-cloud-iotsuite-embedded-c/external/paho.mqtt.embedded-c/MQTTPacket/src']	
	
#tencent-cloud-iotsuite-embedded-c/src/http
#src += Glob('tencent-cloud-iotsuite-embedded-c/src/http/*.c')

#tencent-cloud-iotsuite-embedded-c/src/coap
if GetDepend(['PKG_USING_TENCENT_IOTKIT_COAP']):
	src += Glob('tencent-cloud-iotsuite-embedded-c/src/coap/*.c')

#tencent-cloud-iotsuite-embedded-c/src/iotsuite
#src += Glob('tencent-cloud-iotsuite-embedded-c/src/iotsuite/*.c')
#SrcRemove(src, 'tencent-cloud-iotsuite-embedded-c/src/iotsuite/tc_iot_sub_device.c')

#tencent-cloud-iotsuite-embedded-c/src/common
src += Glob('tencent-cloud-iotsuite-embedded-c/src/common/*.c')	

#tencent-cloud-iotsuite-embedded-c/external/jsmn
src += Glob('tencent-cloud-iotsuite-embedded-c/external/jsmn/*.c')
CPPPATH += [cwd + '/tencent-cloud-iotsuite-embedded-c/external/jsmn']	


# ports/rtthread
src += Glob('ports/rtthread/*.c')

#TLS used  ports/ssl
if GetDepend(['PKG_USING_TENCENT_IOTKIT_MQTT_TLS']):
	src += Glob('ports/ssl/tc_iot_hal_tls.c')

#DTLS used ports/ssl
if GetDepend(['PKG_USING_TENCENT_IOTKIT_COAP_DTLS']):
	src += Glob('ports/ssl/tc_iot_hal_dtls.c')
  

#CPPDEFINES += ['ENABLE_TC_IOT_LOG_TRACE', 'ENABLE_TC_IOT_LOG_DEBUG', 'ENABLE_TC_IOT_LOG_INFO', 'ENABLE_TC_IOT_LOG_WARN', 'ENABLE_TC_IOT_LOG_ERROR', 'ENABLE_TC_IOT_LOG_FATAL']

group = DefineGroup('tencent-iotkit', src, depend = ['PKG_USING_TENCENT_IOTKIT'], CPPPATH = CPPPATH, LOCAL_CCFLAGS = LOCAL_CCFLAGS, CPPDEFINES = CPPDEFINES)
Return('group')
