#include "Diagnostico.h"

void Diagnostico::mostrar(
    const DatosSensores& datos,
    bool wifiConectado,
    const char* ip,
    bool bme280OK,
    bool ruidoOK,
    bool oledOK
)
{
    Serial.println("================================");

    Serial.print("Temperatura : ");
    Serial.print(datos.temperatura, 2);
    Serial.println(" C");

    Serial.print("Humedad     : ");
    Serial.print(datos.humedad, 2);
    Serial.println(" %");

    Serial.print("Presion     : ");
    Serial.print(datos.presion, 2);
    Serial.println(" hPa");

    Serial.print("Ruido       : ");
    Serial.print(datos.ruido, 1);
    Serial.println(" dBA");

    Serial.print("Ruido base  : ");
    Serial.print(datos.ruidoBase, 1);
    Serial.println(" dBA");

    Serial.print("Umbral      : ");
    Serial.print(datos.umbralRuido, 1);
    Serial.println(" dBA");

    Serial.print("Estado aula : ");
    Serial.println(textoEstadoAula(datos.estado));

    Serial.print("WiFi        : ");

    if (wifiConectado)
    {
        Serial.print("CONECTADO | IP: ");
        Serial.println(ip);
    }
    else
    {
        Serial.println("DESCONECTADO");
    }

    Serial.print("BME280      : ");
    Serial.println(bme280OK ? "OK" : "ERROR");

    Serial.print("SEN0232     : ");
    Serial.println(ruidoOK ? "OK" : "ERROR");

    Serial.print("OLED        : ");
    Serial.println(oledOK ? "OK" : "ERROR");

    Serial.println("================================");
}