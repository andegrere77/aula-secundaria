#include "GestorWiFi.h"

#include <WiFi.h>

#include "configuracion.h"
#include "secrets.h"

void GestorWiFi::iniciar()
{
    WiFi.mode(WIFI_STA);
    conectar();
}

void GestorWiFi::actualizar()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFi.setSleep(false);
        wifiConectado = true;
        snprintf(ipActual, sizeof(ipActual), "%s", WiFi.localIP().toString().c_str());
        return;
    }

    wifiConectado = false;

    uint32_t ahora = millis();

    if (ahora - ultimoIntento >= Config::INTERVALO_WIFI)
    {
        ultimoIntento = ahora;
        conectar();
    }
}

bool GestorWiFi::conectado() const
{
    return wifiConectado;
}

const char* GestorWiFi::ip() const
{
    return ipActual;
}

void GestorWiFi::conectar()
{
    WiFi.disconnect();
    WiFi.begin(Secrets::WIFI_SSID, Secrets::WIFI_PASSWORD);
    
}