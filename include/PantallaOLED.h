#pragma once

#include <Adafruit_SSD1306.h>
#include "DatosAula.h"

class PantallaOLED
{
public:
    PantallaOLED();

    bool iniciar();
    void mostrarInicio();
    void mostrarLecturas(const DatosSensores& datos);

private:
    Adafruit_SSD1306 display;
};