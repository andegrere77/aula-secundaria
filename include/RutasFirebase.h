#pragma once

#include <Arduino.h>
#include <stddef.h>

class RutasFirebase
{
public:
    static bool actual(
        char* destino,
        size_t tamanoDestino
    );

    static bool historico(
        char* destino,
        size_t tamanoDestino,
        const char* fecha,
        unsigned long timestamp
    );
    
    static bool versionOTA(
      char* destino,
      size_t tamanoDestino
    );

    static bool firmwareOTA(
      char* destino,
      size_t tamanoDestino
    );

    static bool fechaOTA(
      char* destino,
      size_t tamanoDestino
    );
    
  
};
