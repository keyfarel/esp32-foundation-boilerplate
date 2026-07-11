# ESP-IDF v5.x Pro Boilerplate

![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v5.0%2B-red)
![License](https://img.shields.io/badge/License-MIT-blue)
![Architecture](https://img.shields.io/badge/Architecture-Clean-brightgreen)

A production-ready base boilerplate for ESP-IDF projects. This template provides a robust foundation for building scalable, production-oriented IoT applications by isolating configuration, networking, and system diagnostics into decoupled components.

---

## Project Structure

This project enforces a strict separation of concerns. Please refer to [ARCHITECTURE.md](ARCHITECTURE.md) for the complete rulebook on how to extend this codebase.

```text
esp32-foundation-boilerplate/
├── components/
│   ├── sys_monitor/        # FreeRTOS Core 0 Memory & Uptime Diagnostics
│   └── wifi_manager/       # Event-driven Wi-Fi connection handler
├── main/
│   ├── CMakeLists.txt
│   ├── Kconfig.projbuild   # Wi-Fi SSID & Password configuration menu
│   └── main.c              # Pure orchestrator and Watchdog initialization
├── tests/                  # Hardware-in-the-Loop (HIL) Unit Testing
│   ├── components/         # Isolated test modules
│   └── main/               # Test orchestrator
├── .clang-format           # Standardized code formatting
├── .gitignore              # Git ignore rules (protects credentials)
├── ARCHITECTURE.md         # Component-based architectural rules
├── CMakeLists.txt
└── README.md
```

### Core Components

- **`main.c` (app_main)**
  Entry point of the application. Initializes core services and starts the main application loop.
- **`wifi_manager.c`**
  Handles Wi-Fi initialization, connection management, retry logic, and exponential backoff recovery.
- **`sys_monitor.c`**
  Provides watchdog management, heap monitoring, and runtime diagnostics.

---

## Features

### 1. Robust Wi-Fi Manager
- **Exponential Backoff:** If the router is unreachable, the system gracefully backs off (2s -> 4s -> 8s) instead of spamming connection requests, saving power and preventing freezes.
- **Event-Driven Connectivity:** Wi-Fi connection is managed asynchronously using FreeRTOS Event Groups without blocking application startup.
- **Menuconfig Integration:** Wi-Fi credentials are provided at compile-time via `idf.py menuconfig`. No hardcoded sensitive data in source files.

### 2. Autonomous System Diagnostics
- A decoupled FreeRTOS task runs continuously on Core 0 to monitor system health without interrupting application logic on Core 1.
- **Task Watchdog Timer (TWDT):** Hard-coded hardware watchdog to catch software hangs and automatically trigger panic-reboots to improve system resilience.
- Calculates system uptime and tracks the `Minimum Free Heap` watermark to guarantee stability and detect memory leaks over extended periods.

### 3. Architecture Highlights
- **Non-blocking Wi-Fi startup:** The application starts immediately while Wi-Fi connects in the background.
- **Graceful degradation:** Network-dependent features run only when connectivity is available.
- **Automatic recovery:** Wi-Fi failures are handled using exponential backoff retries.
- **Runtime monitoring:** Heap usage and watchdog status are monitored continuously.
- **Component-Based Design:** Everything is modular (`sys_monitor`, `wifi_manager`) using pure CMake.
- **Automated Unit Testing (Unity):** Includes an independent Unity test runner built according to ESP-IDF v5 standards for Hardware-in-the-Loop (HIL) testing.
- **Encapsulated Module State:** Internal state is hidden inside each component using file-scoped static variables.
- **Strict Error Checking:** Every initialization is wrapped in `ESP_ERROR_CHECK`.

---

## Getting Started

### Prerequisites
- [ESP-IDF v5.0 or higher](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html).
- A compatible ESP32 development board.

### Installation

1. Clone this repository to your local machine:
```bash
git clone https://github.com/YourUsername/esp32-foundation-boilerplate.git
cd esp32-foundation-boilerplate
```

### Configuration
Before compiling, you must provide your Wi-Fi credentials via the configuration menu:

```bash
idf.py set-target esp32
idf.py menuconfig
```
Navigate to `WiFi Configuration` -> Enter your SSID and Password -> Save and Exit.

### Build and Flash
To build the project and flash it to the target device, run:

```bash
idf.py -p COM_PORT flash monitor
```
*(Replace `COM_PORT` with your specific serial port. See the tip below to find your port).*

> **Tip: Finding your COM Port**
> - **Windows (PowerShell):** Run `[System.IO.Ports.SerialPort]::GetPortNames()`
> - **Linux/macOS:** Run `ls /dev/tty* | grep -i usb`

> **Tip: Exiting the Monitor**
> To exit the serial monitor and return to your terminal, press `Ctrl + ]`.

---

## Stability Validation

The diagnostic monitor helps detect memory leaks and abnormal heap degradation during long-running operation.

---

## Troubleshooting

Common issues faced by beginners and how to solve them:

1. **Permission Denied / Port Busy (COM_PORT)**
   - **Cause:** Another program (like Serial Monitor, Putty, or another VSCode instance) is using the port.
   - **Fix:** Close other terminal windows, or unplug and replug your ESP32.

2. **Failed to connect to AP (WIFI_FAIL_BIT)**
   - **Cause:** Incorrect SSID/Password or the router is too far.
   - **Fix:** Run `idf.py menuconfig`, navigate to `WiFi Configuration`, double-check your credentials, save, and re-flash.

3. **NVS No Free Pages Error**
   - **Cause:** The NVS partition scheme changed or became corrupted.
   - **Fix:** The boilerplate automatically erases and formats the NVS partition if this happens, so you don't need to do anything manually!

4. **`idf.py` is not recognized / CommandNotFoundException**
   - **Cause:** The ESP-IDF environment variables are not loaded in your current terminal session.
   - **Fix:** If using VS Code, press `F1`, type `ESP-IDF: Open ESP-IDF Terminal`, and use that new terminal. Alternatively, run the export script manually:
     - **Windows (PowerShell):** `. C:\Espressif\frameworks\esp-idf-v5.x\export.ps1` (Path may vary depending on your installation)
     - **Linux/macOS:** `. $HOME/esp/esp-idf/export.sh`

---

## Enterprise Ready? (Need More Features?)

This free Foundation Boilerplate provides a clean and reliable starting point for ESP-IDF applications. It handles the hardest parts of embedded development (memory management and connectivity) so you can focus on reading sensors and moving motors.

**However, if you are building a commercial product for mass production, you need more.**

Upgrade to the **[ESP-IDF Pro-Max Boilerplate (Coming Soon)](#)** to unlock:
- **Over-The-Air (OTA) Updates:** Update firmware remotely without physical cables.
- **SmartConfig / Captive Portal:** Let end-users configure Wi-Fi passwords from their smartphones (No hardcoded credentials!).
- **Secure MQTT & CI/CD Pipelines:** Ready for AWS/GCP IoT Core integrations.

*(Stay tuned! Follow my Gumroad / Join the Waitlist [here](#) to get an exclusive early-bird discount when it drops!)*

---
*License: MIT License*
