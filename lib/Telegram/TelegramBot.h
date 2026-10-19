#pragma once

#include <Arduino.h>

namespace TelegramBot {

    /**
     * Conecta a WiFi e inicializa el bot de Telegram.
     * Debe llamarse una sola vez en setup().
     */
    void iniciar();

    /**
     * Polling no bloqueante: verifica mensajes nuevos cada ~1 segundo.
     * Procesa todos los mensajes pendientes en orden de llegada (FIFO).
     * Debe llamarse en cada iteración de loop().
     */
    void procesar();
}
