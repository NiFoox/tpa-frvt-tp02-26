#pragma once

#include <Arduino.h>

namespace Config {

    constexpr uint8_t PIN_LED_VERDE = 23;
    constexpr uint8_t PIN_LED_AZUL = 2;

    constexpr uint8_t PIN_DHT22 = 15;
    constexpr uint8_t PIN_POTENCIOMETRO = 34;

    constexpr uint8_t PIN_SDA = 21;
    constexpr uint8_t PIN_SCL = 22;

    constexpr uint8_t OLED_ADDRESS = 0x3C;
    constexpr uint16_t OLED_WIDTH = 128;
    constexpr uint16_t OLED_HEIGHT = 64;

    constexpr uint32_t SERIAL_BAUDRATE = 115200;

    constexpr float ADC_MAX = 4095.0f;
    constexpr float ADC_VOLTAGE = 3.3f;

    constexpr const char* WIFI_SSID     = "Wokwi-GUEST";
    constexpr const char* WIFI_PASSWORD = "";

    constexpr const char* TELEGRAM_BOT_TOKEN = "8494904347:AAHl97uzRIrcoSXcVtoOkzprc-RvKII6EmA";
    constexpr long TELEGRAM_CHAT_ID = 0;

    constexpr const char* THINGSPEAK_API_KEY = "9873XPJV5JGZ69LY";
    constexpr const char* THINGSPEAK_HOST    = "api.thingspeak.com";
}
