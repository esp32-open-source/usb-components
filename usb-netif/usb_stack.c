/**
 * @file usb_stack.c
 * @author chegewara
 * @brief 
 * @version 0.1
 * @date 2024-10-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "lwip/esp_netif_net_stack.h"
#include "esp_netif_ip_addr.h"

#include "esp_mac.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"

#include "tinyusb.h"
#include "tinyusb_net.h"

#include "usb_netif.h"

#define TAG "usb-netif"
extern esp_netif_t *usb_netif_p;

/**
 * @brief Callback to pass data from usb stack to netif stack
 * 
 * @param buffer 
 * @param len 
 * @param ctx 
 * @return esp_err_t 
 */
static esp_err_t netif_recv_callback(void *buffer, uint16_t len, void *ctx)
{
    if (usb_netif_p)
    {
        void *buf_copy = malloc(len);
        if (!buf_copy)
        {
            return ESP_ERR_NO_MEM;
        }
        memcpy(buf_copy, buffer, len);
        return esp_netif_receive(usb_netif_p, buf_copy, len, NULL);
    }
    return ESP_OK;
}

esp_err_t init_tinyusb(const tinyusb_config_t *tusb_cfg)
{
    if (tusb_cfg)
    {
        ESP_RETURN_ON_ERROR(tinyusb_driver_install(tusb_cfg), TAG, "%s:%d", __FILE__, __LINE__);
    }
    else
    {
        const tinyusb_config_t _tusb_cfg = {
            .external_phy = false,
        };
        ESP_RETURN_ON_ERROR(tinyusb_driver_install(&_tusb_cfg), TAG, "%s:%d", __FILE__, __LINE__);
    }

    return ESP_OK;
}

esp_err_t usb_net_create(const tinyusb_net_config_t *net_config)
{
    if (net_config)
    {
        ESP_RETURN_ON_ERROR(tinyusb_net_init(TINYUSB_USBDEV_0, net_config), TAG, "%s:%d", __FILE__, __LINE__);
    }
    else
    {
        unsigned int mac_tmp[6] = {};
        sscanf(CONFIG_USB_NETIF_DEFAULT_MAC, "%02x:%02x:%02x:%02x:%02x:%02x",
                    &mac_tmp[0], &mac_tmp[1], &mac_tmp[2],
                    &mac_tmp[3], &mac_tmp[4], &mac_tmp[5]);
        tinyusb_net_config_t _net_config = {
            // locally administrated address for the ncm device as it's going to be used internally
            // for configuration only
            .on_recv_callback = netif_recv_callback,
        };
        for (size_t i = 0; i < 6; i++)
        {
            _net_config.mac_addr[i] = mac_tmp[i];
        }

        ESP_RETURN_ON_ERROR(tinyusb_net_init(TINYUSB_USBDEV_0, &_net_config), TAG, "%s:%d", __FILE__, __LINE__);
    }

    return ESP_OK;
}


esp_netif_t *usb_ip_init_default_config()
{
    ESP_ERROR_CHECK(init_tinyusb(NULL));
    ESP_ERROR_CHECK(usb_net_create(NULL));
    esp_netif_t* netif = netif_create(NULL, NULL, NULL);
    esp_netif_action_start(netif, 0, 0, 0);
    
    uint8_t s_sta_mac[6] = {};
    ESP_ERROR_CHECK(esp_read_mac(s_sta_mac, ESP_MAC_ETH));
    ESP_ERROR_CHECK(esp_netif_set_mac(netif, s_sta_mac));

    return netif;
}
