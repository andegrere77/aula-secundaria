#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class SemaforoLED
{
public:
    SemaforoLED();

    void iniciar();
    void actualizar(float ruidoDBA);

private:
    Adafruit_NeoPixel tira;

    void mostrarVerdeRojo(uint8_t ledsRojos);
};