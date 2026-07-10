#include "GestorFirebase.h"

#include "configuracion.h"
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

    initializeApp(
        clienteAsync,
        app,
        getAuth(usuarioAuth)
    );

    app.getApp(baseDatos);
    baseDatos.url(Secrets::FIREBASE_DATABASE_URL);

    snprintf(
        mensajeError,
        sizeof(mensajeError),
        "Firebase inicializado"
    );
}

void GestorFirebase::actualizar(bool wifiConectado)
{
    if (!wifiConectado)
    {
        firebaseConectado = false;
        return;
    }

    // Estos dos métodos deben ejecutarse continuamente.
    app.loop();
    baseDatos.loop();

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

const char* GestorFirebase::ultimoError() const
{
    return mensajeError;
}

void GestorFirebase::actualizarUID()
{
    String uidActual = app.getUid();

    if (uidActual.length() > 0)
    {
        snprintf(
            uidUsuario,
            sizeof(uidUsuario),
            "%s",
            uidActual.c_str()
        );
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
        snprintf(
            mensajeError,
            sizeof(mensajeError),
            "Firebase no conectado"
        );

        return false;
    }

    /*
     * JsonWriter no acumula correctamente los campos llamando
     * repetidamente a create() sobre el mismo objeto.
     *
     * Creamos un objeto por campo y luego los unimos.
     */

    object_t objetoTimestamp;
    object_t objetoHora;
    object_t objetoTemperatura;
    object_t objetoHumedad;
    object_t objetoPresion;
    object_t objetoRuido;
    object_t objetoRuidoBase;
    object_t objetoUmbral;
    object_t objetoEstado;

    object_t json;

    JsonWriter escritor;

    escritor.create(
        objetoTimestamp,
        "timestamp",
        static_cast<uint32_t>(timestamp)
    );

    escritor.create(
        objetoHora,
        "hora",
        string_t(hora)
    );

    escritor.create(
        objetoTemperatura,
        "temperatura",
        number_t(datos.temperatura, 2)
    );

    escritor.create(
        objetoHumedad,
        "humedad",
        number_t(datos.humedad, 2)
    );

    escritor.create(
        objetoPresion,
        "presion",
        number_t(datos.presion, 2)
    );

    escritor.create(
        objetoRuido,
        "ruido",
        number_t(datos.ruido, 1)
    );

    escritor.create(
        objetoRuidoBase,
        "ruidoBase",
        number_t(datos.ruidoBase, 1)
    );

    escritor.create(
        objetoUmbral,
        "umbralRuido",
        number_t(datos.umbralRuido, 1)
    );

    escritor.create(
        objetoEstado,
        "estado",
        static_cast<int>(datos.estado)
    );

    escritor.join(
        json,
        9,
        objetoTimestamp,
        objetoHora,
        objetoTemperatura,
        objetoHumedad,
        objetoPresion,
        objetoRuido,
        objetoRuidoBase,
        objetoUmbral,
        objetoEstado
    );

    String ruta = "/aulas/";
    ruta += Config::ID_AULA;
    ruta += "/actual";

    bool resultado = baseDatos.set<object_t>(
        clienteAsync,
        ruta.c_str(),
        json
    );

    if (!resultado || clienteAsync.lastError().code() != 0)
    {
        guardarUltimoError();
        return false;
    }

    snprintf(
        mensajeError,
        sizeof(mensajeError),
        "OK"
    );

    return true;
}

void GestorFirebase::guardarUltimoError()
{
    int codigo = clienteAsync.lastError().code();
    String mensaje = clienteAsync.lastError().message();

    snprintf(
        mensajeError,
        sizeof(mensajeError),
        "Error %d: %s",
        codigo,
        mensaje.c_str()
    );
}