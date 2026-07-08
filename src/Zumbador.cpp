#include "Zumbador.h"
#include "configuracion.h"

void Zumbador::iniciar()
{
    pinMode(Config::PIN_ZUMBADOR, OUTPUT);
    digitalWrite(Config::PIN_ZUMBADOR, LOW);
}

void Zumbador::actualizar(float ruidoDBA)
{
    uint32_t ahora = millis();

    if (ahora - ultimoAviso < Config::TIEMPO_REPOSO_ZUMBADOR)
    {
        return;
    }

    if (ruidoDBA >= Config::UMBRAL_RUIDO_DBA)
    {
        if (!contandoRuido)
        {
            contandoRuido = true;
            inicioRuidoAlto = ahora;
        }

        if (ahora - inicioRuidoAlto >= Config::TIEMPO_RUIDO_ALTO)
        {
            sonar();
            ultimoAviso = millis();
            contandoRuido = false;
        }
    }
    else
    {
        contandoRuido = false;
        digitalWrite(Config::PIN_ZUMBADOR, LOW);
    }
}

void Zumbador::sonar()
{
    digitalWrite(Config::PIN_ZUMBADOR, HIGH);
    delay(Config::TIEMPO_ZUMBADOR);
    digitalWrite(Config::PIN_ZUMBADOR, LOW);
}