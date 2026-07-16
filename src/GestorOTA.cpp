#include "GestorOTA.h"

#include <ArduinoOTA.h>

void GestorOTA::iniciar()
{
    
    Serial.println("[DEBUG] Entrando en GestorOTA::iniciar()");
    ArduinoOTA.setHostname("aula-secundaria");

    ArduinoOTA.onStart([]()
    {
        Serial.println("[OTA  ] Iniciando actualizacion");
    });

    ArduinoOTA.onEnd([]()
    {
        Serial.println();
        Serial.println("[OTA  ] Actualizacion finalizada");
    });

    ArduinoOTA.onProgress([](
        unsigned int progreso,
        unsigned int total
    )
    {
        const unsigned int porcentaje =
            progreso / (total / 100);

        Serial.printf(
            "\r[OTA  ] Progreso: %u%%",
            porcentaje
        );
    });

    ArduinoOTA.onError([](ota_error_t error)
    {
        Serial.printf(
            "\n[ERROR] Error OTA %u\n",
            error
        );
    });

    ArduinoOTA.begin();

    otaDisponible = true;

  
}

void GestorOTA::actualizar()
{
    if (!otaDisponible)
    {
        return;
    }

    ArduinoOTA.handle();
}

bool GestorOTA::disponible() const
{
    return otaDisponible;
}