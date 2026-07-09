#include "Zumbador.h"
#include "configuracion.h"

void Zumbador::iniciar()
{
    pinMode(Config::PIN_ZUMBADOR, OUTPUT);
    digitalWrite(Config::PIN_ZUMBADOR, LOW);
}

void Zumbador::actualizar(float ruidoDBA, float umbralRuidoDBA)
{
    uint32_t ahora = millis();

    if (sonando)
    {
        if (ahora - inicioSonido >= Config::TIEMPO_ZUMBADOR)
        {
            detenerSonido();
            ultimoAviso = ahora;
        }

        return;
    }

    if (ahora - ultimoAviso < Config::TIEMPO_REPOSO_ZUMBADOR)
    {
        return;
    }

    if (ruidoDBA >= umbralRuidoDBA)
    {
        if (!contandoRuido)
        {
            contandoRuido = true;
            inicioRuidoAlto = ahora;
        }

        if (ahora - inicioRuidoAlto >= Config::TIEMPO_RUIDO_ALTO)
        {
            iniciarSonido();
            contandoRuido = false;
        }
    }
    else
    {
        contandoRuido = false;
        detenerSonido();
    }
}

void Zumbador::iniciarSonido()
{
    sonando = true;
    inicioSonido = millis();
    digitalWrite(Config::PIN_ZUMBADOR, HIGH);
}

void Zumbador::detenerSonido()
{
    sonando = false;
    digitalWrite(Config::PIN_ZUMBADOR, LOW);
}