#include "Sistema.h"

#include "configuracion.h"
#include "versiones.h"

void Sistema::iniciar()
{
    logger.iniciar(Config::VELOCIDAD_SERIE);

    pinMode(Config::PIN_LED_INTEGRADO, OUTPUT);

    mostrarBanner();

    logger.info("Sistema iniciado correctamente");

    bme280OK = sensorBME280.iniciar();

    if (bme280OK)
    {
        logger.info("BME280 inicializado correctamente");
    }
    else
    {
        logger.error("No se encuentra el BME280");
    }

    ruidoOK = sensorRuido.iniciar();

    if (ruidoOK)
    {
        logger.info("Sensor de ruido SEN0232 inicializado correctamente");
    }
    else
    {
        logger.error("No se pudo inicializar el sensor de ruido");
    }

    oledOK = pantalla.iniciar();

    if (oledOK)
    {
        logger.info("Pantalla OLED inicializada correctamente");
        pantalla.mostrarInicio();
    }
    else
    {
        logger.error("No se encuentra la pantalla OLED");
    }
}

void Sistema::actualizar()
{
    actualizarLed();
    actualizarBME280();
    actualizarRuido();
}

void Sistema::mostrarBanner()
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

void Sistema::actualizarLed()
{
    uint32_t ahora = millis();

    if (ahora - ultimoParpadeo >= Config::INTERVALO_PARPADEO)
    {
        ultimoParpadeo = ahora;

        estadoLed = !estadoLed;
        digitalWrite(Config::PIN_LED_INTEGRADO, estadoLed);

        logger.debug("Latido del sistema");
    }
}

void Sistema::actualizarBME280()
{
    uint32_t ahora = millis();

    if (ahora - ultimaLecturaBME280 < Config::INTERVALO_OLED)
    {
        return;
    }

    ultimaLecturaBME280 = ahora;

    if (!bme280OK)
    {
        logger.error("No se pueden actualizar lecturas: BME280 no disponible");
        return;
    }

    sensorBME280.actualizar();

    datos.temperatura = sensorBME280.temperatura();
    datos.humedad = sensorBME280.humedad();
    datos.presion = sensorBME280.presion();

    imprimirLecturas();

    if (oledOK)
    {
        pantalla.mostrarLecturas(datos);
    }
}

void Sistema::actualizarRuido()
{
    uint32_t ahora = millis();

    if (ahora - ultimaLecturaRuido < Config::INTERVALO_RUIDO)
    {
        return;
    }

    ultimaLecturaRuido = ahora;

    if (!ruidoOK)
    {
        logger.error("No se puede actualizar ruido: SEN0232 no disponible");
        return;
    }

    sensorRuido.actualizar();

    datos.ruido = sensorRuido.ruidoDBA();

    Serial.print("Ruido      : ");
    Serial.print(datos.ruido, 1);
    Serial.println(" dBA");
}

void Sistema::imprimirLecturas()
{
    Serial.println("--------------------------------");

    Serial.print("Temperatura : ");
    Serial.print(datos.temperatura);
    Serial.println(" C");

    Serial.print("Humedad     : ");
    Serial.print(datos.humedad);
    Serial.println(" %");

    Serial.print("Presion     : ");
    Serial.print(datos.presion);
    Serial.println(" hPa");
}