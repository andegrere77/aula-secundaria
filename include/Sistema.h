#pragma once

#include <Arduino.h>

#include "logger.h"
#include "SensorBME280.h"
#include "SensorRuido.h"
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
    SensorRuido sensorRuido;
    PantallaOLED pantalla;
    DatosSensores datos;

    uint32_t ultimoParpadeo = 0;
    uint32_t ultimaLecturaBME280 = 0;
    uint32_t ultimaLecturaRuido = 0;

    bool estadoLed = false;
    bool bme280OK = false;
    bool ruidoOK = false;
    bool oledOK = false;

    void mostrarBanner();
    void actualizarLed();
    void actualizarBME280();
    void actualizarRuido();
    void imprimirLecturas();
};