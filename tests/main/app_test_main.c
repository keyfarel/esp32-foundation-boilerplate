#include "unity.h"
#include "esp_log.h"
#include "esp_sleep.h"

void app_main(void)
{
    ESP_LOGI("UNIT_TEST", "Starting automated unit tests...");
    
    UNITY_BEGIN();
    
    // ========================================================================
    // INLINE TESTS (QUICK WAY FOR BEGINNERS)
    // ========================================================================
    // Don't want to create separate component folders? 
    // You can write quick TEST_CASE() functions directly above this app_main() 
    // and Unity will automatically discover them when you run unity_run_all_tests().
    // ========================================================================
    
    unity_run_all_tests();
    UNITY_END();

    ESP_LOGI("UNIT_TEST", "Unit tests completed. Halting system...");
    esp_deep_sleep_start();
}
