#include "GestorFirebase.h"

#include "secrets.h"

GestorFirebase::GestorFirebase()
    : clienteAsync(clienteSSL),
      usuarioAuth(
          Secrets::FIREBASE_API_KEY,
          Secrets::FIREBASE_USER_EMAIL,
          Secrets::FIREBASE_USER_PASSWORD
      )
{
}

void GestorFirebase::iniciar()
{
    clienteSSL.setInsecure();

    initializeApp(clienteAsync, app, getAuth(usuarioAuth));

    app.getApp(baseDatos);
    baseDatos.url(Secrets::FIREBASE_DATABASE_URL);
}

void GestorFirebase::actualizar(bool wifiConectado)
{
    if (!wifiConectado)
    {
        firebaseConectado = false;
        return;
    }

    app.loop();

    firebaseConectado = app.ready();

    if (firebaseConectado)
    {
        actualizarUID();
    }
}

bool GestorFirebase::conectado() const
{
    return firebaseConectado;
}

const char* GestorFirebase::uid() const
{
    return uidUsuario;
}

void GestorFirebase::actualizarUID()
{
    String uid = app.getUid();

    if (uid.length() > 0)
    {
        snprintf(uidUsuario, sizeof(uidUsuario), "%s", uid.c_str());
    }
}