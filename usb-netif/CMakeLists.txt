set(src "usb_stack.c")

if(CONFIG_TINYUSB_NET_MODE_NCM)
    list(APPEND src "usb_netif_ncm.c")
endif()

if(CONFIG_USB_NETIF_HTTP_SERVER)
    list(APPEND src "http_server.c")
endif()

idf_component_register(SRCS ${src}
            INCLUDE_DIRS "include"
            PRIV_REQUIRES esp_netif
)
