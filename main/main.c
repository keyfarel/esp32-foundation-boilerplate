#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include "sys_monitor.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting ESP-IDF Foundation Boilerplate...");

    ESP_ERROR_CHECK(sys_monitor_init());
    ESP_ERROR_CHECK(wifi_manager_init());

    // Wi-Fi connects asynchronously; application startup continues immediately.
    ESP_LOGI(TAG, "System ready. Starting user application...");

    while (1) {
        ESP_LOGI(TAG, "Application heartbeat...");

        if (wifi_is_connected()) {
            ESP_LOGI(TAG, "Wi-Fi is connected. Sending data to server...");
        } else {
            ESP_LOGW(TAG, "Wi-Fi is disconnected. Saving data locally to SD Card...");
        }

        sys_monitor_feed_watchdog();

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
