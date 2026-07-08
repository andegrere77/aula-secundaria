#pragma once

#include <Arduino.h>

class Zumbador
{
public:
    void iniciar();
    void actualizar(float ruidoDBA, float umbralRuidoDBA);

private:
    bool contandoRuido = false;

    uint32_t inicioRuidoAlto = 0;
    uint32_t ultimoAviso = 0;

    void sonar();
};