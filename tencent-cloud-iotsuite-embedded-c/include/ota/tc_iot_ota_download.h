#ifndef TC_IOT_OTA_DOWNLOAD
#define TC_IOT_OTA_DOWNLOAD

int tc_iot_ota_download(const char* api_url, int partial_start, tc_iot_http_response_callback download_callback, const void * context);
int tc_iot_ota_request_content_length(const char* api_url);

#endif /* end of include guard */
