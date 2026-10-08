#pragma once

namespace Hardware {

    struct DatosDHT22 {
        float temperatura;
        float humedad;
        bool valido;
    };

    void iniciar();

    void setLedVerde(bool encendido);
    void setLedAzul(bool encendido);

    bool getLedVerde();
    bool getLedAzul();

    DatosDHT22 leerDHT22();

    int leerPotenciometroRaw();
    float leerVoltajePotenciometro();
}