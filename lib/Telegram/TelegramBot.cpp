#include "TelegramBot.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include <AppConfig.h>
#include <Hardware.h>
#include <Display.h>
#include <ThingSpeakClient.h>

// ─── Variables internas del módulo ───────────────────────────────────────────

namespace {

WiFiClientSecure clienteSeguro;
UniversalTelegramBot* bot = nullptr;

unsigned long ultimoPoll = 0;
constexpr unsigned long INTERVALO_POLL_MS = 1000;

// ─── Helpers de respuesta ─────────────────────────────────────────────────────

void responder(const String& chatId, const String& texto) {
    bot->sendMessage(chatId, texto, "");
}

// ─── Mensaje de bienvenida ────────────────────────────────────────────────────

void cmdStart(const String& chatId) {
    String msg =
        "🤖 *Bot ESP32 - TP02*\n\n"
        "Comandos disponibles:\n\n"
        "🔆 *LEDs*\n"
        "  /led23on  — Enciende LED Verde (GPIO 23)\n"
        "  /led23off — Apaga LED Verde (GPIO 23)\n"
        "  /led2on   — Enciende LED Azul (GPIO 2)\n"
        "  /led2off  — Apaga LED Azul (GPIO 2)\n\n"
        "🌡 *Sensores*\n"
        "  /dht22 — Temperatura y humedad\n"
        "  /pote  — Voltaje del potenciómetro\n\n"
        "☁ *IoT*\n"
        "  /platiot — Enviar datos a ThingSpeak\n\n"
        "📺 *Display OLED*\n"
        "  /displayled   — Estado de los LEDs\n"
        "  /displaydht22 — Temp y humedad en pantalla\n"
        "  /displaypote  — Voltaje en pantalla";

    responder(chatId, msg);
}

// ─── Comandos de LEDs ─────────────────────────────────────────────────────────

void cmdLed(const String& chatId, const String& texto) {

    // Formato esperado: /led<gpio><on|off>
    // GPIO válidos: 23 (verde) o 2 (azul)

    struct Caso {
        const char* cmd;
        uint8_t gpio;
        bool estado;
        const char* nombre;
    };

    const Caso casos[] = {
        { "/led23on",  23, true,  "Verde (GPIO 23)" },
        { "/led23off", 23, false, "Verde (GPIO 23)" },
        { "/led2on",    2, true,  "Azul  (GPIO 2)"  },
        { "/led2off",   2, false, "Azul  (GPIO 2)"  },
    };

    for (const auto& c : casos) {
        if (texto == c.cmd) {
            if (c.gpio == 23) {
                Hardware::setLedVerde(c.estado);
            } else {
                Hardware::setLedAzul(c.estado);
            }

            String msg = String("💡 LED ") +
                         c.nombre +
                         (c.estado ? " *encendido*" : " *apagado*");

            responder(chatId, msg);
            return;
        }
    }

    // Formato /led<gpio><on|off> no reconocido
    responder(
        chatId,
        "❌ Comando LED no reconocido.\n"
        "Usar: /led23on, /led23off, /led2on, /led2off"
    );
}

// ─── Comando DHT22 ────────────────────────────────────────────────────────────

void cmdDht22(const String& chatId) {

    Hardware::DatosDHT22 dht = Hardware::leerDHT22();

    if (!dht.valido) {
        responder(chatId, "❌ Error: lectura DHT22 inválida. Intentá de nuevo.");
        return;
    }

    Serial.printf(
        "[DHT22] Temp: %.1f C | Hum: %.1f %%\r\n",
        dht.temperatura,
        dht.humedad
    );

    char buf[80];
    snprintf(
        buf, sizeof(buf),
        "🌡 *DHT22*\nTemperatura: %.1f °C\nHumedad: %.1f %%",
        dht.temperatura,
        dht.humedad
    );
    responder(chatId, String(buf));
}

// ─── Comando Potenciómetro ────────────────────────────────────────────────────

void cmdPote(const String& chatId) {

    float voltaje = Hardware::leerVoltajePotenciometro();

    Serial.printf("[POTE] Voltaje: %.2f V\r\n", voltaje);

    char buf[60];
    snprintf(buf, sizeof(buf), "🎛 *Potenciómetro*\nVoltaje: %.2f V", voltaje);
    responder(chatId, String(buf));
}

// ─── Comando PlatiOT ─────────────────────────────────────────────────────────

void cmdPlatiot(const String& chatId) {

    Hardware::DatosDHT22 dht = Hardware::leerDHT22();

    if (!dht.valido) {
        responder(
            chatId,
            "❌ Error: lectura DHT22 inválida. Envío a ThingSpeak cancelado."
        );
        return;
    }

    bool ok = ThingSpeakClient::enviarDHT22(dht.temperatura, dht.humedad);

    if (ok) {
        char buf[120];
        snprintf(
            buf, sizeof(buf),
            "☁ *ThingSpeak OK*\nTemp: %.1f °C | Hum: %.1f %% enviados correctamente.",
            dht.temperatura,
            dht.humedad
        );
        responder(chatId, String(buf));
    } else {
        responder(
            chatId,
            "❌ Error al enviar a ThingSpeak. Verificá la API Key y la conexión."
        );
    }
}

// ─── Comando Display ──────────────────────────────────────────────────────────

void cmdDisplay(const String& chatId, const String& texto) {

    // texto viene en minúsculas normalizadas, p.ej. "/displayled"

    if (texto == "/displayled") {

        bool verde = Hardware::getLedVerde();
        bool azul  = Hardware::getLedAzul();

        Display::mostrarMensaje(
            "LEDs",
            String("Verde: ") + (verde ? "ON" : "OFF") +
            "\nAzul:  " + (azul  ? "ON" : "OFF")
        );

        responder(
            chatId,
            String("📺 OLED: LEDs\nVerde: ") +
            (verde ? "ON" : "OFF") +
            " | Azul: " +
            (azul  ? "ON" : "OFF")
        );

    } else if (texto == "/displaydht22") {

        Hardware::DatosDHT22 dht = Hardware::leerDHT22();

        if (dht.valido) {
            Display::mostrarDHT22(dht.temperatura, dht.humedad);
            responder(chatId, "📺 OLED: mostrando DHT22.");
        } else {
            Display::mostrarError("DHT22 invalido");
            responder(chatId, "❌ Lectura DHT22 inválida. OLED muestra error.");
        }

    } else if (texto == "/displaypote") {

        float voltaje = Hardware::leerVoltajePotenciometro();
        Display::mostrarPotenciometro(voltaje);
        responder(chatId, "📺 OLED: mostrando potenciómetro.");

    } else {

        // Subcmd no reconocido: mostrar en OLED y notificar al usuario
        String subcmd = texto.substring(8); // quitar "/display"
        Display::mostrarError("Cmd no reconocido:\n" + subcmd);
        responder(
            chatId,
            "❌ Subcomando de display no reconocido: *" + texto + "*\n"
            "Opciones válidas: /displayled, /displaydht22, /displaypote"
        );
    }
}

// ─── Comando desconocido ──────────────────────────────────────────────────────

void cmdDesconocido(const String& chatId, const String& texto) {
    responder(
        chatId,
        "❓ Comando no reconocido: *" + texto + "*\n"
        "Enviá /start para ver los comandos disponibles."
    );
}

// ─── Dispatcher principal ─────────────────────────────────────────────────────

void despacharMensaje(const telegramMessage& msg) {

    const String chatId   = msg.chat_id;
    const String usuario  = msg.from_name;
    String texto          = msg.text;
    texto.trim();
    texto.toLowerCase();

    // Loguear petición en Serial con chat_id y username
    Serial.printf(
        "[BOT] De: %s (@%s) | Cmd: %s\r\n",
        chatId.c_str(),
        usuario.c_str(),
        texto.c_str()
    );

    // Filtro por CHAT_ID (si está configurado)
    if (Config::TELEGRAM_CHAT_ID != 0 &&
        chatId.toInt() != Config::TELEGRAM_CHAT_ID)
    {
        Serial.printf(
            "[BOT] Acceso denegado a: %s\r\n", chatId.c_str()
        );
        responder(chatId, "⛔ Acceso no autorizado.");
        return;
    }

    // Ruteo de comandos
    if (texto == "/start") {
        cmdStart(chatId);
    } else if (texto.startsWith("/led")) {
        cmdLed(chatId, texto);
    } else if (texto == "/dht22") {
        cmdDht22(chatId);
    } else if (texto == "/pote") {
        cmdPote(chatId);
    } else if (texto == "/platiot") {
        cmdPlatiot(chatId);
    } else if (texto.startsWith("/display")) {
        cmdDisplay(chatId, texto);
    } else {
        cmdDesconocido(chatId, texto);
    }

    Serial.printf("[BOT] Respuesta enviada a: %s\r\n\r\n", chatId.c_str());
}

}  // namespace anónimo

// ─── API pública ──────────────────────────────────────────────────────────────

namespace TelegramBot {

void iniciar() {

    Serial.printf("[WiFi] Conectando a: %s ", Config::WIFI_SSID);

    WiFi.begin(Config::WIFI_SSID, Config::WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.printf(
        "\r\n[WiFi] Conectado. IP: %s\r\n",
        WiFi.localIP().toString().c_str()
    );

    // Aceptar cualquier certificado SSL (adecuado para proyectos educativos;
    // en producción usar bot->setFingerprint o cargar el CA root)
    clienteSeguro.setInsecure();

    bot = new UniversalTelegramBot(Config::TELEGRAM_BOT_TOKEN, clienteSeguro);

    Serial.println("[BOT] Bot de Telegram inicializado. Esperando mensajes...");
}

void procesar() {

    if (!bot) return;

    unsigned long ahora = millis();

    if (ahora - ultimoPoll < INTERVALO_POLL_MS) return;

    ultimoPoll = ahora;

    // getUpdates devuelve el número de mensajes nuevos.
    // La biblioteca los almacena en bot->messages[0..N-1]
    // ordenados por message_id (FIFO estricto según la API de Telegram).
    int N = bot->getUpdates(bot->last_message_received + 1);

    // Procesar cada mensaje en orden de llegada
    for (int i = 0; i < N; i++) {
        despacharMensaje(bot->messages[i]);
    }
}

}  // namespace TelegramBot
