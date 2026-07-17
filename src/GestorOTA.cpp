#include "GestorOTA.h"

#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <Update.h>
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
    clienteSeguro.setInsecure();
    clienteSeguro.setTimeout(10);

    HTTPClient http;

    http.setConnectTimeout(10000);
    http.setTimeout(15000);
    http.setReuse(false);

    /*
     * HTTP/1.0 evita la transferencia chunked.
     * Si el servidor no indica el tamaño, el final
     * se detecta cuando cierra la conexión.
     */
    http.useHTTP10(true);

    if (!http.begin(clienteSeguro, urlFirmware))
    {
        Serial.println(
            "[ERROR] No se pudo iniciar la descarga HTTPS"
        );

        clienteSeguro.stop();
        return false;
    }

    http.addHeader(
        "Accept-Encoding",
        "identity"
    );

    int codigoHTTP = http.GET();

    if (codigoHTTP != HTTP_CODE_OK)
    {
        Serial.print("[ERROR] Respuesta HTTP: ");
        Serial.println(codigoHTTP);

        http.end();
        clienteSeguro.stop();

        return false;
    }

    int tamanoIndicado = http.getSize();

    Serial.print("[OTA  ] Tamano anunciado: ");

    if (tamanoIndicado > 0)
    {
        Serial.print(tamanoIndicado);
        Serial.println(" bytes");
    }
    else
    {
        Serial.println(
            "desconocido; se leera hasta cerrar la conexion"
        );
    }

    if (!Update.begin(UPDATE_SIZE_UNKNOWN))
    {
        Serial.print(
            "[ERROR] No se pudo iniciar Update: "
        );

        Update.printError(Serial);

        http.end();
        clienteSeguro.stop();

        return false;
    }

    WiFiClient* flujo = http.getStreamPtr();

    uint8_t buffer[1024];

    size_t bytesTotales = 0;
    int ultimoPorcentaje = -1;

    uint32_t ultimoDatoRecibido = millis();

    Serial.println(
        "[OTA  ] Comenzando descarga del firmware"
    );

    while (http.connected() || flujo->available() > 0)
    {
        size_t disponibles = flujo->available();

        if (disponibles > 0)
        {
            size_t cantidadLeer = disponibles;

            if (cantidadLeer > sizeof(buffer))
            {
                cantidadLeer = sizeof(buffer);
            }

            int bytesLeidos = flujo->readBytes(
                buffer,
                cantidadLeer
            );

            if (bytesLeidos <= 0)
            {
                continue;
            }

            /*
             * El primer byte de un firmware ESP32
             * válido debe ser 0xE9.
             */
            if (bytesTotales == 0 &&
                buffer[0] != 0xE9)
            {
                Serial.printf(
                    "[ERROR] Archivo no valido. "
                    "Primer byte: 0x%02X\n",
                    buffer[0]
                );

                Update.abort();

                http.end();
                clienteSeguro.stop();

                return false;
            }

            size_t bytesEscritos = Update.write(
                buffer,
                static_cast<size_t>(bytesLeidos)
            );

            if (bytesEscritos !=
                static_cast<size_t>(bytesLeidos))
            {
                Serial.println(
                    "[ERROR] Fallo escribiendo firmware"
                );

                Update.printError(Serial);
                Update.abort();

                http.end();
                clienteSeguro.stop();

                return false;
            }

            bytesTotales += bytesEscritos;
            ultimoDatoRecibido = millis();

            if (tamanoIndicado > 0)
            {
                int porcentaje =
                    static_cast<int>(
                        (bytesTotales * 100ULL) /
                        tamanoIndicado
                    );

                if (porcentaje != ultimoPorcentaje)
                {
                    ultimoPorcentaje = porcentaje;

                    Serial.printf(
                        "\r[OTA  ] Descarga: %d%%",
                        porcentaje
                    );
                }
            }
            else if (
                bytesTotales % (50 * 1024) <
                bytesEscritos)
            {
                Serial.printf(
                    "\r[OTA  ] Recibidos: %u KB",
                    static_cast<unsigned int>(
                        bytesTotales / 1024
                    )
                );
            }
        }
        else
        {
            /*
             * Evita quedar bloqueado indefinidamente
             * si la red desaparece a mitad de descarga.
             */
            if (millis() - ultimoDatoRecibido > 15000)
            {
                Serial.println();
                Serial.println(
                    "[ERROR] Tiempo agotado durante la descarga"
                );

                Update.abort();

                http.end();
                clienteSeguro.stop();

                return false;
            }

            delay(1);
        }
    }

    Serial.println();

    http.end();
    clienteSeguro.stop();

    if (bytesTotales == 0)
    {
        Serial.println(
            "[ERROR] No se recibieron datos del firmware"
        );

        Update.abort();
        return false;
    }

    /*
     * true permite finalizar usando el tamaño
     * realmente recibido.
     */
    if (!Update.end(true))
    {
        Serial.print(
            "[ERROR] No se pudo finalizar la actualizacion: "
        );

        Update.printError(Serial);

        return false;
    }

    if (Update.hasError())
    {
        Serial.print(
            "[ERROR] Update termino con error: "
        );

        Update.printError(Serial);

        return false;
    }

    Serial.printf(
        "[OTA  ] Firmware recibido: %u bytes\n",
        static_cast<unsigned int>(bytesTotales)
    );

    Serial.println(
        "[OTA  ] Actualizacion completada"
    );

    Serial.println(
        "[OTA  ] Reiniciando ESP32..."
    );

    Serial.flush();
    delay(500);

    ESP.restart();

    return true;
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