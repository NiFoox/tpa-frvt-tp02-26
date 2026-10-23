#pragma once

#include <Arduino.h>
#include "Secrets.h"

namespace Config {

    // LEDs
    constexpr uint8_t PIN_LED_VERDE = 23;
    constexpr uint8_t PIN_LED_AZUL = 2;

    // Sensores
    constexpr uint8_t PIN_DHT22 = 15;
    constexpr uint8_t PIN_POTENCIOMETRO = 34;

    // Display OLED
    constexpr uint8_t PIN_SDA = 21;
    constexpr uint8_t PIN_SCL = 22;

    constexpr uint8_t OLED_ADDRESS = 0x3C;
    constexpr uint16_t OLED_WIDTH = 128;
    constexpr uint16_t OLED_HEIGHT = 64;

    // Puerto serie
    constexpr uint32_t SERIAL_BAUDRATE = 115200;

    // ADC
    constexpr float ADC_MAX = 4095.0f;
    constexpr float ADC_VOLTAGE = 3.3f;

    // WiFi: valores obtenidos de Secrets.h
    constexpr const char* WIFI_SSID =
        Secrets::WIFI_SSID;

    constexpr const char* WIFI_PASSWORD =
        Secrets::WIFI_PASSWORD;

    // Telegram
    constexpr const char* TELEGRAM_BOT_TOKEN =
        Secrets::TELEGRAM_BOT_TOKEN;

    constexpr const char* TELEGRAM_CHAT_ID =
        Secrets::TELEGRAM_CHAT_ID;

    // ThingSpeak
    constexpr const char* THINGSPEAK_API_KEY =
        Secrets::THINGSPEAK_API_KEY;

    constexpr const char* THINGSPEAK_HOST =
        "api.thingspeak.com";
}
