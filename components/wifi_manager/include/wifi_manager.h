#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

/**
 * @brief Initialize WiFi in station mode and connect using menuconfig credentials
 * @return esp_err_t ESP_OK on success
 */
esp_err_t wifi_manager_init(void);


/**
 * @brief Check if WiFi is currently connected without blocking.
 *        Useful for graceful degradation in the main application loop.
 * @return true if connected, false otherwise
 */
bool wifi_is_connected(void);

#endif // WIFI_MANAGER_H
