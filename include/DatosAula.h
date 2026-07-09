#pragma once

enum class EstadoAula
{
    SILENCIO,
    NORMAL,
    RUIDO,
    ALARMA
};

inline const char* textoEstadoAula(EstadoAula estado)
{
    switch (estado)
    {
        case EstadoAula::SILENCIO:
            return "SILENCIO";

        case EstadoAula::NORMAL:
            return "NORMAL";

        case EstadoAula::RUIDO:
            return "RUIDO";

        case EstadoAula::ALARMA:
            return "ALARMA";

        default:
            return "DESCONOCIDO";
    }
}

struct DatosSensores
{
    float temperatura = 0.0f;
    float humedad = 0.0f;
    float presion = 0.0f;

    float ruido = 0.0f;
    float ruidoBase = 0.0f;
    float umbralRuido = 0.0f;
    float ruidoMaximo = 0.0f;

    EstadoAula estado = EstadoAula::SILENCIO;
};