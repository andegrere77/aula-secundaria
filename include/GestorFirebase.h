#pragma once

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

#include "DatosAula.h"

#include "RutasFirebase.h"

class GestorFirebase
{
public:
    GestorFirebase();

    void iniciar();
    void actualizar(bool wifiConectado);

    bool conectado() const;
    const char* uid() const;
    const char* ultimoError() const;

   bool enviarActual(
    const DatosSensores& datos,
    const char* fecha,
    const char* hora,
    unsigned long timestamp
);

    bool enviarHistorico(
        const DatosSensores& datos,
        const char* fecha,
        const char* hora,
        unsigned long timestamp
);

private:
    WiFiClientSecure clienteSSL;
    AsyncClientClass clienteAsync;
    UserAuth usuarioAuth;
    FirebaseApp app;
    RealtimeDatabase baseDatos;

    bool firebaseConectado = false;

    bool construirJsonDatos(
        object_t& json,
        const DatosSensores& datos,
        const char* fecha,
        const char* hora,
        unsigned long timestamp
);


    char uidUsuario[64] = "";
    char mensajeError[160] = "";

    void actualizarUID();
    void guardarUltimoError();


};