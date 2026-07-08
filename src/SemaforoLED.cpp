#include "SemaforoLED.h"
#include "configuracion.h"

SemaforoLED::SemaforoLED()
    : tira(Config::NUM_LEDS, Config::PIN_LEDS, NEO_GRB + NEO_KHZ800)
{
}

void SemaforoLED::iniciar()
{
    tira.begin();
    tira.setBrightness(Config::BRILLO_LEDS);
    tira.show();
}

void SemaforoLED::actualizar(float ruidoDBA, float ruidoBaseDBA, float umbralRuidoDBA)
{
    uint8_t ledsRojos = 0;

    if (ruidoDBA >= umbralRuidoDBA)
    {
        ledsRojos = Config::NUM_LEDS;
    }
    else
    {
        float proporcion =
            (ruidoDBA - ruidoBaseDBA) /
            (umbralRuidoDBA - ruidoBaseDBA);

        proporcion = constrain(proporcion, 0.0f, 1.0f);

        ledsRojos = round(proporcion * (Config::NUM_LEDS - 1));
    }

    mostrarVerdeRojo(ledsRojos);
}

void SemaforoLED::mostrarCalibracion()
{
    for (uint8_t i = 0; i < Config::NUM_LEDS; i++)
    {
        tira.setPixelColor(i, tira.Color(0, 0, 0));
    }

    tira.setPixelColor(posicionCalibracion, tira.Color(0, 0, 255));
    tira.show();

    posicionCalibracion++;

    if (posicionCalibracion >= Config::NUM_LEDS)
    {
        posicionCalibracion = 0;
    }
}

void SemaforoLED::mostrarVerdeRojo(uint8_t ledsRojos)
{
    uint8_t ledsVerdes = Config::NUM_LEDS - ledsRojos;

    for (uint8_t i = 0; i < Config::NUM_LEDS; i++)
    {
        if (i < ledsVerdes)
        {
            tira.setPixelColor(i, tira.Color(0, 255, 0));
        }
        else
        {
            tira.setPixelColor(i, tira.Color(255, 0, 0));
        }
    }

    tira.show();
}