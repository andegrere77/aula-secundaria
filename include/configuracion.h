#pragma once

#include <Arduino.h>

namespace Config
{
    constexpr uint32_t VELOCIDAD_SERIE = 115200;

    constexpr uint8_t PIN_LED_INTEGRADO = 2;

    constexpr uint32_t INTERVALO_PARPADEO = 1000;

    constexpr uint8_t DIRECCION_BME280 = 0x76;
}