# ESP-IDF Foundation Boilerplate: Unit Testing

![Testing](https://img.shields.io/badge/Testing-Unity-blue)
![Architecture](https://img.shields.io/badge/Architecture-Modular-brightgreen)
![Difficulty](https://img.shields.io/badge/Difficulty-Beginner_to_Pro-orange)

Welcome to the dedicated testing environment for the **ESP32 Foundation Boilerplate**. 

Unlike basic Arduino sketches, production firmware benefits from repeatable automated testing. This folder contains a fully isolated **Hardware-in-the-Loop (HIL) testing architecture** powered by the **Unity** test framework natively supported by ESP-IDF v5.x.

---

## 1. Project Structure

This testing environment strictly isolates test code from your production `components/` folder, ensuring a clean and professional codebase.

```text
tests/
├── components/                 # Component-specific test modules
│   ├── test_sys_monitor/       # Tests for memory & uptime diagnostics
│   └── test_wifi_manager/      # Tests for connection logic
├── main/
│   ├── CMakeLists.txt
│   └── app_test_main.c         # The Test Runner (orchestrator)
├── CMakeLists.txt              # Test environment CMake config
└── README.md
```

---

## 2. How to Run the Tests

To execute the test suite directly on your ESP32 hardware, run the following commands from the **root directory** of your project:

```bash
cd tests
idf.py build flash monitor
```

> [!NOTE]  
> If you encounter strange build errors after heavily restructuring your folders or modifying `CMakeLists.txt` files, you can run `idf.py fullclean` to wipe the build cache, then rebuild. Otherwise, stick to the standard build command to save compilation time.

Once completed, the ESP32 will automatically print the test report and enter a deep sleep state to halt execution cleanly.

> [!IMPORTANT]  
> **How to exit?** The terminal monitor will stay open indefinitely (this is normal for embedded systems). Press `Ctrl + ]` to safely close the serial monitor and return to your command line.

---

## 3. Writing Tests: Two Approaches

We embrace developers of all skill levels. Choose the method that best fits your workflow:

### Approach A: The Quick Way (For Beginners)

If you just want to test a simple function without the hassle of creating new folders and CMake files, you can write your `TEST_CASE()` macros directly inside `tests/main/app_test_main.c` right before the `app_main` function. Unity will automatically discover and run them.

```c
// Example inside app_test_main.c
TEST_CASE("Math works", "[dummy_tag]") {
    TEST_ASSERT_EQUAL(4, 2 + 2);
}
```

### Approach B: Modular Approach (Recommended for Growing Projects)

If your project is growing, keep your tests modular and scalable by creating a separate component for each test module (just like we did with `test_sys_monitor` and `test_wifi_manager`).

1. **Create the Folder**: Make a new folder in `tests/components/` (e.g., `test_mqtt_manager`).
2. **Add Files**: Add your `test_mqtt_manager.c` and a `CMakeLists.txt` inside it.
3. **Register It**: Add your new test component to the `REQUIRES` list inside `tests/main/CMakeLists.txt`.

> [!TIP]  
> **The WHOLE_ARCHIVE Magic:**  
> Normally, the C Linker aggressively deletes (garbage collects) isolated test files during compilation if their functions aren't explicitly called by `app_main`. 
> Instead of using hacky "dummy functions" to prevent this, we use the official ESP-IDF CMake `WHOLE_ARCHIVE` parameter inside your test component's `CMakeLists.txt`:
> ```cmake
> # Example tests/components/test_mqtt_manager/CMakeLists.txt
> idf_component_register(
>     SRCS "test_mqtt_manager.c"
>     REQUIRES unity mqtt_manager
>     WHOLE_ARCHIVE  # Forces the Linker to include all tests!
> )
> ```

---

## 4. Troubleshooting

Below are common issues encountered during testing and how to resolve them:

> [!WARNING]  
> **Issue: Tests are passing, but it says `0 Tests 0 Failures`**
> - **Cause**: The C Linker stripped your test file because it thought it was unused.
> - **Fix**: Ensure you added `WHOLE_ARCHIVE` in your test component's `CMakeLists.txt` as shown in Approach B.

> [!CAUTION]  
> **Issue: Wi-Fi Tests are failing with `ESP_ERR_WIFI_NOT_INIT` or no AP found**
> - **Cause**: The `tests/` environment is completely isolated from the main project. It does not inherit your root `sdkconfig`.
> - **Fix**: You must run `cd tests` followed by `idf.py menuconfig` to configure your Wi-Fi SSID and Password specifically for the test environment.

---

## Need More Features?

Want to automate this? In the upcoming **ESP-IDF Pro Boilerplate**, we include complete CI/CD GitHub Actions workflows. Push your code, and the cloud will automatically build and execute these tests on real hardware runners before merging!

*(Stay tuned for the Pro Version. Follow my Gumroad / Join the Waitlist [here](#) to get an exclusive early-bird discount when it drops!)*

---
*Happy Testing. Catch those bugs before they catch you.*