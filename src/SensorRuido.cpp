#include "SensorRuido.h"
#include "configuracion.h"

bool SensorRuido::iniciar()
{
    pinMode(Config::PIN_SONIDO, INPUT);
    return true;
}

bool SensorRuido::actualizar()
{
    const int muestras = 20;
    long sumaMilivoltios = 0;

    for (int i = 0; i < muestras; i++)
    {
        int milivoltios = analogReadMilliVolts(Config::PIN_SONIDO);
        sumaMilivoltios += milivoltios;
        delay(5);
    }

    float mediaMilivoltios = sumaMilivoltios / float(muestras);
    float voltaje = mediaMilivoltios / 1000.0f;

    ruidoActual = voltaje * 50.0f;

    return true;
}

float SensorRuido::ruidoDBA() const
{
    return ruidoActual;
}