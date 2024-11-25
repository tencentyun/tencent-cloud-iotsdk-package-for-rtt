#include "tc_iot_inc.h"

int tc_iot_copy_net_context(tc_iot_net_context_t * net_context, tc_iot_net_context_init_t * init) {
    IF_NULL_RETURN(net_context, TC_IOT_NULL_POINTER);
    IF_NULL_RETURN(init, TC_IOT_NULL_POINTER);

    net_context->use_tls           = init->use_tls      ;
    net_context->host              = init->host         ;
    net_context->port              = init->port         ;
    net_context->fd                = init->fd           ;
    net_context->is_connected      = init->is_connected ;
    net_context->extra_context     = init->extra_context;

#ifdef ENABLE_TLS
    net_context->tls_config = init->tls_config;
#endif

#ifdef ENABLE_DTLS
    net_context->tls_config = init->tls_config;
#endif
    return 0;
}

int tc_iot_network_prepare(tc_iot_network_t * p_network, tc_iot_network_type type, tc_iot_network_protocol proto, bool over_tls, void * extra_options) {
    tc_iot_net_context_init_t netcontext;
#ifdef ENABLE_TLS
    tc_iot_tls_config_t* config;
#endif
    memset(&netcontext, 0, sizeof(netcontext));
    memset(p_network, 0, sizeof(* p_network));

    if (over_tls) {
#if defined(ENABLE_TLS)
        netcontext.fd = -1;
        netcontext.use_tls = 1;

        config = &(netcontext.tls_config);
        config->timeout_ms = TC_IOT_DEFAULT_TLS_HANSHAKE_TIMEOUT_MS;
        if (proto == TC_IOT_PROTO_HTTP) {
            config->verify_server = TC_IOT_HTTPS_CERT_STRICT_CHECK;
            /* config->root_ca_in_mem = g_tc_iot_https_root_ca_certs; */
        } else if (proto == TC_IOT_PROTO_MQTT) {
            config->verify_server = 1;
        } else {
            config->verify_server = 1;
        }

        if (TC_IOT_SOCK_STREAM == type) {
            tc_iot_hal_tls_init(p_network, &netcontext);
            TC_IOT_LOG_TRACE("tls network intialized.");
        } else {
            TC_IOT_LOG_FATAL("tls network not supported type=%d.", type);
            return TC_IOT_TLS_NOT_SUPPORTED;
        }
        /* init network end*/
#else
        TC_IOT_LOG_FATAL("tls network not supported.");
        return TC_IOT_TLS_NOT_SUPPORTED;
#endif
    } else {
        netcontext.fd = -1;
        netcontext.use_tls = 0;
        tc_iot_hal_net_init(p_network, &netcontext);
        TC_IOT_LOG_TRACE("dirtect tcp network intialized.");
    }

    return TC_IOT_SUCCESS;

}


