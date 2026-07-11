#include "unity.h"
#include "wifi_manager.h"

TEST_CASE("WiFi Manager Initializer returns ESP_OK (Including NVS)", "[wifi_manager]")
{
    esp_err_t err = wifi_manager_init();
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err, "wifi_manager_init() failed! NVS might be corrupted.");
}
