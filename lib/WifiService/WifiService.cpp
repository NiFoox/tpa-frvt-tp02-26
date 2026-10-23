#include "WifiService.h"

#include <Arduino.h>
#include <WiFi.h>
#include <AppConfig.h>

namespace {

    unsigned long ultimoIntento = 0;
    bool estabaConectado = false;

}

namespace WifiService {

void iniciar() {

    WiFi.mode(WIFI_STA);
    WiFi.begin(Config::WIFI_SSID, Config::WIFI_PASSWORD);

    ultimoIntento = millis();
    estabaConectado = false;

    Serial.printf(
        "[WIFI] Conectando a %s\r\n",
        Config::WIFI_SSID
    );
}

void actualizar() {

    if (conectado()) {

        if (!estabaConectado) {

            Serial.print("[WIFI] Conectado. IP: ");
            Serial.println(WiFi.localIP());

            estabaConectado = true;
        }

        return;
    }

    // Detectar perdida de conexion
    if (estabaConectado) {

        Serial.println("[WIFI] Conexion perdida");
        estabaConectado = false;
    }

    unsigned long ahora = millis();

    // Reintento periodico sin bloquear el loop
    if (ahora - ultimoIntento >=
        Config::WIFI_RETRY_INTERVAL_MS) {

        ultimoIntento = ahora;

        Serial.println("[WIFI] Reintentando conexion...");
        WiFi.reconnect();
    }
}

bool conectado() {
    return WiFi.status() == WL_CONNECTED;
}

}
