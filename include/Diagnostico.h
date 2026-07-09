#pragma once

#include <Arduino.h>
#include "DatosAula.h"

class Diagnostico
{
public:
    void mostrar(
        const DatosSensores& datos,
        bool wifiConectado,
        const char* ip,
        bool ntpSincronizado,
        const char* hora,
        bool bme280OK,
        bool ruidoOK,
        bool oledOK
    );
};