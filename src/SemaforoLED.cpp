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

void SemaforoLED::actualizar(float ruidoDBA)
{
    uint8_t ledsRojos = 0;

    if (ruidoDBA >= Config::UMBRAL_RUIDO_DBA)
    {
        ledsRojos = Config::NUM_LEDS;
    }
    else
    {
        float proporcion =
            (ruidoDBA - Config::RUIDO_BASE_DBA) /
            (Config::UMBRAL_RUIDO_DBA - Config::RUIDO_BASE_DBA);

        proporcion = constrain(proporcion, 0.0f, 1.0f);

        ledsRojos = round(proporcion * (Config::NUM_LEDS - 1));
    }

    mostrarVerdeRojo(ledsRojos);
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