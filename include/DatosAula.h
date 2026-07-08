#pragma once

enum class EstadoAula
{
    SILENCIO,
    NORMAL,
    RUIDO,
    ALARMA
};

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