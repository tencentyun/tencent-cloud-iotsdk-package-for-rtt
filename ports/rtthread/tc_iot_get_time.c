#include <stdint.h>
#include "tc_iot_inc.h"


#define HTTP_RESP_CONTENT_LEN   (64)
#define NTP_SERVER_SJ      		"ntp.sjtu.edu.cn"
#define NTP_SERVER_TENCENT      "s2g.time.edu.cn"


#define NTP_PORT        		 (123)

#define LI                      0
#define VN                      3
#define MODE                    3
#define STRATUM                 0
#define POLL                    4
#define PREC                   -6

#define JAN_1970                0x83aa7e80 /* 2208988800 1970 - 1900 in seconds */

/* How to multiply by 4294.967296 quickly (and not quite exactly)
 * without using floating point or greater than 32-bit integers.
 * If you want to fix the last 12 microseconds of error, add in
 * (2911*(x))>>28)
 */
#define NTPFRAC(x)              (4294 * (x) + ((1981 * (x)) >> 11))

/* The reverse of the above, needed if we want to set our microsecond
 * clock (via settimeofday) based on the incoming time in NTP format.
 * Basically exact.
 */
#define USEC(x)                 (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))

/* Converts NTP delay and dispersion, apparently in seconds scaled
 * by 65536, to microseconds.  RFC1305 states this time is in seconds,
 * doesn't mention the scaling.
 * Should somehow be the same as 1000000 * x / 65536
 */
#define sec2u(x)                ((x) * 15.2587890625)

struct timeval_t {
    uint32_t tv_sec;
    uint32_t tv_usec;
};

struct ntptime_t {
    uint32_t coarse;
    uint32_t fine;
};

struct ntp_packet_t {
    int li;
    int vn;
    int mode;
    int stratum;
    int poll;
    int prec;
    int delay;
    int disp;
    int refid;
};

/**
 * implement of htonl and ntohl
 */
#define BigLittleSwap(A)        ((((uint32_t)(A) & 0xff000000) >> 24) | \
                                (((uint32_t)(A) & 0x00ff0000) >> 8) | \
                                (((uint32_t)(A) & 0x0000ff00) << 8) | \
                                (((uint32_t)(A) & 0x000000ff) << 24))

/* return 1 if big endian */
static int _check_endian(void)
{
    union {
        uint32_t i;
        uint8_t c[4];
    } u;

    u.i = 0x12345678;
    return (0x12 == u.c[0]);
}

static uint32_t _htonl(uint32_t h)
{
    return _check_endian() ? h : BigLittleSwap(h);
}

static uint32_t _ntohl(uint32_t n)
{
    return _check_endian() ? n : BigLittleSwap(n);
}

static int _get_packet(unsigned char *packet, int *len)
{
    uint32_t data[12];
    struct timeval_t now = {0, 0};

    if (*len < 48) {
        TC_IOT_LOG_ERROR("packet buf too short!\n");
        return -1;
    }

    memset(packet, 0, *len);

    data[0] = _htonl((LI << 30) | (VN << 27) | (MODE << 24) |
                     (STRATUM << 16) | (POLL << 8) | (PREC & 0xff));
    data[1] = _htonl(1 << 16);  /* Root Delay (seconds) */
    data[2] = _htonl(1 << 16);  /* Root Dispersion (seconds) */
    data[10] = _htonl(now.tv_sec + JAN_1970); /* Transmit Timestamp coarse */
    data[11] = _htonl(NTPFRAC(now.tv_usec));  /* Transmit Timestamp fine */

    memcpy(packet, data, 48);
    *len = 48;

    return 0;
}

static void parse_timeval(unsigned char *read_buf, struct timeval_t *tv)
{
/* straight out of RFC-1305 Appendix A */
    struct ntp_packet_t ntp_packet;
    struct ntptime_t xmttime;
#ifdef NTP_DEBUG
    struct ntptime_t reftime, orgtime, rectime;
#endif
    memset(&ntp_packet, 0, sizeof(struct ntp_packet_t));

#define Data(i) _ntohl(((unsigned int *)read_buf)[i])
    ntp_packet.li      = Data(0) >> 30 & 0x03;
    ntp_packet.vn      = Data(0) >> 27 & 0x07;
    ntp_packet.mode    = Data(0) >> 24 & 0x07;
    ntp_packet.stratum = Data(0) >> 16 & 0xff;
    ntp_packet.poll    = Data(0) >>  8 & 0xff;
    ntp_packet.prec    = Data(0)       & 0xff;
    if (ntp_packet.prec & 0x80) ntp_packet.prec |= 0xffffff00;
    ntp_packet.delay   = Data(1);
    ntp_packet.disp    = Data(2);
    ntp_packet.refid   = Data(3);

#ifdef NTP_DEBUG
    reftime.coarse = Data(4);
    reftime.fine   = Data(5);
    orgtime.coarse = Data(6);
    orgtime.fine   = Data(7);
    rectime.coarse = Data(8);
    rectime.fine   = Data(9);
#endif
    xmttime.coarse = Data(10);
    xmttime.fine   = Data(11);
#undef Data

#ifdef NTP_DEBUG
    TC_IOT_LOG_TRACE("LI=%d  VN=%d  Mode=%d  Stratum=%d  Poll=%d  Precision=%d\n",
              ntp_packet.li, ntp_packet.vn, ntp_packet.mode,
              ntp_packet.stratum, ntp_packet.poll, ntp_packet.prec);
    TC_IOT_LOG_TRACE("Delay=%.1f  Dispersion=%.1f  Refid=%u.%u.%u.%u\n",
              sec2u(ntp_packet.delay), sec2u(ntp_packet.disp),
              ntp_packet.refid >> 24 & 0xff, ntp_packet.refid >> 16 & 0xff,
              ntp_packet.refid >> 8 & 0xff, ntp_packet.refid & 0xff);
    TC_IOT_LOG_TRACE("Reference %u.%.6u\n", reftime.coarse - JAN_1970, USEC(reftime.fine));
    TC_IOT_LOG_TRACE("Originate %u.%.6u\n", orgtime.coarse - JAN_1970, USEC(orgtime.fine));
    TC_IOT_LOG_TRACE("Receive   %u.%.6u\n", rectime.coarse - JAN_1970, USEC(rectime.fine));
    TC_IOT_LOG_TRACE("Transmit  %u.%.6u\n", xmttime.coarse - JAN_1970, USEC(xmttime.fine));
#endif

    tv->tv_sec = xmttime.coarse - JAN_1970;
    tv->tv_usec = USEC(xmttime.fine);
}

static uint32_t _get_timestamp_from_ntp(char *host)
{
    //long fd;  /* socket */
    int fd;  /* socket */
    int ret = -1;
    struct timeval_t tv;
    unsigned char write_buf[48] = {0};
    int write_len = sizeof(write_buf);
    unsigned char read_buf[1500] = {0};

	//===============
	tc_iot_network_t udp_network;
	tc_iot_net_context_init_t netcontext;

	memset(&udp_network, 0, sizeof(udp_network));
	netcontext.fd = -1;
    netcontext.use_tls = 0;
    netcontext.host = host;
    netcontext.port = NTP_PORT;

	tc_iot_hal_udp_init(&udp_network, &netcontext);
	ret = udp_network.do_connect(&udp_network, host, NTP_PORT);
	
    if (ret < 0) {
        TC_IOT_LOG_ERROR("udp connect error!");
        return 0;
    }
	else
	{
		TC_IOT_LOG_DEBUG("udp fd (%d)", udp_network.net_context.fd);
	}

    ret = _get_packet(write_buf, &write_len);
    if (ret < 0) {
        TC_IOT_LOG_ERROR("get_packet error!");
        goto do_exit;
    }


    ret = udp_network.do_write(&udp_network, write_buf, write_len, 0);
    if (ret < 0) {
        TC_IOT_LOG_ERROR("udp write error!");
        goto do_exit;
    }


	ret = udp_network.do_read(&udp_network, read_buf, sizeof(read_buf), 10000);
    if (ret < 0) {
        TC_IOT_LOG_ERROR("udp read error!");
        goto do_exit;
    }

    parse_timeval(read_buf, &tv);
	TC_IOT_LOG_TRACE("Second[%d]", tv.tv_sec);

do_exit:

	udp_network.do_destroy(&udp_network);
    return tv.tv_sec;
}

uint32_t tc_iot_get_time_from_ntp(void)
{
    char ntp_server[30] = {0};
    uint32_t time_in_second = 0;

	tc_iot_hal_snprintf(ntp_server, 30, NTP_SERVER_SJ);
    time_in_second = _get_timestamp_from_ntp(ntp_server);
    if (time_in_second > 0) 
	{
		goto end;
    }

	tc_iot_hal_snprintf(ntp_server, 30, NTP_SERVER_TENCENT);
    time_in_second = _get_timestamp_from_ntp(ntp_server);
    if (time_in_second > 0) 
	{	
		goto end;
    }
	else
	{
		TC_IOT_LOG_ERROR("get timestamp fail");
	}

end:	
    return time_in_second;
}
