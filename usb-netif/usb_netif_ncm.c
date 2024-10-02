/**
 * @file usb_netif.c
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
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"

#include "usb_netif.h"

#define TAG "usb-netif"

/// @brief USB netif handle
esp_netif_t *usb_netif_p = NULL;


/**
 * @brief Callback to pass data from esp netif stack to usb stack
 * 
 * @param h netif handle
 * @param buffer 
 * @param len 
 * @return esp_err_t 
 */
static esp_err_t netif_transmit(void *h, void *buffer, size_t len)
{
    if (tinyusb_net_send_sync(buffer, len, NULL, pdMS_TO_TICKS(100)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send buffer to USB!");
    }
    return ESP_OK;
}
/**
 * @brief Clear buffer after tx 
 * 
 * @param h netif handle
 * @param buffer 
 */
static void l2_free(void *h, void *buffer)
{
    free(buffer);
}

esp_netif_t *netif_create(const esp_netif_inherent_config_t *base, const esp_netif_driver_ifconfig_t *driver, const esp_netif_netstack_config_t *stack)
{
    esp_netif_ip_info_t _netif_usb_ip = {};

    _netif_usb_ip.ip.addr = esp_ip4addr_aton(CONFIG_USB_NETIF_DEFAULT_IP);
    _netif_usb_ip.gw.addr = esp_ip4addr_aton(CONFIG_USB_NETIF_DEFAULT_GATEWAY);
    _netif_usb_ip.netmask.addr = esp_ip4addr_aton(CONFIG_USB_NETIF_DEFAULT_NETMASK);

    // Definition of
    // 1) Derive the base config (very similar to IDF's default WiFi AP with DHCP server)
    esp_netif_inherent_config_t base_cfg = {
        .flags = ESP_NETIF_DHCP_SERVER | ESP_NETIF_FLAG_AUTOUP, // Run DHCP server; set the netif "ip" immediately
        .ip_info = &_netif_usb_ip,                              // Use the same IP ranges as IDF's soft AP
        .if_key = CONFIG_USB_NETIF_IF_KEY,                                  // Set mame, key, priority
        .if_desc = "usb ncm config device",
        .route_prio = 30};
    // 2) Use static config for driver's config pointing only to static transmit and free functions
    esp_netif_driver_ifconfig_t driver_cfg = {
        .handle = (void *)1,             // not using an instance, USB-NCM is a static singleton (must be != NULL)
        .transmit = netif_transmit,      // point to static Tx function
        .driver_free_rx_buffer = l2_free // point to Free Rx buffer function
    };
    // 3) USB-NCM is an Ethernet netif from lwip perspective, we already have IO definitions for that:
    struct esp_netif_netstack_config lwip_netif_config = {
        .lwip = {
            .init_fn = ethernetif_init,
            .input_fn = ethernetif_input}};

    esp_netif_config_t cfg = {
        .base = &base_cfg,
        .driver = &driver_cfg,
        .stack = &lwip_netif_config
    };

    if (base)
    {
        cfg.base = base;
    }
    if (driver)
    {
        cfg.driver = driver;
    }
    if (stack)
    {
        cfg.stack = stack;
    }

    usb_netif_p = esp_netif_new(&cfg);

    return usb_netif_p;
}
