#pragma once

namespace ThingSpeakClient {

    /**
     * Envía temperatura y humedad a ThingSpeak.
     * Valida que los valores sean numéricos antes de enviar.
     * Imprime en Serial el resultado del envío.
     *
     * @return true si el servidor respondió HTTP 200 con una entry válida.
     */
    bool enviarDHT22(float temperatura, float humedad);
}
