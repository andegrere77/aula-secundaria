#include "SensorBME280.h"
#include "configuracion.h"

bool SensorBME280::iniciar()
{
    return bme.begin(Config::DIRECCION_BME280);
}

bool SensorBME280::actualizar()
{
    temperaturaActual = bme.readTemperature();

    humedadActual = bme.readHumidity();

    presionActual = bme.readPressure() / 100.0F;

    return true;
}

float SensorBME280::temperatura() const
{
    return temperaturaActual;
}

float SensorBME280::humedad() const
{
    return humedadActual;
}

float SensorBME280::presion() const
{
    return presionActual;
}