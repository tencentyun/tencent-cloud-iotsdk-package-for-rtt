if (ENABLE_MQTT)
    if(ENABLE_DATA_TEMPLATE)
        ADD_SUBDIRECTORY(mqtt)
        ADD_SUBDIRECTORY(http)
        if(ENABLE_SUB_DEVICE)
            ADD_SUBDIRECTORY(sub_device)
        endif()
    endif()
endif()

if(ENABLE_COAP)
    ADD_SUBDIRECTORY(coap)
endif()

