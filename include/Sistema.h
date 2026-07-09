#pragma once

#include <Arduino.h>

#include "logger.h"
#include "SensorBME280.h"
#include "SensorRuido.h"
#include "PantallaOLED.h"
#include "SemaforoLED.h"
#include "Zumbador.h"
#include "GestorWiFi.h"
#include "DatosAula.h"
#include "Diagnostico.h"
#include "GestorNTP.h"

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
    SemaforoLED semaforo;
    Zumbador zumbador;
    GestorWiFi wifi;
    DatosSensores datos;

    Diagnostico diagnostico;

    GestorNTP ntp;

    uint32_t ultimoParpadeo = 0;
    uint32_t ultimaLecturaBME280 = 0;
    uint32_t ultimaLecturaRuido = 0;
    uint32_t ultimoEstadoWiFi = 0;

    bool estadoLed = false;
    bool bme280OK = false;
    bool ruidoOK = false;
    bool oledOK = false;
    bool wifiAnterior = false;

    void mostrarBanner();
    void actualizarLed();
    void actualizarBME280();
    void actualizarRuido();
    void actualizarWiFi();
    void imprimirLecturas();
    void calibrarRuido();
    void actualizarEstadoAula();

    void actualizarNTP();

    
};