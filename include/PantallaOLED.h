#pragma once

#include <Adafruit_SSD1306.h>

class PantallaOLED
{
public:
    PantallaOLED();

    bool iniciar();
    void mostrarInicio();
    void mostrarLecturas(float temperatura, float humedad, float presion);

private:
    Adafruit_SSD1306 display;
};