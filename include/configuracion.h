#pragma once

#include <Arduino.h>

namespace Config
{
    constexpr uint32_t VELOCIDAD_SERIE = 115200;

    constexpr uint8_t PIN_LED_INTEGRADO = 2;

    constexpr uint32_t INTERVALO_PARPADEO = 1000;

    constexpr uint8_t DIRECCION_BME280 = 0x76;

    constexpr uint8_t OLED_ANCHO = 128;
    constexpr uint8_t OLED_ALTO = 64;
    constexpr int8_t OLED_RESET = -1;
    constexpr uint8_t OLED_DIRECCION = 0x3C;
    constexpr uint32_t INTERVALO_OLED = 2000;

    constexpr uint8_t PIN_SONIDO = 34;
    constexpr uint32_t INTERVALO_RUIDO = 1000;

    constexpr uint8_t PIN_LEDS = 12;
    constexpr uint8_t NUM_LEDS = 8;
    constexpr uint8_t BRILLO_LEDS = 40;

    constexpr float RUIDO_BASE_DBA = 45.0f;
    constexpr float UMBRAL_RUIDO_DBA = 70.0f;

    constexpr uint8_t PIN_ZUMBADOR = 14;

    constexpr uint32_t TIEMPO_RUIDO_ALTO = 5000;
    constexpr uint32_t TIEMPO_ZUMBADOR = 5000;
    constexpr uint32_t TIEMPO_REPOSO_ZUMBADOR = 30000;

    constexpr uint32_t TIEMPO_CALIBRACION_RUIDO = 10000;
    constexpr float MARGEN_RUIDO_DBA = 20.0f; 
}