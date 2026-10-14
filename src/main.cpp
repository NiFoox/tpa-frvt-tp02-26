#include <Arduino.h>

#include <AppConfig.h>
#include <Hardware.h>
#include <Display.h>

unsigned long ultimoTest = 0;

bool estadoLed = false;

void setup() {

    Serial.begin(Config::SERIAL_BAUDRATE);

    Serial.println();
    Serial.println("[INFO] Iniciando TP02");

    Hardware::iniciar();
    Display::iniciar();

    Serial.println("[INFO] Hardware inicializado");
}

void loop() {

    unsigned long ahora = millis();

    if (ahora - ultimoTest >= 2000) {

        ultimoTest = ahora;

        // LEDs
        estadoLed = !estadoLed;

        Hardware::setLedVerde(estadoLed);
        Hardware::setLedAzul(!estadoLed);

        // DHT22
        Hardware::DatosDHT22 dht =
            Hardware::leerDHT22();

        if (dht.valido) {

            Serial.printf(
                "[DHT22] Temp: %.1f C | Hum: %.1f %%\r\n",
                dht.temperatura,
                dht.humedad
            );

        } else {

            Serial.println(
                "[ERROR] Lectura DHT22 invalida"
            );
        }

        // Potenciómetro
        float voltaje =
            Hardware::leerVoltajePotenciometro();

        Serial.printf(
            "[POTE] Voltaje: %.2f V\r\n",
            voltaje
        );
    }
}