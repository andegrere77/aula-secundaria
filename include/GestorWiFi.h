#pragma once

#include <Arduino.h>

class GestorWiFi
{
public:
    void iniciar();
    void actualizar();

    bool conectado() const;
    const char* ip() const;

private:
    bool wifiConectado = false;
    char ipActual[20] = "0.0.0.0";

    uint32_t ultimoIntento = 0;

    void conectar();
};