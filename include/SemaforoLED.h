#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class SemaforoLED
{
public:
    SemaforoLED();

    void iniciar();
    void actualizar(float ruidoDBA, float ruidoBaseDBA, float umbralRuidoDBA);
    void mostrarCalibracion();

private:
    Adafruit_NeoPixel tira;
    uint8_t posicionCalibracion = 0;

    void mostrarVerdeRojo(uint8_t ledsRojos);
};