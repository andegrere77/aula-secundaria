#pragma once

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

#include "DatosAula.h"

class GestorFirebase
{
public:
    GestorFirebase();

    void iniciar();
    void actualizar(bool wifiConectado);

    bool conectado() const;
    const char* uid() const;

    bool enviarActual(const DatosSensores& datos, const char* hora, unsigned long timestamp);

    bool enviarPrueba();
    const char* ultimoError() const;

private:
    WiFiClientSecure clienteSSL;
    AsyncClientClass clienteAsync;
    UserAuth usuarioAuth;
    FirebaseApp app;
    RealtimeDatabase baseDatos;

    bool firebaseConectado = false;
    char uidUsuario[64] = "";

    char mensajeError[128] = "";

    void actualizarUID();
};