#pragma once

#include <Arduino.h>

class SensorRuido
{
public:
    bool iniciar();
    bool actualizar();

    float ruidoDBA() const;

private:
    float ruidoActual = 0.0f;
};