#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <string.h>
//#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include <rtthread.h>


#include "tc_iot_inc.h"


int tc_iot_hal_net_read(tc_iot_network_t* network, unsigned char* buffer, int len, int timeout_ms) 
{
	int ret, err_code = 0;
	uint32_t len_recv = 0;
	time_int_type t_left;
	fd_set sets;
	struct timeval timeout;
	tc_iot_timer timer;


	IF_NULL_RETURN(network, TC_IOT_NULL_POINTER);
    TC_IOT_LOG_TRACE("tc_iot_hal_net_read entry, len=%d timeout=%dms", len, timeout_ms);	
    tc_iot_hal_timer_countdown_ms(&timer, timeout_ms);

	do{
		
		FD_ZERO(&sets);
		FD_SET(network->net_context.fd, &sets);
		t_left = tc_iot_hal_timer_left_ms(&timer);
		timeout.tv_sec = t_left / 1000;
		timeout.tv_usec = (t_left % 1000) * 1000;
		ret = select(network->net_context.fd + 1, &sets, NULL, NULL, &timeout);
		if (ret > 0) 
		{
			ret = recv(network->net_context.fd, buffer + len_recv, len - len_recv, 0);
			if (ret > 0) 
			{
				len_recv += ret;
			} 
			else if (0 == ret) 
			{
				TC_IOT_LOG_ERROR("tc_iot_hal_net_read connection is closed");
				err_code = TC_IOT_NET_READ_ERROR;
				break;
			} 
			else 
			{
				if (EINTR == errno) 
				{
					TC_IOT_LOG_TRACE("tc_iot_hal_net_read EINTR is caught");
					continue;
				}
				TC_IOT_LOG_ERROR("tc_iot_hal_net_read recv fail");
				err_code = TC_IOT_NET_READ_ERROR;
				break;
			}
		}
		else if (0 == ret) 
		{
			TC_IOT_LOG_WARN("tc_iot_hal_net_read nothing read");
			err_code = TC_IOT_NET_NOTHING_READ;
            break;
        }				
		else 
		{
			TC_IOT_LOG_ERROR("tc_iot_hal_net_read select fail");
			err_code = TC_IOT_NET_READ_ERROR;
			break;
		}
	} while ((len_recv < len) && (!tc_iot_hal_timer_is_expired(&timer)));


	return (0 != len_recv) ? len_recv : err_code;
}


int tc_iot_hal_net_write(tc_iot_network_t* network, const unsigned char* buffer, int len, int timeout_ms) 

{
	int ret;
	uint32_t len_sent = 0;
	time_int_type t_left;
	struct timeval timeout;
	tc_iot_timer timer;
	fd_set sets;


	IF_NULL_RETURN(network, TC_IOT_NULL_POINTER);
    TC_IOT_LOG_TRACE("tc_iot_hal_net_write entry, len=%d timeout=%dms", len, timeout_ms);
	
    tc_iot_hal_timer_countdown_ms(&timer, timeout_ms);

	do {	
			FD_ZERO(&sets);
			FD_SET(network->net_context.fd, &sets);

			t_left = tc_iot_hal_timer_left_ms(&timer);
			timeout.tv_sec = t_left / 1000;
			timeout.tv_usec = (t_left % 1000) * 1000;

			ret = select(network->net_context.fd + 1, NULL, &sets, NULL, &timeout);
			if (ret > 0) 
			{
				if (0 == FD_ISSET(network->net_context.fd, &sets)) 
				{
					TC_IOT_LOG_TRACE("tc_iot_hal_net_write fd no set");
					/* If timeout in next loop, it will not sent any data */
					ret = 0;
					continue;
				}
			} 
			else if (0 == ret) 
			{
				TC_IOT_LOG_TRACE("tc_iot_hal_net_write select timeout fd=%d", (int)network->net_context.fd);
				break;
			} 
			else 
			{
				if (EINTR == errno) 
				{
					TC_IOT_LOG_TRACE("tc_iot_hal_net_write EINTR is caught");
					continue;
				}

				TC_IOT_LOG_ERROR("tc_iot_hal_net_write select fail");
				break;
			}
	

			if (ret > 0) 
			{
				ret = send(network->net_context.fd, buffer + len_sent, len - len_sent, 0);
				if (ret > 0) 
				{
					len_sent += ret;
				} 
				else if (0 == ret) 
				{
					TC_IOT_LOG_TRACE("tc_iot_hal_net_write no data send");
				} 
				else 
				{
					if (EINTR == errno) 
					{
						TC_IOT_LOG_TRACE("tc_iot_hal_net_write EINTR is caught");
						continue;
					}

					TC_IOT_LOG_ERROR("tc_iot_hal_net_write send fail");
					len_sent = TC_IOT_SEND_PACK_FAILED;
					break;
				}
			}
	} while ((len_sent < len)  && (!tc_iot_hal_timer_is_expired(&timer)));

	return len_sent;
}

int tc_iot_hal_net_connect(tc_iot_network_t* n, char* host, uint16_t port) 
{
    struct addrinfo hints;
    struct addrinfo *addrInfoList = NULL;
    struct addrinfo *cur = NULL;
    int rc = -1;
    char buff[6];

  

    TC_IOT_LOG_TRACE("tc_iot_hal_net_connect entry");
	memset(&hints, 0, sizeof(hints));
    if (host) {
        n->net_context.host = host;
    }

    if (port) {
        n->net_context.port = port;
    }

    hints.ai_family = AF_INET; /* only IPv4 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

	memset(buff, 0, sizeof(buff));
    sprintf(buff, "%u", n->net_context.port);

	TC_IOT_LOG_TRACE("host:(%s) port:(%s)", n->net_context.host, buff);

    if ((rc = getaddrinfo(n->net_context.host, buff, &hints, &addrInfoList)) != 0) 
	{
        TC_IOT_LOG_ERROR("getaddrinfo failed for host:%s", n->net_context.host);
		rc = TC_IOT_NET_UNKNOWN_HOST;
        goto exit;
    }

    for (cur = addrInfoList; cur != NULL; cur = cur->ai_next) 
	{
        if (cur->ai_family != AF_INET) 
		{
            TC_IOT_LOG_ERROR("socket type error");
            rc = 0;
            continue;
        }

        n->net_context.fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (n->net_context.fd < 0) 
		{
            TC_IOT_LOG_ERROR("create socket failed for host:%s", n->net_context.host);
            rc = TC_IOT_NET_SOCKET_FAILED;
            continue;
        }

        if (connect(n->net_context.fd, cur->ai_addr, cur->ai_addrlen) == 0) 
		{
            rc = TC_IOT_SUCCESS;
            break;
        }

        close(n->net_context.fd);
        TC_IOT_LOG_ERROR("connect failed for host:%s", n->net_context.host);
		rc = TC_IOT_NET_CONNECT_FAILED;
       
    }

    if (TC_IOT_SUCCESS == rc) 
    {
        TC_IOT_LOG_TRACE("connect host(%s) success, fd=%d", host, n->net_context.fd);
    }
    freeaddrinfo(addrInfoList);
	
exit:

    return rc;
}


int tc_iot_hal_net_is_connected(tc_iot_network_t* network) {
    return network->net_context.is_connected;
}

int tc_iot_hal_net_disconnect(tc_iot_network_t* network) {
	if (network->net_context.fd >= 0)
    {
        closesocket(network->net_context.fd);
        network->net_context.fd = -1;
    }
    network->is_connected = 0;
    return TC_IOT_SUCCESS;
}

int tc_iot_hal_net_destroy(tc_iot_network_t* network) {
    return TC_IOT_SUCCESS;
}



int tc_iot_hal_net_init(tc_iot_network_t* network,
                        tc_iot_net_context_init_t* net_context) {
    if (NULL == network) {
        return TC_IOT_NETWORK_PTR_NULL;
    }

    network->do_read = tc_iot_hal_net_read;
    network->do_write = tc_iot_hal_net_write;
    network->do_connect = tc_iot_hal_net_connect;
    network->do_disconnect = tc_iot_hal_net_disconnect;
    network->is_connected = tc_iot_hal_net_is_connected;
    network->do_destroy = tc_iot_hal_net_destroy;
    tc_iot_copy_net_context(&(network->net_context), net_context);

    return TC_IOT_SUCCESS;
}

#ifdef __cplusplus
}
#endif
