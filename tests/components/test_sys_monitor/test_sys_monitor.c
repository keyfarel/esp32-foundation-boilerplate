#include "unity.h"
#include "esp_system.h"
#include "sys_monitor.h"

TEST_CASE("System Monitor Initializer returns ESP_OK", "[sys_monitor]")
{
    // Normally init is called once, but calling it again should return ESP_OK or handle it
    esp_err_t err = sys_monitor_init();
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err, "sys_monitor_init() failed!");
}

TEST_CASE("System Free Heap is greater than 100KB", "[memory]")
{
    uint32_t free_heap = esp_get_free_heap_size();
    TEST_ASSERT_GREATER_THAN_MESSAGE(100000, free_heap, "Free heap is dangerously low (< 100KB)!");
}
