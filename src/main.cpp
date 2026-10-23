
#include <Arduino.h>

#include <AppConfig.h>
#include <Hardware.h>
#include <Display.h>
#include <WifiService.h>
#include <TelegramBot.h>

void setup() {

    Serial.begin(Config::SERIAL_BAUDRATE);

    Serial.println();
    Serial.println("[INFO] Iniciando TP02");

    Hardware::iniciar();
    Display::iniciar();

    WifiService::iniciar();
    TelegramBot::iniciar();

    Serial.println("[INFO] Modulos inicializados");
}

void loop() {

    WifiService::actualizar();
    TelegramBot::procesar();

}
