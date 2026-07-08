#include <Arduino.h>

constexpr char NOMBRE_PROYECTO[] = "aula-secundaria";
constexpr char VERSION[] = "0.1.0";

void mostrarBanner()
{
    Serial.println();
    Serial.println(F("=============================================="));
    Serial.println(F("              aula-secundaria"));
    Serial.println(F("=============================================="));

    Serial.print(F("Versión : "));
    Serial.println(VERSION);

    Serial.print(F("Compilado: "));
    Serial.print(__DATE__);
    Serial.print(F(" "));
    Serial.println(__TIME__);

    Serial.println(F("=============================================="));
    Serial.println();
}

void setup()
{
    Serial.begin(115200);

    delay(1000);

    mostrarBanner();
}

void loop()
{
    delay(1000);
}