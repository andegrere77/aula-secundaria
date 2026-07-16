#pragma once

#include <Arduino.h>

class GestorOTA
{
public:
    void iniciar();
    void actualizar();

    bool disponible() const;

private:
    bool otaDisponible = false;
};