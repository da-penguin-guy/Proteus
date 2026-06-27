// hw_config.c
#include "hw_config.h"
#include "Proteus.h" 

static sd_sdio_if_t sdio_if = {
    .CMD_gpio  = PICO_SDIO_CMD_PIN, // Pin 33
    .D0_gpio   = PICO_SDIO_D0_PIN,  // Pin 34
    .baud_rate = 125 * 1000 * 1000 / 6 
};

static sd_card_t sd_card = {
    .type                 = SD_IF_SDIO,
    .sdio_if_p            = &sdio_if,
    .use_card_detect      = true,
    .card_detect_gpio     = PICO_SDIO_DET_PIN, // Pin 38
    .card_detected_true   = 0,                 // 0 means card is present when pin is low (GND)
    .card_detect_use_pull = true,              // Use internal pull-up resistor
    .card_detect_pull_hi  = true               // Pull up high
};

// Satisfies the extern "C" declarations in hw_config.h
size_t sd_get_num() { return 1; }

sd_card_t* sd_get_by_num(size_t num) {
    if (0 == num) return &sd_card;
    return NULL;
}