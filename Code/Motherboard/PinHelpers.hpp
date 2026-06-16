#pragma once
#include <cstdint>

#include <array>
#include <cmath>

// Basic RP2350 GPIO pin aliases.
// Adjust these values to match the specific board you are wiring.
namespace PinHelpers {

    constexpr std::uint8_t NET_MISO = 0;
    constexpr std::uint8_t NET_MOSI = 3;
    constexpr std::uint8_t NET_CLK = 2;

    constexpr std::uint8_t W5500_CS = 45;
    constexpr std::uint8_t W5500_INT = 46;
    constexpr std::uint8_t W5500_RST = 47;

    constexpr std::uint8_t RADIO_CS = 42;
    constexpr std::uint8_t RADIO_INT = 43;
    constexpr std::uint8_t RADIO_EN = 44;

    constexpr std::uint8_t MODULE_MISO = 8;
    constexpr std::uint8_t MODULE_MOSI = 11;
    constexpr std::uint8_t MODULE_CLK = 10;

    constexpr std::array<std::uint8_t, 16> MODULE_GPIO = { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 };

    constexpr std::array<std::uint8_t, 4> CS_PINS = { 4, 5, 6, 7 };
    constexpr std::uint8_t CS_EN = 40;

    constexpr std::uint8_t POWER_CS = 1;
    constexpr std::uint8_t POWER_INT = 9;
    constexpr std::uint8_t POWER_RST = 39;

    constexpr std::uint8_t SWDIO_OUT = 28;
    constexpr std::uint8_t SWCLK_OUT = 29;

    constexpr std::uint8_t SDA = 30;
    constexpr std::uint8_t SCL = 31;

    constexpr std::uint8_t SD_CLK = 32;
    constexpr std::uint8_t SD_CMD = 33;
    constexpr std::uint8_t SD_D0 = 34;
    constexpr std::uint8_t SD_D1 = 35;
    constexpr std::uint8_t SD_D2 = 36;
    constexpr std::uint8_t SD_D3 = 37;
    constexpr std::uint8_t SD_DET = 38;
    
    constexpr std::uint8_t RAM_CS = 41;

    void SetupPins()
    {
        gpio_set_function(NET_MISO, GPIO_FUNC_SPI);
        gpio_set_function(NET_MOSI, GPIO_FUNC_SPI);
        gpio_set_function(NET_CLK, GPIO_FUNC_SPI);

        gpio_set_function(W5500_CS, GPIO_FUNC_SIO);
        gpio_set_function(W5500_INT, GPIO_FUNC_SIO);
        gpio_set_function(W5500_RST, GPIO_FUNC_SIO);

        gpio_set_function(RADIO_CS, GPIO_FUNC_SIO);
        gpio_set_function(RADIO_INT, GPIO_FUNC_SIO);
        gpio_set_function(RADIO_EN, GPIO_FUNC_SIO);

        gpio_set_function(MODULE_MISO, GPIO_FUNC_SPI);
        gpio_set_function(MODULE_MOSI, GPIO_FUNC_SPI);
        gpio_set_function(MODULE_CLK, GPIO_FUNC_SPI);

        gpio_set_function(POWER_CS, GPIO_FUNC_SIO);
        gpio_set_function(POWER_INT, GPIO_FUNC_SIO);
        gpio_set_function(POWER_RST, GPIO_FUNC_SIO);

        for(std::uint8_t i = 0; i < CS_PINS.size(); i++)
        {
            gpio_set_function(CS_PINS[i], GPIO_FUNC_SIO);
        }
        gpio_set_function(CS_EN, GPIO_FUNC_SIO);

        gpio_set_function(SDA, GPIO_FUNC_I2C);
        gpio_set_function(SCL, GPIO_FUNC_I2C);
   }

    void SetModuleCS(std::int8_t moduleIndex)
    {
        if(moduleIndex < 0)
        {
            gpio_put(CS_EN, 1);
            return;
        }
        if(moduleIndex >= std::pow(2, CS_PINS.size()))
        {
            gpio_put(CS_EN, 1);
            //Soft Error log
            return;
        }
        gpio_put(CS_EN, 0);
        for(std::uint8_t i = 0; i < CS_PINS.size(); i++)
        {
            gpio_put(CS_PINS[i], (moduleIndex >> i) & 1);
        }
    }


}
