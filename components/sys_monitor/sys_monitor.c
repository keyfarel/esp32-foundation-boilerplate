#include <stdio.h>
#include "sys_monitor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"
static const char *TAG = "sys_monitor";
#define MEMORY_WARNING_THRESHOLD 100000
#define TWDT_TIMEOUT_MS          10000
#define MONITOR_INTERVAL_MS      10000

/**
 * @brief FreeRTOS task dedicated to monitoring memory leaks and stress testing.
 *
 * @param pvParameters Pointer to task parameters (unused)
 */
static void memory_monitor_task(void *pvParameters)
{
    while (1) {
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t min_free_heap = esp_get_minimum_free_heap_size();
        
        int64_t time_us = esp_timer_get_time();
        int32_t seconds = time_us / 1000000;
        int32_t hours = seconds / 3600;
        int32_t minutes = (seconds % 3600) / 60;
        int32_t secs = seconds % 60;

        ESP_LOGI(TAG, "[DIAGNOSTICS] UPTIME %02ld:%02ld:%02ld",
                 hours, minutes, secs);

        ESP_LOGI(TAG, "FREE_HEAP: %lu B", free_heap);
        ESP_LOGI(TAG, "MIN_HEAP : %lu B", min_free_heap);

        if (min_free_heap > MEMORY_WARNING_THRESHOLD) {
            ESP_LOGI(TAG, "STATUS: PASS");
        } else {
            ESP_LOGW(TAG, "STATUS: LOW MEMORY");
        }

        vTaskDelay(pdMS_TO_TICKS(MONITOR_INTERVAL_MS));
    }
}

esp_err_t sys_monitor_init(void)
{
    ESP_LOGI(TAG, "Initializing System Monitor & Watchdog Timer...");
    
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = TWDT_TIMEOUT_MS,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
        .trigger_panic = true
    };
    
    esp_err_t err = esp_task_wdt_init(&wdt_config);
    if (err == ESP_ERR_INVALID_STATE) {
        // TWDT was already initialized by the system (menuconfig default)
        // We reconfigure it to use our 10-second timeout instead of the default 5-second.
        esp_task_wdt_reconfigure(&wdt_config);
    }
    
    esp_task_wdt_add(NULL);
    
    BaseType_t ret = xTaskCreatePinnedToCore(
        memory_monitor_task, "MemoryMonitor", 2048, NULL, 1, NULL, 0
    );
    
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create Memory Monitor task");
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

void sys_monitor_feed_watchdog(void)
{
    esp_task_wdt_reset();
}
