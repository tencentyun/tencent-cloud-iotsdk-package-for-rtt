#ifndef TC_IOT_HAL_OS_H
#define TC_IOT_HAL_OS_H

#include "tc_iot_inc.h"


/**
 * @brief tc_iot_hal_malloc ����������ڴ�ռ䣬������һ��ָ������ָ��
 *
 * @param size ������ռ�Ĵ�С
 *
 * @return ָ��ָ����䵽���ڴ�ռ䣬���ؿ�ָ�����ʾδ���䵽�ڴ�
 */
 void *tc_iot_hal_malloc(uint32_t size);


/**
 * @brief tc_iot_hal_free �ͷ�֮ǰ���� tc_iot_hal_malloc ��������ڴ�ռ䡣 
 *
 * @param ptr ���ͷŵ��ڴ�
 */
void tc_iot_hal_free(void *ptr);


/**
 * @brief tc_iot_hal_printf ���͸�ʽ���������׼��� stdout
 *
 * @param format ��ʽ���ַ���
 * @param ... ��ʽ������������
 *
 */
void tc_iot_hal_printf(const char *format, ...);

/**
 * @brief tc_iot_hal_snprintf ���͸�ʽ��������ַ���
 *
 * @param str ���������
 * @param size �����������С
 * @param format ��ʽ���ַ���
 * @param ... ��ʽ������������
 *
 * @return ����ֽ�������Ҫע����ǣ�������ֵ�����size��Сһ��ʱ���п��ܻ�����
 * ��С���������ݲ�δ��ȫ���
 */
/* int tc_iot_hal_snprintf(char *str, size_t size, const char *format, ...); */


/**
 * @brief tc_iot_hal_timestamp ϵͳʱ�������������ʱ�� 1970-1-1 00����������
 *
 * @param ������������δ����
 *
 * @return ʱ���
 */
long tc_iot_hal_timestamp(void *);


/**
 * @brief tc_iot_hal_sleep_ms ˯�߹���һ��ʱ������λ��ms
 *
 * @param sleep_ms ʱ������λms
 *
 * @return 0 ��ʾ�ɹ���-1 ��ʾʧ��
 */
int tc_iot_hal_sleep_ms(long sleep_ms);


/**
 * @brief tc_iot_hal_random ��ȡ�����
 *
 * @return ��õ������
 */
long tc_iot_hal_random(void);



/**
 * @brief tc_iot_hal_srandom �������������ֵ
 *
 * @param seed ���������ֵ
 */
void tc_iot_hal_srandom(unsigned int seed);


/**
 * @brief tc_iot_get_device_name ��ȡ�豸������, �����ƾ���Ψһ��,����ʹ��Ψһ�Ե�Ӳ��ID����IMEI / MAC ��
 *
 * @param device_name �������, ���ڴ洢 device_name, ����ΪNULL, ��ʱ����ʹ�þ�̬����ȫ�ֿռ�����ַ�������
 * @param  len        ���device_name��������С
 *
 * @return  NULL ��ʾʧ��,  �ɹ�����ָ�� device_name ��ָ��
 */
const char* tc_iot_hal_get_device_name(char *device_name, size_t len);

/**
 * @brief tc_iot_hal_set_value �־û�����һ�� key - value ֵ, ���籣�����ļ�ϵͳ���� flash ����
 *
 * @param  key		�������, �����key
 * @param  value    �������, �����value
 *
 * @return  <0  ��ʾʧ��,  �ɹ����� 0
 */
int   tc_iot_hal_set_value(const char* key ,  const char* value );

/**
 * @brief tc_iot_hal_get_value ��ȡ����� key - value ֵ
 *
 * @param  key		�������, �����key
 * @param  value    �������, value
 * @param  len      value��������С
 *
 *
 * @return  <0  ��ʾʧ��,  �ɹ����� 0
 */
int   tc_iot_hal_get_value(const char* key , char* value , size_t len );

#endif /* end of include guard */
