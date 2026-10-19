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

    // WiFi
    constexpr const char* WIFI_SSID     = "Wokwi-GUEST";
    constexpr const char* WIFI_PASSWORD = "";

    // Telegram Bot
    // Obtener el token con @BotFather en Telegram
    constexpr const char* TELEGRAM_BOT_TOKEN = "8494904347:AAHl97uzRIrcoSXcVtoOkzprc-RvKII6EmA";
    // 0 = acepta mensajes de cualquier usuario;
    // reemplazar con tu chat_id para restringir el acceso
    constexpr long TELEGRAM_CHAT_ID = 0;

    // ThingSpeak
    // Crear canal en thingspeak.com con field1=temperatura, field2=humedad
    // y pegar aqui la Write API Key
    constexpr const char* THINGSPEAK_API_KEY = "9873XPJV5JGZ69LY";
    constexpr const char* THINGSPEAK_HOST    = "api.thingspeak.com";
}
