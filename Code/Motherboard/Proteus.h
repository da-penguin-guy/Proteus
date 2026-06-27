#ifndef _BOARDS_PROTEUS_H
#define _BOARDS_PROTEUS_H

// Flash and board information

#define PICO_FLASH_SIZE_BYTES        (16 * 1024 * 1024)

#ifndef PICO_BOARD
#define PICO_BOARD "Proteus"
#endif

// Networking SPI

#define PICO_DEFAULT_SPI             spi0
#define PICO_DEFAULT_SPI_RX_PIN      0   // NET_MISO
#define PICO_DEFAULT_SPI_TX_PIN      3   // NET_MOSI
#define PICO_DEFAULT_SPI_SCK_PIN     2   // NET_CLK

#define W5500_CS_PIN                 45 // Active-low CS pin for the W5500 Ethernet controller
#define W5500_INT_PIN                46 // Interrupt pin for the W5500 Ethernet controller
#define W5500_RST_PIN                47 // Reset pin for the W5500 Ethernet controller

#define RADIO_CS_PIN                 42 // Active-low CS pin for the RPI radio module 2
#define RADIO_INT_PIN                43 // Interrupt pin for the RPI radio module 2
#define RADIO_EN_PIN                 44 // Enable pin for the RPI radio module 2

// Module SPI

#define MODULE_SPI                   spi1
#define MODULE_MISO_PIN              8 
#define MODULE_MOSI_PIN              11
#define MODULE_CLK_PIN               10

// Module CS
#define CS_EN_PIN                    40 // Active-low enable pin for the CS demultiplexer. Pull high for no module selected.

// Power Management
#define POWER_CS_PIN                 1 // Active-low CS pin for the power management controller
#define POWER_INT_PIN                9 // Interrupt pin for the power management controller
#define POWER_RST_PIN                39 // Reset pin for the power management controller

// Swd Config
#define SWDIO_OUT_PIN                28 // SWDIO pin for programming modules.
#define SWCLK_OUT_PIN                29 // SWCLK pin for programming modules.

//I2C Configuration

#define PICO_DEFAULT_I2C             i2c1
#define PICO_DEFAULT_I2C_SDA_PIN     30 // I2C SDA pin. Only used internally, not passed to modules. Modules should use the MODULE SPI for communication, and not I2C.
#define PICO_DEFAULT_I2C_SCL_PIN     31 // I2C SCL pin. Only used internally, not passed to modules. Modules should use the MODULE SPI for communication, and not I2C.

// SD Card Configuration (4-bit SDIO Mode)
/*
Pins CLK_gpio, D1_gpio, D2_gpio, and D3_gpio are at offsets from pin D0_gpio.
The offsets are determined by sd_driver\SDIO\rp2040_sdio.pio.
    CLK_gpio = (D0_gpio + SDIO_CLK_PIN_D0_OFFSET) % 32;
    As of this writing, SDIO_CLK_PIN_D0_OFFSET is 30,
        which is -2 in mod32 arithmetic, so:
    CLK_gpio = D0_gpio -2.
    D1_gpio = D0_gpio + 1;
    D2_gpio = D0_gpio + 2;
    D3_gpio = D0_gpio + 3;
*/

#define PICO_SDIO_CLK_PIN            32 // SD card clock pin (SDIO CLK)
#define PICO_SDIO_CMD_PIN            33 // SD card command pin (SDIO CMD)
#define PICO_SDIO_D0_PIN             34 // SD card data pin 0 (SDIO D0)
#define PICO_SDIO_D1_PIN             35 // SD card data pin 1 (SDIO D1)
#define PICO_SDIO_D2_PIN             36 // SD card data pin 2 (SDIO D2)
#define PICO_SDIO_D3_PIN             37 // SD card data pin 3 (SDIO D3)
#define PICO_SDIO_DET_PIN            38 // SD card detect pin (active low when card is present)

// ==========================================
// 9. Auxiliary RAM
// ==========================================
#define RAM_CS_PIN                   41 // Active-low CS pin for the auxiliary RAM chip. This is a simple SPI RAM chip that can be used for extra storage or as a RAM disk. It is not intended for high-speed access, but can be useful for caching or buffering data from modules before writing to the SD card.


// ==========================================
// C++ Execution Layer (Hidden from C libraries)
// ==========================================
#ifdef __cplusplus
#include <cstdint>
#include <array>
#include <cstdio>
#include <cstdarg> 
#include <vector>
#include <string_view>
#include <iostream>
#include <string>
#include <vector>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "ff.h"       
#include "f_util.h"

/// @brief The namespace containing all helper functions and pin definition for arrays.
namespace Proteus
{
    constexpr std::array<std::uint8_t, 16> MODULE_GPIO = { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 };
    constexpr std::array<std::uint8_t, 4> CS_PINS = { 4, 5, 6, 7 };

    /// @brief Global FATFS instance for the SD card. This is used internally by the MountSDCard() helper, and can be used directly for advanced users who want more control over the filesystem. For most use cases, the simplified WriteFile() and ReadFile() helpers should be sufficient and you won't need to interact with this directly.
    inline FATFS StorageFS;
    inline FIL CurentFile; // Global FIL instance for the currently open file. This is used internally by the ReadFile() and WriteFile() helpers, but can be used directly for advanced users who want more control over file handling. 

    /// @brief Log severity levels for the logging system.
    enum LogLevel {
        LOG_ERROR,
        LOG_WARNING,
        LOG_INFO,
        LOG_DEBUG
    };

    /// @brief Helper to convert log level enum to a string prefix for log messages.
    /// @param level The severity level of the log message.
    /// @return The string prefix for the given log level. eg: "[ERROR] ", "[INFO]  ", etc.
    inline const std::string GetLevelString(LogLevel level) {
        switch (level) {
            case LOG_DEBUG:   return "[DEBUG]";
            case LOG_INFO:    return "[INFO]";
            case LOG_WARNING: return "[WARN]";
            case LOG_ERROR:   return "[ERROR]";
            default:          return "[LOG]";
        }
    }

    /// @brief Gets the current system timestamp string.
    /// @return The timestamp string, formatted as "(T+Xs.Yms)" where X is seconds and Y is milliseconds since boot.
    inline std::string GetTimestampString() {
        // Example using Pico's absolute uptime in milliseconds
        uint32_t uptime_ms = to_ms_since_boot(get_absolute_time());
        uint32_t seconds = uptime_ms / 1000;
        uint32_t ms = uptime_ms % 1000;
        return std::string("(T+") + std::to_string(seconds) + "s." + std::to_string(ms) + "ms)";
    }

    /// @brief Logs a message with a given severity level, using a printf-style format string and variable arguments.
    /// @param level The severity level of the log message.
    /// @param message The format string for the log message.
    /// @param args The variable arguments for the format string.
    /// @note Do not use this function directly. Use the Log(), LogError(), LogWarning(), LogInfo(), or LogDebug() helpers instead.
    inline void vLog(LogLevel level, const char* message, va_list args) {
        // 1. Clone the va_list because we need to read it twice (once to measure, once to format)
        va_list args_copy;
        va_copy(args_copy, args);

        // 2. Measure exactly how many characters are needed (passing NULL and 0)
        int length = vsnprintf(nullptr, 0, message, args_copy);
        va_end(args_copy);

        if (length < 0) {
            printf("Logging error: Formatting failed.\n");
            return;
        }

        // 3. Dynamically allocate exactly enough space (+1 for the null terminator)
        std::vector<char> buffer(length + 1);

        // 4. Format the string directly into our new dynamic layout
        vsnprintf(buffer.data(), buffer.size(), message, args);

        std::string formattedMessage(buffer.data());

        // 5. Prepend the log level and timestamp
        std::string finalMessage = GetLevelString(level) + " " + formattedMessage + " " + GetTimestampString() + "\n";

        std::cout << finalMessage;
    }

    /// @brief Logs a message with a given severity level, using a printf-style format string and variable arguments.
    /// @param level The severity level of the log message.
    /// @param message The format string for the log message.
    /// @param ... The variable arguments for the format string.
    /// @note Uses vsnprintf internally, so the same formatting rules apply as printf. For example, use %d for integers, %s for strings, etc.
    inline void Log(LogLevel level, const char* message, ...) {
        va_list args;
        va_start(args, message);
        
        vLog(level, message, args); // Forward the argument package
        
        va_end(args);
    }

    /// @brief Logs a message with the error severity level, using a printf-style format string and variable arguments.
    /// @param message The format string for the log message.
    /// @param ... The variable arguments for the format string.
    /// @note Uses vsnprintf internally, so the same formatting rules apply as printf. For example, use %d for integers, %s for strings, etc.
    inline void LogError(const char* message, ...) {
        va_list args;
        va_start(args, message);
        vLog(LOG_ERROR, message, args);
        va_end(args);
    }

    /// @brief Logs a message with the warning severity level, using a printf-style format string and variable arguments.
    /// @param message The format string for the log message.
    /// @param ... The variable arguments for the format string.
    /// @note Uses vsnprintf internally, so the same formatting rules apply as printf. For example, use %d for integers, %s for strings, etc.
    inline void LogWarning(const char* message, ...) {
        va_list args;
        va_start(args, message);
        vLog(LOG_WARNING, message, args);
        va_end(args);
    }

    /// @brief Logs a message with the info severity level, using a printf-style format string and variable arguments.
    /// @param message The format string for the log message.
    /// @param ... The variable arguments for the format string.
    /// @note Uses vsnprintf internally, so the same formatting rules apply as printf. For example, use %d for integers, %s for strings, etc.
    inline void LogInfo(const char* message, ...) {
        va_list args;
        va_start(args, message);
        vLog(LOG_INFO, message, args);
        va_end(args);
    }

    /// @brief Logs a message with the debug severity level, using a printf-style format string and variable arguments.
    /// @param message The format string for the log message.
    /// @param ... The variable arguments for the format string.
    /// @note Uses vsnprintf internally, so the same formatting rules apply as printf. For example, use %d for integers, %s for strings, etc.
    inline void LogDebug(const char* message, ...) {
        va_list args;
        va_start(args, message);
        vLog(LOG_DEBUG, message, args);
        va_end(args);
    }

    /// @brief Configures all the GPIO pins used by Proteus, including setting their function (SPI, I2C, GPIO) and direction (input/output) as needed.
    inline void SetupPins()
    {
        stdio_init_all();
        // Network SPI
        gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
        gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
        gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);

        // Network Peripherals (SIO means software-controlled GPIO)
        gpio_init(W5500_CS_PIN);  gpio_set_dir(W5500_CS_PIN, GPIO_OUT);
        gpio_init(W5500_INT_PIN); gpio_set_dir(W5500_INT_PIN, GPIO_IN);
        gpio_init(W5500_RST_PIN); gpio_set_dir(W5500_RST_PIN, GPIO_OUT);

        gpio_init(RADIO_CS_PIN);  gpio_set_dir(RADIO_CS_PIN, GPIO_OUT);
        gpio_init(RADIO_INT_PIN); gpio_set_dir(RADIO_INT_PIN, GPIO_IN);
        gpio_init(RADIO_EN_PIN);  gpio_set_dir(RADIO_EN_PIN, GPIO_OUT);

        // Module SPI
        gpio_set_function(MODULE_MISO_PIN, GPIO_FUNC_SPI);
        gpio_set_function(MODULE_MOSI_PIN, GPIO_FUNC_SPI);
        gpio_set_function(MODULE_CLK_PIN, GPIO_FUNC_SPI);

        // Power Management
        gpio_init(POWER_CS_PIN);  gpio_set_dir(POWER_CS_PIN, GPIO_OUT);
        gpio_init(POWER_INT_PIN); gpio_set_dir(POWER_INT_PIN, GPIO_IN);
        gpio_init(POWER_RST_PIN); gpio_set_dir(POWER_RST_PIN, GPIO_OUT);

        // CS Demux initialization
        for(std::uint8_t i = 0; i < CS_PINS.size(); i++)
        {
            gpio_init(CS_PINS[i]);
            gpio_set_dir(CS_PINS[i], GPIO_OUT);
        }
        gpio_init(CS_EN_PIN);
        gpio_set_dir(CS_EN_PIN, GPIO_OUT);
        gpio_put(CS_EN_PIN, 1); // Keep demux disabled by default

        // I2C
        gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
        gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
        gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    }

    /// @brief Sets the chip select line for a specific module.
    /// @param moduleIndex The index of the module for which to set the chip select line. Pass -1 to disable all modules (pull CS high).
    inline void SetModuleCS(std::int8_t moduleIndex)
    {
        if(moduleIndex < 0)
        {
            gpio_put(CS_EN_PIN, 1); // Disable demux
            return;
        }
        
        //Check if index is greater then the n bit int limit, where n is the number of cs pins
        if(moduleIndex >= (1 << CS_PINS.size()))
        {
            gpio_put(CS_EN_PIN, 1);
            LogWarning("Module index %d out of range for CS pins. Assuming no module selected.", moduleIndex);
            return;
        }
        
        for(std::uint8_t i = 0; i < CS_PINS.size(); i++)
        {
            gpio_put(CS_PINS[i], (moduleIndex >> i) & 1);
        }
        
        // Drop the enable line low to pass the signal to the target peripheral
        gpio_put(CS_EN_PIN, 0);
    }


    /// @brief Mounts the SD card and initializes the global FATFS instance.
    /// @return true if the SD card was mounted successfully, false otherwise.
    inline bool MountSDCard() {
        FRESULT fr = f_mount(&StorageFS, "0:", 1); // Directly mounts the global instance
        if (fr != FR_OK) {
            LogError("Failed to mount SD card: %s", FRESULT_str(fr));
            return false;
        }
        LogInfo("SD Card Mounted successfully onto Proteus.");
        return true;
    }

    /// @brief Unmounts the SD card and deinitializes the global FATFS instance.
    /// @return true if the SD card was unmounted successfully, false otherwise.
    inline bool UnmountSDCard() {
        FRESULT fr = f_unmount("0:");
        if (fr != FR_OK) {
            LogError("Failed to unmount SD card: %s", FRESULT_str(fr));
            return false;
        }
        LogInfo("SD Card unmounted successfully.");
        return true;
    }

    inline bool OpenFile(const char* filename, BYTE mode) {
        FRESULT fr = f_open(&CurentFile, filename, mode);
        if (fr != FR_OK) {
            LogError("Failed to open file '%s': %s", filename, FRESULT_str(fr));
            return false;
        }
        LogInfo("File '%s' opened successfully.", filename);
        return true;
    }

    inline bool CloseFile() {
        FRESULT fr = f_close(&CurentFile);
        if (fr != FR_OK) {
            LogError("Failed to close file: %s", FRESULT_str(fr));
            return false;
        }
        LogInfo("File closed successfully.");
        return true;
    }

    
    
}
#endif // __cplusplus

#endif