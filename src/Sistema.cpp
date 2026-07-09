#include "Sistema.h"

#include "configuracion.h"
#include "versiones.h"

void Sistema::iniciar()
{
    logger.iniciar(Config::VELOCIDAD_SERIE);

    pinMode(Config::PIN_LED_INTEGRADO, OUTPUT);

    mostrarBanner();

    wifi.iniciar();
    logger.info("Gestor WiFi iniciado");

    ntp.iniciar();
    logger.info("Gestor NTP iniciado");

    firebase.iniciar();
    logger.info("Gestor Firebase iniciado");

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
   
    semaforo.iniciar();
    logger.info("Semaforo WS2812 inicializado correctamente");

    zumbador.iniciar();
    logger.info("Zumbador activo inicializado correctamente");

    calibrarRuido();

}

void Sistema::actualizar()
{
    actualizarWiFi();
    actualizarNTP();
    actualizarFirebase();
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

        // logger.debug("Latido del sistema");
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
    actualizarEstadoAula();
    semaforo.actualizar(datos.ruido, datos.ruidoBase, datos.umbralRuido);
    zumbador.actualizar(datos.ruido, datos.umbralRuido);

  
}

void Sistema::calibrarRuido()
{
    logger.info("Calibrando ruido ambiente durante 10 segundos");

    uint32_t inicio = millis();

    float sumaRuido = 0.0f;
    uint16_t contador = 0;

    while (millis() - inicio < Config::TIEMPO_CALIBRACION_RUIDO)
    {
        sensorRuido.actualizar();

        sumaRuido += sensorRuido.ruidoDBA();
        contador++;

        semaforo.mostrarCalibracion();

        delay(100);
    }

    if (contador > 0)
    {
        datos.ruidoBase = sumaRuido / contador;
    }
    else
    {
        datos.ruidoBase = Config::RUIDO_BASE_DBA;
    }

    datos.umbralRuido = datos.ruidoBase + Config::MARGEN_RUIDO_DBA;

    logger.info("Calibracion de ruido finalizada");

    Serial.print("Ruido base : ");
    Serial.print(datos.ruidoBase, 1);
    Serial.println(" dBA");

    Serial.print("Umbral ruido : ");
    Serial.print(datos.umbralRuido, 1);
    Serial.println(" dBA");
}

void Sistema::imprimirLecturas()
{
    diagnostico.mostrar(
        datos,
        wifi.conectado(),
        wifi.ip(),
        ntp.sincronizado(),
        ntp.horaActual(),
        firebase.conectado(),
        firebase.uid(),
        bme280OK,
        ruidoOK,
        oledOK
    );
}

void Sistema::actualizarEstadoAula()
{
    if (datos.umbralRuido <= datos.ruidoBase)
    {
        datos.estado = EstadoAula::SILENCIO;
        return;
    }

    float proporcion =
        (datos.ruido - datos.ruidoBase) /
        (datos.umbralRuido - datos.ruidoBase);

    if (proporcion < 0.25f)
    {
        datos.estado = EstadoAula::SILENCIO;
    }
    else if (proporcion < 0.60f)
    {
        datos.estado = EstadoAula::NORMAL;
    }
    else if (proporcion < 1.0f)
    {
        datos.estado = EstadoAula::RUIDO;
    }
    else
    {
        datos.estado = EstadoAula::ALARMA;
    }
}

void Sistema::actualizarWiFi()
{
    wifi.actualizar();

    bool wifiActual = wifi.conectado();

    if (wifiActual != wifiAnterior)
    {
        wifiAnterior = wifiActual;

        if (wifiActual)
        {
            logger.info("WiFi conectado");

            Serial.print("IP: ");
            Serial.println(wifi.ip());
        }
        else
        {
            logger.error("WiFi desconectado");
        }
    }
}

void Sistema::actualizarNTP()
{
    ntp.actualizar(wifi.conectado());
}

void Sistema::actualizarFirebase()
{
    firebase.actualizar(wifi.conectado());
}