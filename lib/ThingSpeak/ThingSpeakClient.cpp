#include "ThingSpeakClient.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <math.h>

#include <AppConfig.h>

namespace ThingSpeakClient {

bool enviarDHT22(float temperatura, float humedad) {
    if (isnan(temperatura) || isnan(humedad)) {
        Serial.println(
            "[THINGSPEAK] Error: valores invalidos (NaN). Envio cancelado."
        );
        return false;
    }

    Serial.printf(
        "[THINGSPEAK] Enviando -> Temp: %.1f C | Hum: %.1f %%\r\n",
        temperatura,
        humedad
    );

    HTTPClient http;

    String url = String("http://") +
                 Config::THINGSPEAK_HOST +
                 "/update";

    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String payload = String("api_key=") +
                     Config::THINGSPEAK_API_KEY +
                     "&field1=" + String(temperatura, 2) +
                     "&field2=" + String(humedad, 2);

    int httpCode = http.POST(payload);
    String respuesta = http.getString();
    http.end();

    if (httpCode == 200 && respuesta.toInt() > 0) {
        Serial.printf(
            "[THINGSPEAK] OK | HTTP: %d | Entry #: %s\r\n",
            httpCode,
            respuesta.c_str()
        );
        return true;
    }

    Serial.printf(
        "[THINGSPEAK] Error | HTTP: %d | Resp: %s\r\n",
        httpCode,
        respuesta.c_str()
    );
    return false;
}

}  // namespace ThingSpeakClient
