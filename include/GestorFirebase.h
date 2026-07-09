#pragma once

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

class GestorFirebase
{
public:
    GestorFirebase();

    void iniciar();
    void actualizar(bool wifiConectado);

    bool conectado() const;
    const char* uid() const;

private:
    WiFiClientSecure clienteSSL;
    AsyncClientClass clienteAsync;
    UserAuth usuarioAuth;
    FirebaseApp app;
    RealtimeDatabase baseDatos;

    bool firebaseConectado = false;
    char uidUsuario[64] = "";

    void actualizarUID();
};