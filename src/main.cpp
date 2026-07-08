#include <Arduino.h>

#include "configuracion.h"
#include "versiones.h"
#include "logger.h"

Logger logger;

unsigned long ultimoParpadeo = 0;
bool estadoLed = false;

void mostrarBanner()
{
    Serial.println();
    Serial.println("==============================================");
    Serial.println("              aula-secundaria");
    Serial.println("==============================================");

    Serial.print("Version : ");
    Serial.println(Version::VERSION);

    Serial.print("Descripcion : ");
    Serial.println(Version::DESCRIPCION);

    Serial.print("Compilado : ");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.println(__TIME__);

    Serial.println("==============================================");
    Serial.println();
}

void setup()
{
    logger.iniciar(Config::VELOCIDAD_SERIE);

    pinMode(Config::PIN_LED_INTEGRADO, OUTPUT);

    mostrarBanner();

    logger.info("Sistema iniciado correctamente");
}

void loop()
{
    unsigned long ahora = millis();

    if (ahora - ultimoParpadeo >= Config::INTERVALO_PARPADEO)
    {
        ultimoParpadeo = ahora;

        estadoLed = !estadoLed;
        digitalWrite(Config::PIN_LED_INTEGRADO, estadoLed);

        logger.debug("Latido del sistema");
    }
}