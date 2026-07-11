#include "wifi_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_timer.h"


#define MAXIMUM_RETRY CONFIG_ESP_MAXIMUM_RETRY
#define WIFI_BACKOFF_INITIAL_MS 2000
#define WIFI_BACKOFF_MAX_MS     60000

static const char* TAG = "wifi_manager";
static int s_retry_num = 0;
static uint32_t s_retry_delay_ms = WIFI_BACKOFF_INITIAL_MS;

static EventGroupHandle_t s_wifi_event_group;
static esp_timer_handle_t s_retry_timer = NULL;

static void retry_timer_callback(void* arg) {
    ESP_LOGI(TAG, "Backoff timer expired. Retrying WiFi connection...");
    esp_wifi_connect();
}


#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "WiFi Started. Connecting to AP...");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGW(TAG, "Disconnected! Retrying to connect to the AP (Attempt %d)...", s_retry_num);
        } else {
            ESP_LOGE(TAG, "Failed to connect to the AP after %d attempts. Backing off for %lu ms...", MAXIMUM_RETRY,
                s_retry_delay_ms);
            
            // Use timer-based retry to avoid blocking the event loop.
            esp_timer_start_once(s_retry_timer, s_retry_delay_ms * 1000ULL);

            // Cap backoff at 60 seconds.
            if (s_retry_delay_ms < WIFI_BACKOFF_MAX_MS) {
                s_retry_delay_ms *= 2;
            }

            s_retry_num = 0; // Reset counter to retry indefinitely
            xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "Success! Got IP Address: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        s_retry_delay_ms = WIFI_BACKOFF_INITIAL_MS; // Reset backoff delay on success
        xEventGroupClearBits(s_wifi_event_group, WIFI_FAIL_BIT);
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

esp_err_t wifi_manager_init(void) {
    ESP_LOGI(TAG, "Initializing WiFi Manager (ESP-IDF Foundation Boilerplate)...");

    s_wifi_event_group = xEventGroupCreate();

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK)
        return ret;

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    const esp_timer_create_args_t timer_args = {
        .callback = &retry_timer_callback,
        .name = "wifi_retry_timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &s_retry_timer));

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    return ESP_OK;
}



bool wifi_is_connected(void) {
    if (s_wifi_event_group == NULL) return false;
    EventBits_t bits = xEventGroupGetBits(s_wifi_event_group);
    return (bits & WIFI_CONNECTED_BIT) != 0;
}