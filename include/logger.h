#pragma once

#include <Arduino.h>

enum class NivelLog
{
    ERROR,
    INFO,
    DEBUG
};

class Logger
{
public:
    void iniciar(uint32_t velocidad);
    void info(const char* mensaje);
    void error(const char* mensaje);
    void debug(const char* mensaje);

private:
    void escribir(NivelLog nivel, const char* mensaje);
};