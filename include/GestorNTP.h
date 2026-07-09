#pragma once

#include <Arduino.h>

class GestorNTP
{
public:
    void iniciar();
    void actualizar(bool wifiConectado);

    bool sincronizado() const;
    const char* horaActual() const;
    unsigned long timestamp() const;

private:
    bool horaSincronizada = false;
    uint32_t ultimaComprobacion = 0;

    char bufferHora[25] = "--:--:--";

    void actualizarHora();
};