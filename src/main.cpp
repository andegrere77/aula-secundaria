#include <Arduino.h>

#include "configuracion.h"
#include "versiones.h"
#include "logger.h"
#include "SensorBME280.h"
#include "PantallaOLED.h"

Logger logger;

SensorBME280 sensorBME280;

PantallaOLED pantalla;

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

    if (sensorBME280.iniciar())
    {
        logger.info("BME280 inicializado correctamente");
    }
    else
    {
        logger.error("No se encuentra el BME280");
    }
    
    if (pantalla.iniciar())
    {
        logger.info("Pantalla OLED inicializada correctamente");
        pantalla.mostrarInicio();
    }
    else
    {
        logger.error("No se encuentra la pantalla OLED");
    }
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

    static uint32_t ultimo = 0;

if (millis() - ultimo >= 1000)
{
    ultimo = millis();

    sensorBME280.actualizar();

    Serial.println("--------------------------------");

    Serial.print("Temperatura : ");
    Serial.print(sensorBME280.temperatura());
    Serial.println(" °C");

    Serial.print("Humedad     : ");
    Serial.print(sensorBME280.humedad());
    Serial.println(" %");

    Serial.print("Presion     : ");
    Serial.print(sensorBME280.presion());
    Serial.println(" hPa");


    pantalla.mostrarLecturas(
    sensorBME280.temperatura(),
    sensorBME280.humedad(),
    sensorBME280.presion()
);
}
}