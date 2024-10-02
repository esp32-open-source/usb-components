#pragma once
/**
 * @file usb_netif.h
 * @author chegewara
 * @brief 
 * @version 0.1
 * @date 2024-10-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "tinyusb.h"
#include "tinyusb_net.h"
#include "esp_netif_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Create ethernet and netif and register on the stack
 * 
 * @param base 
 * @param driver 
 * @param stack 
 * @return esp_netif_t* 
 */
esp_netif_t *netif_create(const esp_netif_inherent_config_t *base, const esp_netif_driver_ifconfig_t *driver, const esp_netif_netstack_config_t *stack);

/**
 * @brief Init USB stack 
 * 
 * @param tusb_cfg 
 * @return esp_err_t 
 */
esp_err_t init_tinyusb(const tinyusb_config_t *tusb_cfg);
/**
 * @brief Init CDC-NCM interface on usb stack
 * 
 * @param net_config ESP TinyUSB NCM driver configuration structure - if NULL use default config
 * @return esp_err_t 
 */
esp_err_t usb_net_create(const tinyusb_net_config_t *net_config);

/**
 * @brief Wrapper to init usb netif stack with default configurations
 * 
 * @return esp_netif_t* esp netif handle
 */
esp_netif_t *usb_ip_init_default_config();

#ifdef __cplusplus
}
#endif

