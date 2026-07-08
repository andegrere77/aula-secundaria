#include "logger.h"

void Logger::iniciar(uint32_t velocidad)
{
    Serial.begin(velocidad);
    delay(1000);
}

void Logger::info(const char* mensaje)
{
    escribir(NivelLog::INFO, mensaje);
}

void Logger::error(const char* mensaje)
{
    escribir(NivelLog::ERROR, mensaje);
}

void Logger::debug(const char* mensaje)
{
    escribir(NivelLog::DEBUG, mensaje);
}

void Logger::escribir(NivelLog nivel, const char* mensaje)
{
    switch (nivel)
    {
        case NivelLog::ERROR:
            Serial.print("[ERROR] ");
            break;

        case NivelLog::INFO:
            Serial.print("[INFO ] ");
            break;

        case NivelLog::DEBUG:
            Serial.print("[DEBUG] ");
            break;
    }

    Serial.println(mensaje);
}