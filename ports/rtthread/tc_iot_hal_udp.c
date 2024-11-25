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

#if 0
//==============UDP API==================//
void *tc_iot_hal_udp_create(char *host, unsigned short port)
{
#define NETWORK_ADDR_LEN    (16)

    int                     rc = -1;
    long                    socket_id = -1;
    //int                     socket_id = -1;
    char                    port_ptr[6] = {0};
    struct addrinfo         hints;
    char                    addr[NETWORK_ADDR_LEN] = {0};
    struct addrinfo        *res, *ainfo;
    struct sockaddr_in     *sa = NULL;

    if (NULL == host) {
        return (void *)(-1);
    }

    sprintf(port_ptr, "%d", port);
    memset((char *)&hints, 0x00, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_UDP;

    rc = getaddrinfo(host, port_ptr, &hints, &res);
    if (0 != rc) {
        TC_IOT_LOG_ERROR("getaddrinfo error");
        return (void *)(-1);
    }

    for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
        if ((AF_INET == ainfo->ai_family) || (AF_AT == ainfo->ai_family)) {
            sa = (struct sockaddr_in *)ainfo->ai_addr;

            strncpy(addr, inet_ntoa(sa->sin_addr), NETWORK_ADDR_LEN);

            TC_IOT_LOG_TRACE("The host IP %s, port is %d\r\n", addr, ntohs(sa->sin_port));

            socket_id = socket(ainfo->ai_family, ainfo->ai_socktype, ainfo->ai_protocol);
            if (socket_id < 0) {
                TC_IOT_LOG_ERROR("create socket error");
                continue;
            }
            if (0 == connect(socket_id, ainfo->ai_addr, ainfo->ai_addrlen)) {
                break;
            }

            close(socket_id);
        }
		else
		{
			TC_IOT_LOG_ERROR("ai_family %d", ainfo->ai_family);
		}
    }
    freeaddrinfo(res);

    return (void *)socket_id;

#undef NETWORK_ADDR_LEN
}

void tc_iot_hal_udp_close(void *p_socket)
{
    long            socket_id = -1;

    socket_id = (long)p_socket;
    close(socket_id);
}

int tc_iot_hal_udp_write(void *p_socket,
                  const unsigned char *p_data,
                  unsigned int datalen)
{
    int             rc = -1;
    long            socket_id = -1;

    socket_id = (long)p_socket;
    rc = send(socket_id, (char *)p_data, (int)datalen, 0);
    if (-1 == rc) {
        return -1;
    }

    return rc;
}

int tc_iot_hal_udp_read(void *p_socket,
                 unsigned char *p_data,
                 unsigned int datalen)
{
    long            socket_id = -1;
    int             count = -1;

    if (NULL == p_data || NULL == p_socket) {
        return -1;
    }

    socket_id = (long)p_socket;
    count = (int)read(socket_id, p_data, datalen);

    return count;
}

int tc_iot_hal_udp_readTimeout(void *p_socket,
                        unsigned char *p_data,
                        unsigned int datalen,
                        unsigned int timeout)
{
    int                 ret;
    struct timeval      tv;
    fd_set              read_fds;
    long                socket_id = -1;

    if (NULL == p_socket || NULL == p_data) {
        return -1;
    }
    socket_id = (long)p_socket;

    if (socket_id < 0) {
        return -1;
    }

    FD_ZERO(&read_fds);
    FD_SET(socket_id, &read_fds);

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    ret = select(socket_id + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv);

    /* Zero fds ready means we timed out */
    if (ret == 0) {
        return -2;    /* receive timeout */
    }

    if (ret < 0) {
        if (errno == EINTR) {
            return -3;    /* want read */
        }

        return -4; /* receive failed */
    }

    /* This call will not block */
    return tc_iot_hal_udp_read(p_socket, p_data, datalen);
}

//===================================//
#else

//====================================================================

#if 0
int tc_iot_hal_udp_read(tc_iot_network_t* network, unsigned char* buffer, int len, int timeout_ms)
{
    int   count;

    if (NULL == buffer) 
	{
        return TC_IOT_INVALID_PARAMETER;
    }

    count = (int)read(network->net_context.fd, buffer, len);

	if (count == 0) 
	{
        return TC_IOT_NET_NOTHING_READ;
    }

    return count;
}
#else
int tc_iot_hal_udp_read(tc_iot_network_t* network, unsigned char* buffer, int len, int timeout_ms)
{
	int 				ret;
	struct timeval		tv;
	fd_set				read_fds;
	int				    socket_id = -1;
	int 				readlen;


	TC_IOT_LOG_TRACE("tc_iot_hal_udp_read entry, len=%d timeout=%dms", len, timeout_ms);	
	IF_NULL_RETURN(network, TC_IOT_NULL_POINTER);
	IF_NULL_RETURN(buffer, TC_IOT_NULL_POINTER);
	
	socket_id = network->net_context.fd;

	if (socket_id < 0) 
	{
		return TC_IOT_NETWORK_ERROR_BASE;
	}

	FD_ZERO(&read_fds);
	FD_SET(socket_id, &read_fds);

	tv.tv_sec  = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;

	ret = select(socket_id + 1, &read_fds, NULL, NULL, &tv);

	if (ret == 0) 
	{
		return TC_IOT_NET_READ_TIMEOUT;;	  /* receive timeout */
	}

	if (ret < 0) 
	{
		if (errno == EINTR) 
		{
			TC_IOT_LOG_TRACE("tc_iot_hal_udp_read EINTR is caught");
		}

		return TC_IOT_NET_READ_ERROR;
	}

	readlen = (int)read(network->net_context.fd, buffer, len);
	if (readlen == 0) 
	{
        return TC_IOT_NET_NOTHING_READ;
    }

    return readlen;

}




#endif

int tc_iot_hal_udp_write(tc_iot_network_t* network, const unsigned char* buffer, int len, int timeout_ms)
{
	TC_IOT_LOG_TRACE("tc_iot_hal_udp_write entry, len=%d timeout=%dms", len, timeout_ms);
	IF_NULL_RETURN(network, TC_IOT_NULL_POINTER);
	IF_NULL_RETURN(buffer, TC_IOT_NULL_POINTER);
	
    return send(network->net_context.fd, (char *)buffer, len, 0);
}


int tc_iot_hal_udp_connect(tc_iot_network_t* network, const char* host, uint16_t port) 
{  
	char        port_ptr[6] = {0};
	struct addrinfo* ainfo = NULL;
	struct addrinfo* res = NULL;
    struct addrinfo hints;
	struct sockaddr_in     *sa = NULL;
	int rc;


	TC_IOT_LOG_TRACE("tc_iot_hal_udp_connect entry");
	IF_NULL_RETURN(network, TC_IOT_NULL_POINTER);	
    if (host) {
        network->net_context.host = (char *)host;
    }

    if (port) {
        network->net_context.port = port;
    }

	sprintf(port_ptr, "%u",  network->net_context.port);
    memset((char *)&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_UDP;


	rc = getaddrinfo(network->net_context.host, port_ptr, &hints, &res);
    if (0 != rc) 
	{
        TC_IOT_LOG_ERROR("getaddrinfo failed for host:%s", network->net_context.host);
        return TC_IOT_NET_UNKNOWN_HOST;
    }

	for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) 
	{
		if ((AF_INET == ainfo->ai_family) || (AF_AT == ainfo->ai_family)) 
		{
		    sa = (struct sockaddr_in *)ainfo->ai_addr;

		    TC_IOT_LOG_TRACE("The host IP %s, port is %d\r\n", inet_ntoa(sa->sin_addr), ntohs(sa->sin_port));

		    network->net_context.fd  = socket(ainfo->ai_family, ainfo->ai_socktype, ainfo->ai_protocol);
		    if (network->net_context.fd  < 0) 
			{
		        TC_IOT_LOG_ERROR("create socket error");
		        continue;
		    }
		    if (0 == connect(network->net_context.fd , ainfo->ai_addr, ainfo->ai_addrlen)) 
			{
				network->net_context.is_connected = 1;
				rc = TC_IOT_SUCCESS;
		        break;
		    }

		    close(network->net_context.fd);
		}
		else
		{
			TC_IOT_LOG_ERROR("ai_family %d", ainfo->ai_family);
		}
	}
    freeaddrinfo(res);


	if(TC_IOT_SUCCESS != rc)
	{
		rc = TC_IOT_NET_CONNECT_FAILED;
	}

	return rc;    
}

int tc_iot_hal_udp_is_connected(tc_iot_network_t* network) {
    return network->net_context.is_connected;
}

int tc_iot_hal_udp_disconnect(tc_iot_network_t* network) {
    //TC_IOT_LOG_TRACE("network disconnecting...");
    close(network->net_context.fd);
    network->is_connected = 0;
    TC_IOT_LOG_TRACE("network disconnected");
    return TC_IOT_SUCCESS;
}

int tc_iot_hal_udp_destroy(tc_iot_network_t* network) {
    if (tc_iot_hal_udp_is_connected(network)) {
        tc_iot_hal_udp_disconnect(network);
    }

    //TC_IOT_LOG_TRACE("network destroying...");
    TC_IOT_LOG_TRACE("network destroied");
    return TC_IOT_SUCCESS;
}

int tc_iot_hal_udp_init(tc_iot_network_t* network,
                        tc_iot_net_context_init_t* net_context) {
    if (NULL == network) {
        return TC_IOT_NETWORK_PTR_NULL;
    }

    network->do_read = tc_iot_hal_udp_read;
    network->do_write = tc_iot_hal_udp_write;
    network->do_connect = tc_iot_hal_udp_connect;
    network->do_disconnect = tc_iot_hal_udp_disconnect;
    network->is_connected = tc_iot_hal_udp_is_connected;
    network->do_destroy = tc_iot_hal_udp_destroy;
    tc_iot_copy_net_context(&(network->net_context), net_context);

    return TC_IOT_SUCCESS;
}




#endif

#ifdef __cplusplus
}
#endif
