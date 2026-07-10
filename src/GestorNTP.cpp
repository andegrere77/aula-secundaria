#include "GestorNTP.h"

#include <time.h>

#include "configuracion.h"

void GestorNTP::iniciar()
{
    configTime(
        Config::NTP_GMT_OFFSET,
        Config::NTP_OFFSET_VERANO,
        Config::NTP_SERVIDOR
    );

    ultimaComprobacion = 0;
}

void GestorNTP::actualizar(bool wifiConectado)
{
    if (!wifiConectado)
    {
        horaSincronizada = false;
        snprintf(bufferHora, sizeof(bufferHora), "--:--:--");
        return;
    }

    uint32_t ahora = millis();

    if (ultimaComprobacion != 0 &&
    ahora - ultimaComprobacion < Config::INTERVALO_NTP)
    {
        return;
    }

    ultimaComprobacion = ahora;
    actualizarHora();
}

bool GestorNTP::sincronizado() const
{
    return horaSincronizada;
}

const char* GestorNTP::horaActual() const
{
    return bufferHora;
}

unsigned long GestorNTP::timestamp() const
{
    time_t ahora;
    time(&ahora);
    return static_cast<unsigned long>(ahora);
}

void GestorNTP::actualizarHora()
{
    struct tm tiempo;

    if (!getLocalTime(&tiempo))
    {
        horaSincronizada = false;
        snprintf(bufferHora, sizeof(bufferHora), "--:--:--");
        return;
    }

    horaSincronizada = true;

    snprintf(
        bufferHora,
        sizeof(bufferHora),
        "%02d:%02d:%02d",
        tiempo.tm_hour,
        tiempo.tm_min,
        tiempo.tm_sec
    );

    snprintf(
        bufferFecha,
        sizeof(bufferFecha),
        "%04d-%02d-%02d",
        tiempo.tm_year + 1900,
        tiempo.tm_mon + 1,
        tiempo.tm_mday
);
}

const char* GestorNTP::fechaActual() const
{
    return bufferFecha;
}