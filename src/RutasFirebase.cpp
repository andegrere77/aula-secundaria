#include "RutasFirebase.h"

#include "configuracion.h"

bool RutasFirebase::actual(
    char* destino,
    size_t tamanoDestino
)
{
    if (destino == nullptr || tamanoDestino == 0)
    {
        return false;
    }

    int caracteresEscritos = snprintf(
        destino,
        tamanoDestino,
        "/aulas/%s/actual",
        Config::ID_AULA
    );

    return caracteresEscritos >= 0 &&
           static_cast<size_t>(caracteresEscritos) < tamanoDestino;
}

bool RutasFirebase::historico(
    char* destino,
    size_t tamanoDestino,
    const char* fecha,
    unsigned long timestamp
)
{
    if (
        destino == nullptr ||
        tamanoDestino == 0 ||
        fecha == nullptr
    )
    {
        return false;
    }

    int ano;
    int mes;
    int dia;

    int camposLeidos = sscanf(
        fecha,
        "%4d-%2d-%2d",
        &ano,
        &mes,
        &dia
    );

    if (camposLeidos != 3)
    {
        return false;
    }

    int caracteresEscritos = snprintf(
        destino,
        tamanoDestino,
        "/aulas/%s/historico/%04d/%02d/%02d/%lu",
        Config::ID_AULA,
        ano,
        mes,
        dia,
        timestamp
    );

    return caracteresEscritos >= 0 &&
           static_cast<size_t>(caracteresEscritos) < tamanoDestino;
}