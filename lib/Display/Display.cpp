#include "Display.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include <AppConfig.h>

namespace {

Adafruit_SSD1306 display(
    Config::OLED_WIDTH,
    Config::OLED_HEIGHT,
    &Wire,
    -1
);

void prepararPantalla() {

    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
}

}

namespace Display {

void iniciar() {

    Wire.begin(
        Config::PIN_SDA,
        Config::PIN_SCL
    );

    if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        Config::OLED_ADDRESS
    )) {
        Serial.println(
            "[ERROR] No se pudo inicializar el OLED"
        );

        return;
    }

    mostrarMensaje(
        "TP02",
        "Sistema iniciado"
    );
}

void mostrarMensaje(
    const String& titulo,
    const String& mensaje
) {

    prepararPantalla();

    display.println(titulo);
    display.println();
    display.println(mensaje);

    display.display();
}

void mostrarDHT22(
    float temperatura,
    float humedad
) {

    prepararPantalla();

    display.println("Sensor DHT22");
    display.println();

    display.print("Temp: ");
    display.print(temperatura, 1);
    display.println(" C");

    display.print("Hum: ");
    display.print(humedad, 1);
    display.println(" %");

    display.display();
}

void mostrarPotenciometro(
    float voltaje
) {

    prepararPantalla();

    display.println("Potenciometro");
    display.println();

    display.print("Voltaje: ");
    display.print(voltaje, 2);
    display.println(" V");

    display.display();
}

void mostrarLed(
    const String& nombre,
    bool encendido
) {

    prepararPantalla();

    display.println(nombre);
    display.println();

    display.println(
        encendido
            ? "Led encendido"
            : "Led apagado"
    );

    display.display();
}

void mostrarError(
    const String& mensaje
) {

    prepararPantalla();

    display.println("ERROR");
    display.println();
    display.println(mensaje);

    display.display();
}

}