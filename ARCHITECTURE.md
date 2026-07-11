# Architecture Guidelines & Rulebook

This project strictly adheres to a Component-Based Architecture design pattern for ESP-IDF. To maintain a clean, scalable, and bug-free codebase, any extensions or modifications to this boilerplate must follow the rules outlined below.

---

## 1. The Role of `main`

The `main` folder is exclusively reserved for **Orchestration** and high-level application logic.

### Rules:
- **Zero Hardware Logic:** Do not place hardware initialization details (e.g., I2C configs, SPI setups, or raw Wi-Fi logic) directly in `main.c`.
- **Delegation:** `main.c` should only call `_init()` functions from components and check their return values.
- **Watchdog Ownership:** `sys_monitor` owns the TWDT lifecycle. `main.c` must feed the watchdog via `sys_monitor_feed_watchdog()` in the primary application loop.

### Example of a compliant `main.c`:
```c
void app_main(void)
{
    ESP_ERROR_CHECK(sys_monitor_init());
    ESP_ERROR_CHECK(wifi_manager_init());

    while(1) {
        sensor_read_data();
        sys_monitor_feed_watchdog();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

---

## 2. The Role of `components`

All specific features (Networking, Sensors, Displays, Diagnostics) must be isolated into their own standalone directories inside `components/`.

### Rules:
- **Strict Isolation:** A component must never depend on the `main` application directory. If the `main` folder is deleted, the component must still compile independently.
- **Explicit Dependencies:** If Component A relies on Component B, it must explicitly declare the dependency in its `CMakeLists.txt` using the `REQUIRES` keyword.
- **Semantic Returns:** Every initialization function inside a component (e.g., `wifi_manager_init()`) MUST return `esp_err_t`. Do not use `void` for init functions. Return `ESP_OK` for success, and standard error codes (e.g., `ESP_FAIL`) for failures.
- **Hidden Complexities:** High-level abstractions should hide complex setup from `main.c`. For example, this boilerplate handles `nvs_flash_init()` entirely inside `wifi_manager.c`, keeping `main.c` incredibly clean.

### Standard `CMakeLists.txt` Template for Components
If you (or an AI assistant like Copilot) are generating a new component, use this standard template:

```cmake
idf_component_register(
    SRCS "your_component.c"
    INCLUDE_DIRS "."        # Use "." if your .h files are in the root folder, or "include" if using an include/ sub-directory.
    REQUIRES esp_timer  # Add dependencies here
    PRIV_REQUIRES nvs_flash
)
```

---

## 3. Error Handling

- **Catch Everything:** Never ignore return values from initialization functions. Use `ESP_ERROR_CHECK()` in `main.c` to gracefully catch and log any fatal setup failures.
- **Non-Fatal Errors:** If a non-critical error occurs inside a background FreeRTOS task, log it using `ESP_LOGW` or `ESP_LOGE` instead of triggering a system panic. Let the task recover if possible.

---

## 4. FreeRTOS Task Management

- **Core Allocation:** Always specify which core a task runs on using `xTaskCreatePinnedToCore()` instead of `xTaskCreate()`.
- **Core 0 (PRO_CPU):** Reserved for heavy networking, radio tasks, and system diagnostics (e.g., Memory Monitor).
- **Core 1 (APP_CPU):** Reserved for application UI, sensor processing, and main business logic.
- **Stack Size:** Always optimize stack sizes. Do not arbitrarily allocate 4096 bytes if a task only requires 2048 bytes.

---

## 5. Memory Management & Task Safety Rules

Due to the limited SRAM in ESP32, memory fragmentation is the primary cause of system crashes over long execution periods.

- **Dynamic Allocation Restrictions:** Strictly avoid using `malloc()` or `calloc()` inside infinite FreeRTOS loops. This will cause catastrophic heap fragmentation. Allocate memory statically (global/static variables) where possible, or ensure `free()` is called in the exact same scope.
- **String Safety:** It is strictly forbidden to use unsafe C string functions like `strcpy()` or `sprintf()` as they are highly susceptible to Buffer Overflows. You MUST use their safe bounds-checked alternatives: `strncpy()` and `snprintf()`.

By adhering to these principles, your IoT product will remain stable, maintainable, and highly professional.
