#include "TelegramBot.h"

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include <AppConfig.h>
#include <Hardware.h>
#include <Display.h>
#include <ThingSpeakClient.h>
#include <WifiService.h>

namespace {

WiFiClientSecure clienteSeguro;
UniversalTelegramBot* bot = nullptr;

unsigned long ultimoPoll = 0;
constexpr unsigned long INTERVALO_POLL_MS = 1000;

bool responder(const String& chatId, const String& texto) {

    if (!bot) {
        Serial.println("[BOT][ERROR] Bot no inicializado");
        return false;
    }

    bool enviado = bot->sendMessage(chatId, texto, "");

    if (enviado) {
        Serial.println("[BOT] Respuesta enviada correctamente");
    } else {
        Serial.println("[BOT][ERROR] No se pudo enviar la respuesta");
    }

    return enviado;
}

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

void cmdLed(const String& chatId, const String& texto) {
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

    responder(
        chatId,
        "❌ Comando LED no reconocido.\n"
        "Usar: /led23on, /led23off, /led2on, /led2off"
    );
}

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

void cmdPote(const String& chatId) {
    float voltaje = Hardware::leerVoltajePotenciometro();

    Serial.printf("[POTE] Voltaje: %.2f V\r\n", voltaje);

    char buf[60];
    snprintf(buf, sizeof(buf), "🎛 *Potenciómetro*\nVoltaje: %.2f V", voltaje);
    responder(chatId, String(buf));
}

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

void cmdDisplay(const String& chatId, const String& texto) {
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
        String subcmd = texto.substring(8);
        Display::mostrarError("Cmd no reconocido:\n" + subcmd);
        responder(
            chatId,
            "❌ Subcomando de display no reconocido: *" + texto + "*\n"
            "Opciones válidas: /displayled, /displaydht22, /displaypote"
        );
    }
}

void cmdDesconocido(const String& chatId, const String& texto) {
    responder(
        chatId,
        "❓ Comando no reconocido: *" + texto + "*\n"
        "Enviá /start para ver los comandos disponibles."
    );
}

void despacharMensaje(const telegramMessage& msg) {
    const String chatId   = msg.chat_id;
    const String usuario  = msg.from_name;
    String texto          = msg.text;
    texto.trim();
    texto.toLowerCase();

    Serial.printf(
        "[BOT] De: %s (@%s) | Cmd: %s\r\n",
        chatId.c_str(),
        usuario.c_str(),
        texto.c_str()
    );

    const String chatIdAutorizado = Config::TELEGRAM_CHAT_ID;

    // Solo restringir acceso cuando hay un Chat ID configurado
    if (!chatIdAutorizado.isEmpty() && chatId != chatIdAutorizado) {

        Serial.printf(
            "[BOT] Acceso denegado a: %s\r\n",
            chatId.c_str()
        );

        responder(chatId, "Acceso no autorizado.");
        return;
    }

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
}

}  // namespace

namespace TelegramBot {

void iniciar() {

    // Temporal: se revisara la validacion TLS al final del TP.
    clienteSeguro.setInsecure();

    bot = new UniversalTelegramBot(
        Config::TELEGRAM_BOT_TOKEN,
        clienteSeguro
    );

    Serial.println("[BOT] Servicio Telegram inicializado");
}

void procesar() {

    if (!bot || !WifiService::conectado()) {
        return;
    }

    unsigned long ahora = millis();

    if (ahora - ultimoPoll < INTERVALO_POLL_MS) {
        return;
    }

    ultimoPoll = ahora;

    int nuevosMensajes = bot->getUpdates(
        bot->last_message_received + 1
    );

    if (nuevosMensajes < 0) {
        Serial.println("[BOT] Error al consultar Telegram");
        return;
    }

    for (int i = 0; i < nuevosMensajes; i++) {
        despacharMensaje(bot->messages[i]);
    }
}

}  // namespace TelegramBot
