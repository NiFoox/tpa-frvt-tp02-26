#include <Arduino.h>

#include <AppConfig.h>
#include <Hardware.h>
#include <Display.h>
#include <TelegramBot.h>

void setup() {

    Serial.begin(Config::SERIAL_BAUDRATE);

    Serial.println();
    Serial.println("[INFO] Iniciando TP02");

    Hardware::iniciar();
    Display::iniciar();
    TelegramBot::iniciar();

    Serial.println("[INFO] Sistema listo.");
}

void loop() {

    // Polling no bloqueante: verifica mensajes de Telegram cada ~1 segundo.
    // Los mensajes se procesan en orden de llegada (FIFO).
    TelegramBot::procesar();
}
