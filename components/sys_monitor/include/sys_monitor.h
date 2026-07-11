#ifndef SYS_MONITOR_H
#define SYS_MONITOR_H

#include "esp_err.h"

/**
 * @brief Initialize the System Monitor and configure the Hardware Watchdog.
 *        The calling task is automatically subscribed to the Watchdog.
 * @return esp_err_t ESP_OK on success
 */
esp_err_t sys_monitor_init(void);

/**
 * @brief Feed the Hardware Watchdog to prevent system panic.
 *        Must be called periodically in the main application loop.
 */
void sys_monitor_feed_watchdog(void);

#endif // SYS_MONITOR_H
