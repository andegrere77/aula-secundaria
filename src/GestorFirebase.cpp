#include "GestorFirebase.h"

#include "secrets.h"
#include "configuracion.h"

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

bool GestorFirebase::enviarActual(
    const DatosSensores& datos,
    const char* hora,
    unsigned long timestamp
)
{
    if (!firebaseConectado)
    {
        return false;
    }





object_t json;
JsonWriter writer;

writer.create(json, "timestamp", timestamp);
writer.create(json, "hora", hora);

writer.create(json, "temperatura", datos.temperatura);
writer.create(json, "humedad", datos.humedad);
writer.create(json, "presion", datos.presion);

writer.create(json, "ruido", datos.ruido);
writer.create(json, "ruidoBase", datos.ruidoBase);
writer.create(json, "umbralRuido", datos.umbralRuido);
writer.create(json, "estado", static_cast<int>(datos.estado));

String ruta = "/aulas/";
ruta += Config::ID_AULA;
ruta += "/actual";

return baseDatos.set<object_t>(clienteAsync, ruta.c_str(), json);
}

bool GestorFirebase::enviarPrueba()
{
    if (!firebaseConectado)
    {
        snprintf(mensajeError, sizeof(mensajeError), "Firebase no conectado");
        return false;
    }

    String ruta = "/aulas/";
    ruta += Config::ID_AULA;
    ruta += "/prueba";

    bool ok = baseDatos.set<bool>(clienteAsync, ruta.c_str(), true);

    if (!ok || clienteAsync.lastError().code() != 0)
    {
        snprintf(
            mensajeError,
            sizeof(mensajeError),
            "Error %d: %s",
            clienteAsync.lastError().code(),
            clienteAsync.lastError().message().c_str()
        );

        return false;
    }

    snprintf(mensajeError, sizeof(mensajeError), "OK");
    return true;
}

const char* GestorFirebase::ultimoError() const
{
    return mensajeError;
}