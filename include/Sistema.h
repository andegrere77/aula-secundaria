#pragma once

#include <Arduino.h>

#include "logger.h"
#include "SensorBME280.h"
#include "PantallaOLED.h"
#include "DatosAula.h"

class Sistema
{
public:
    void iniciar();
    void actualizar();

private:
    Logger logger;
    SensorBME280 sensorBME280;
    PantallaOLED pantalla;
    DatosSensores datos;

    uint32_t ultimoParpadeo = 0;
    uint32_t ultimaLectura = 0;

    bool estadoLed = false;
    bool bme280OK = false;
    bool oledOK = false;

    void mostrarBanner();
    void actualizarLed();
    void actualizarSensores();
    void imprimirLecturas();
};