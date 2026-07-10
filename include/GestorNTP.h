#pragma once

#include <Arduino.h>

class GestorNTP
{

public:
    void iniciar();
    void actualizar(bool wifiConectado);

    bool sincronizado() const;
    const char* horaActual() const;
    const char* fechaActual() const;
    unsigned long timestamp() const;

private:
    bool horaSincronizada = false;

    uint32_t ultimaComprobacion = 0;

    char bufferHora[9] = "--:--:--";
    char bufferFecha[11] = "0000-00-00";

    void actualizarHora();   // ← Se mantiene
};
