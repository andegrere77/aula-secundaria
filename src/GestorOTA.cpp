#include "GestorOTA.h"

#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>


void GestorOTA::iniciar()
{
    ArduinoOTA.setHostname("aula-secundaria");

    ArduinoOTA.onStart([this]()
{
    otaEnCurso = true;
    ultimoPorcentaje = 101;

    Serial.println();
    Serial.println("[OTA  ] Iniciando actualizacion");
});

    ArduinoOTA.onEnd([this]()
    {
        otaEnCurso = false;

        Serial.println();
        Serial.println("[OTA  ] Actualizacion finalizada");
    });

    ArduinoOTA.onProgress([this](
      unsigned int progreso,
      unsigned int total
)
{
      if (total == 0)
      {
          return;
      }

      unsigned int porcentaje =
          static_cast<unsigned int>(
              (progreso * 100ULL) / total
          );

      if (porcentaje == ultimoPorcentaje)
      {
          return;
      }

      ultimoPorcentaje = porcentaje;

      Serial.printf(
          "\r[OTA  ] Progreso: %u%%",
          porcentaje
      );
});

   ArduinoOTA.onError([this](ota_error_t error)
{
    otaEnCurso = false;

    Serial.printf("\n[ERROR] OTA codigo %u: ", error);

    if (error == OTA_AUTH_ERROR)
    {
        Serial.println("Fallo de autenticacion");
    }
    else if (error == OTA_BEGIN_ERROR)
    {
        Serial.println("No se pudo iniciar la escritura");
    }
    else if (error == OTA_CONNECT_ERROR)
    {
        Serial.println("Fallo de conexion");
    }
    else if (error == OTA_RECEIVE_ERROR)
    {
        Serial.println("Fallo durante la recepcion");
    }
    else if (error == OTA_END_ERROR)
    {
        Serial.println("Fallo al finalizar la actualizacion");
    }
    else
    {
        Serial.println("Error desconocido");
    }
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

bool GestorOTA::actualizando() const
{
    return otaEnCurso;
}

bool GestorOTA::consultarVersionHTTPS(
    const char* url,
    char* destino,
    size_t tamanoDestino
)
{
    if (url == nullptr ||
        destino == nullptr ||
        tamanoDestino < 2)
    {
        return false;
    }

    destino[0] = '\0';

    WiFiClientSecure clienteSeguro;
    clienteSeguro.setInsecure();
    clienteSeguro.setTimeout(5);

    HTTPClient http;
    http.setConnectTimeout(5000);
    http.setTimeout(5000);
    http.setReuse(false);

    bool resultado = false;

    if (!http.begin(clienteSeguro, url))
    {
        Serial.println("[ERROR] No se pudo iniciar HTTPS");
        clienteSeguro.stop();
        return false;
    }

    int codigoHTTP = http.GET();

    if (codigoHTTP != HTTP_CODE_OK)
    {
        Serial.print("[ERROR] Respuesta HTTP: ");
        Serial.println(codigoHTTP);

        http.end();
        clienteSeguro.stop();
        return false;
    }

    WiFiClient* flujo = http.getStreamPtr();

    size_t posicion = 0;
    uint32_t ultimoDato = millis();

    while (http.connected() &&
           posicion < tamanoDestino - 1)
    {
        while (flujo->available() > 0 &&
               posicion < tamanoDestino - 1)
        {
            char caracter = flujo->read();

            if (caracter != '\r' &&
                caracter != '\n')
            {
                destino[posicion++] = caracter;
            }

            ultimoDato = millis();
        }

        if (millis() - ultimoDato > 5000)
        {
            Serial.println(
                "[ERROR] Tiempo agotado leyendo version.txt"
            );
            break;
        }

        delay(1);
    }

    destino[posicion] = '\0';

    if (posicion > 0)
    {
        resultado = true;
    }

    http.end();
    clienteSeguro.stop();

    return resultado;
}

bool GestorOTA::actualizarDesdeHTTPS(
    const char* urlFirmware
)
{
    if (urlFirmware == nullptr ||
        urlFirmware[0] == '\0')
    {
        Serial.println(
            "[ERROR] URL de firmware no valida"
        );

        return false;
    }

    Serial.println("--------------------------------");
    Serial.println("ACTUALIZACION HTTPS OTA");
    Serial.print("Firmware: ");
    Serial.println(urlFirmware);
    Serial.println("--------------------------------");

    WiFiClientSecure clienteSeguro;

    /*
     * Fase inicial del proyecto:
     * se usa HTTPS sin validar el certificado.
     */
    clienteSeguro.setInsecure();

    HTTPUpdate actualizador;

    actualizador.rebootOnUpdate(true);

    actualizador.onStart([]()
    {
        Serial.println(
            "[OTA  ] Comenzando descarga del firmware"
        );
    });

    actualizador.onProgress([](
        int progreso,
        int total
    )
    {
        if (total <= 0)
        {
            return;
        }

        int porcentaje =
            static_cast<int>(
                (progreso * 100LL) / total
            );

        static int ultimoPorcentajeMostrado = -1;

        if (porcentaje != ultimoPorcentajeMostrado)
        {
            ultimoPorcentajeMostrado = porcentaje;

            Serial.printf(
                "\r[OTA  ] Descarga: %d%%",
                porcentaje
            );
        }
    });

    actualizador.onEnd([]()
    {
        Serial.println();
        Serial.println(
            "[OTA  ] Firmware recibido correctamente"
        );
    });

    actualizador.onError([](
        int error
    )
    {
        Serial.println();

        Serial.print(
            "[ERROR] Actualizacion HTTPS: "
        );

        Serial.println(error);
    });

    t_httpUpdate_return resultado =
        actualizador.update(
            clienteSeguro,
            urlFirmware
        );

    switch (resultado)
    {
        case HTTP_UPDATE_FAILED:
            Serial.print(
                "[ERROR] Fallo HTTPUpdate: "
            );

            Serial.println(
                actualizador.getLastErrorString()
            );

            return false;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println(
                "[OTA  ] No hay una actualizacion disponible"
            );

            return false;

        case HTTP_UPDATE_OK:
            /*
             * Normalmente no se llega a mostrar porque
             * rebootOnUpdate(true) reinicia el ESP32.
             */
            Serial.println(
                "[OTA  ] Actualizacion completada"
            );

            return true;
    }

    return false;
}

bool GestorOTA::esVersionSuperior(
    const char* versionDisponible,
    const char* versionInstalada
) const
{
    if (versionDisponible == nullptr ||
        versionInstalada == nullptr)
    {
        return false;
    }

    int mayorDisponible = 0;
    int menorDisponible = 0;
    int parcheDisponible = 0;

    int mayorInstalada = 0;
    int menorInstalada = 0;
    int parcheInstalada = 0;

    if (sscanf(
            versionDisponible,
            "%d.%d.%d",
            &mayorDisponible,
            &menorDisponible,
            &parcheDisponible) != 3 ||
        sscanf(
            versionInstalada,
            "%d.%d.%d",
            &mayorInstalada,
            &menorInstalada,
            &parcheInstalada) != 3)
    {
        return false;
    }

    if (mayorDisponible != mayorInstalada)
    {
        return mayorDisponible > mayorInstalada;
    }

    if (menorDisponible != menorInstalada)
    {
        return menorDisponible > menorInstalada;
    }

    return parcheDisponible > parcheInstalada;
}