#include "Hardware.h"

#include <Arduino.h>
#include <DHTesp.h>

#include <AppConfig.h>

namespace {

DHTesp dht;

bool estadoLedVerde = false;
bool estadoLedAzul = false;

}

namespace Hardware {

void iniciar() {

    pinMode(Config::PIN_LED_VERDE, OUTPUT);
    pinMode(Config::PIN_LED_AZUL, OUTPUT);

    digitalWrite(Config::PIN_LED_VERDE, LOW);
    digitalWrite(Config::PIN_LED_AZUL, LOW);

    dht.setup(Config::PIN_DHT22, DHTesp::DHT22);
}

void setLedVerde(bool encendido) {

    estadoLedVerde = encendido;

    digitalWrite(
        Config::PIN_LED_VERDE,
        encendido ? HIGH : LOW
    );
}

void setLedAzul(bool encendido) {

    estadoLedAzul = encendido;

    digitalWrite(
        Config::PIN_LED_AZUL,
        encendido ? HIGH : LOW
    );
}

bool getLedVerde() {
    return estadoLedVerde;
}

bool getLedAzul() {
    return estadoLedAzul;
}

DatosDHT22 leerDHT22() {

    TempAndHumidity datos = dht.getTempAndHumidity();

    bool valido =
        !isnan(datos.temperature) &&
        !isnan(datos.humidity);

    return {
        datos.temperature,
        datos.humidity,
        valido
    };
}

int leerPotenciometroRaw() {
    return analogRead(Config::PIN_POTENCIOMETRO);
}

float leerVoltajePotenciometro() {

    int lectura = leerPotenciometroRaw();

    return lectura *
           Config::ADC_VOLTAGE /
           Config::ADC_MAX;
}

}