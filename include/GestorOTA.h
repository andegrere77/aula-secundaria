#pragma once

#include <Arduino.h>

class GestorOTA
{
public:
    void iniciar();
    void actualizar();

    bool disponible() const;
    bool actualizando() const;

    bool consultarVersionHTTPS(
        const char* url,
        char* destino,
        size_t tamanoDestino
    );

    bool actualizarDesdeHTTPS(
    const char* urlFirmware
    );

    bool esVersionSuperior(
    const char* versionDisponible,
    const char* versionInstalada
    ) const;

private:
    bool otaDisponible = false;
    bool otaEnCurso = false;

    unsigned int ultimoPorcentaje = 101;
};