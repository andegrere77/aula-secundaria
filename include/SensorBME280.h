#pragma once

#include <Adafruit_BME280.h>

class SensorBME280
{
public:

    bool iniciar();

    bool actualizar();

    float temperatura() const;

    float humedad() const;

    float presion() const;

private:

    Adafruit_BME280 bme;

    float temperaturaActual = 0.0f;
    float humedadActual = 0.0f;
    float presionActual = 0.0f;
};