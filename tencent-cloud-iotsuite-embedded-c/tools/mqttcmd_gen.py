#coding:utf-8
#import paho.mqtt.client as mqtt
'''
ʹ��ǰ�ȸ��ݿ���̨��Ʒ��Ϣ���û�������, ��Ҫ�������, ��Ϊ ��Կ������
export TENCENT_CLOUD_DEMO_PRODUCT_ID="iot-i0ujhadi"
export TENCENT_CLOUD_DEMO_DEVICE_NAME="device_t1"
export TENCENT_CLOUD_DEMO_DEVICE_SECRET="xxxxxxxxxxxxxxxxx"
export TENCENT_CLOUD_DEMO_MQTT_HOST="mqtt-1ipy7vr68.ap-guangzhou.mqtt.tencentcloudmq.com"
export TENCENT_CLOUD_DEMO_MQTT_CID="mqtt-1ipy7vr68@device_t1"

�����ѡ, ��Ҫ�Ǳ����������Ի�����Ҫ
export  TENCENT_CLOUD_DEMO_MQTT_TOPIC= "pub_test"
export  TENCENT_CLOUD_DEMO_HTTP_HOST="stress-gz.auth-device-iot.tencentcloudapi.com"


'''
import time
import urllib
import urllib2
import contextlib
import hashlib
import hmac
import binascii
import json
from collections import OrderedDict
import os

'''
#�⼸�ж�û����
product_id = 'iot-r3cyxxxx' 
product_key = 'mqtt-xxxxxxxx'
device_name = 'dev0001'
device_secret = 'xxxxxxxxxxxxxxxxxxxxxxx'
topic = '%s/%s/tp0001' % (product_id, device_name)
client_id = product_key + '@dev0001'
'''

product_id = os.getenv("TENCENT_CLOUD_DEMO_PRODUCT_ID")
mqtt_host =   os.getenv("TENCENT_CLOUD_DEMO_MQTT_HOST")
device_name = os.getenv("TENCENT_CLOUD_DEMO_DEVICE_NAME")
device_secret = os.getenv("TENCENT_CLOUD_DEMO_DEVICE_SECRET")
client_id =  os.getenv("TENCENT_CLOUD_DEMO_MQTT_CID")

product_key = mqtt_host.split(".")[0]

if not client_id or len(client_id) < 2 :
    client_id = product_key + '@' + device_name

topic_name =  os.getenv("TENCENT_CLOUD_DEMO_MQTT_TOPIC")
if not topic_name or len(topic_name) < 2 :
    topic_name = "XXXXX_PLS_MODIFY_TOPIC"

token_host =   os.getenv("TENCENT_CLOUD_DEMO_HTTP_HOST")
if not token_host or len(token_host) < 2 :
    auth_url = 'http://gz.auth-device-iot.tencentcloudapi.com/token' 
    if mqtt_host.find("beijing")>=0 :
        auth_url = 'http://bj.auth-device-iot.tencentcloudapi.com/token' 
    if mqtt_host.find("shanghai")>=0 :
        auth_url = 'http://sh.auth-device-iot.tencentcloudapi.com/token' 
else :

    auth_url = 'http://%s/token' % token_host


def gen_cmd_http( _client_id) :

    params = OrderedDict([
        ['deviceName', device_name],
        ['message', 'message123'],
        ['nonce', 123456],
        ['productId', product_id],
        ['timestamp', int(time.time())],
        ['topic_name', topic_name]
        ])
    params['signature'] = binascii.b2a_base64(hmac.new(device_secret, '&'.join(k + '=' + str(params[k]) for k in params), hashlib.sha256).digest())[:-1]

    print "POST BODY:" , urllib.urlencode(params)



def gen_cmd( _client_id) :

    params = OrderedDict([
        ['clientId', _client_id],
        ['deviceName', device_name],
        ['expire', 2592000], #30 days
        ['nonce', 123456],
        ['productId', product_id],
        ['timestamp', int(time.time())]])
    params['signature'] = binascii.b2a_base64(hmac.new(device_secret, '&'.join(k + '=' + str(params[k]) for k in params), hashlib.sha256).digest())[:-1]


        
    print "======================================================================="
    print "URL:\t", auth_url
    print "POST BODY:" , urllib.urlencode(params)

    request = urllib2.Request( auth_url , urllib.urlencode(params))

    with contextlib.closing(urllib2.urlopen(request, timeout=3)) as response:
        code = response.getcode()
        rsp = response.read()
        print "RESPONSE:" , code, rsp
        data = json.loads(rsp)['data']
        code = json.loads(rsp)['returnCode']

    print "-------------------------------------------------\n"

    if 0 != code :
        print  'http token return invalid code , return'
        return 

    print 'examples/coap-client  -m post coap://122.152.224.121/auth -e "%s" \n' %  urllib.urlencode(params)
    print 'mosquitto_sub  -h %s -p 1883 -u %s -P "%s" -i %s  -V mqttv311 -t "%s" -d\n'%( mqtt_host, data['id'], data['secret'] , client_id, "%s/%s/#" %(product_id, device_name) )
    print 'mosquitto_pub  -h %s -p 1883 -u %s -P "%s" -i %s  -V mqttv311 -t "%s" -d -m "just4test" -q 1\n'%( mqtt_host, data['id'], data['secret'] , client_id, "%s/%s/%s" %(product_id, device_name, topic_name) )


def mqtt_auth_gen( _client_id) :
    nonce = 1234561
    now_ts = int(time.time())
    params = OrderedDict([
        ['clientId', _client_id],
        ['deviceName', device_name],
        ['nonce', nonce],
        ['productId', product_id],
        ['timestamp', now_ts]])
    context = '&'.join(k + '=' + str(params[k]) for k in params)
    params['signature'] = binascii.b2a_base64(hmac.new(device_secret, context, hashlib.sha256).digest())[:-1]
    print "content:\t", context ,"\ndevice_sec:\t",  device_secret, "\nSignature:\t", params['signature']
    #print params
    sig = urllib.urlencode( {'signature' : params['signature'] } )
    password = "productId=%s&nonce=%d&timestamp=%d&%s" % (product_id, nonce, now_ts, sig)

    #print password
    print 'mosquitto_sub  -h %s -p 1883 -u %s -P "%s" -i %s  -V mqttv311 -t "%s" -d\n'%( mqtt_host, device_name, password , _client_id, "%s/%s/#" %(product_id, device_name) )
    print 'mosquitto_pub  -h %s -p 1883 -u %s -P "%s" -i %s  -V mqttv311 -t "%s" -d -m "just4test" -q 1\n'%( mqtt_host, device_name, password, _client_id, "%s/%s/XXXXX_PLS_MODIFY" %(product_id, device_name) )


mqtt_auth_gen( client_id )

gen_cmd( client_id )

#gen_cmd( client_id + "_sub1")
#gen_cmd( client_id + "_pub1")