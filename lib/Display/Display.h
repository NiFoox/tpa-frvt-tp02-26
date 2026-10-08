#pragma once

#include <Arduino.h>

namespace Display {

    void iniciar();

    void mostrarMensaje(
        const String& titulo,
        const String& mensaje
    );

    void mostrarDHT22(
        float temperatura,
        float humedad
    );

    void mostrarPotenciometro(
        float voltaje
    );

    void mostrarLed(
        const String& nombre,
        bool encendido
    );

    void mostrarError(
        const String& mensaje
    );
}