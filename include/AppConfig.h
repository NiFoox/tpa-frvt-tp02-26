#pragma once

#include <Arduino.h>

namespace Config {

    // LEDs indicados por la consigna
    constexpr uint8_t PIN_LED_VERDE = 23;
    constexpr uint8_t PIN_LED_AZUL = 2;

    // Sensores
    constexpr uint8_t PIN_DHT22 = 15;
    constexpr uint8_t PIN_POTENCIOMETRO = 34;

    // I2C OLED
    constexpr uint8_t PIN_SDA = 21;
    constexpr uint8_t PIN_SCL = 22;

    constexpr uint8_t OLED_ADDRESS = 0x3C;
    constexpr uint16_t OLED_WIDTH = 128;
    constexpr uint16_t OLED_HEIGHT = 64;

    constexpr uint32_t SERIAL_BAUDRATE = 115200;

    // ADC ESP32
    constexpr float ADC_MAX = 4095.0f;
    constexpr float ADC_VOLTAGE = 3.3f;
}