#include "GestorFirebase.h"

#include "secrets.h"

#include "versiones.h"

#include "RutasFirebase.h"

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

bool GestorFirebase::construirJsonDatos(
    object_t& json,
    const DatosSensores& datos,
    const char* fecha,
    const char* hora,
    unsigned long timestamp
)
{
    object_t objTimestamp;
    object_t objFecha;
    object_t objHora;
    object_t objTemperatura;
    object_t objHumedad;
    object_t objPresion;
    object_t objRuido;
    object_t objRuidoBase;
    object_t objUmbral;
    object_t objEstado;
    object_t objVersionFirmware;

    JsonWriter escritor;

    escritor.create(objTimestamp, "timestamp", static_cast<uint32_t>(timestamp));
    escritor.create(objFecha, "fecha", string_t(fecha));
    escritor.create(objHora, "hora", string_t(hora));

    escritor.create(objTemperatura, "temperatura", number_t(datos.temperatura, 2));
    escritor.create(objHumedad, "humedad", number_t(datos.humedad, 2));
    escritor.create(objPresion, "presion", number_t(datos.presion, 2));

    escritor.create(objRuido, "ruido", number_t(datos.ruido, 1));
    escritor.create(objRuidoBase, "ruidoBase", number_t(datos.ruidoBase, 1));
    escritor.create(objUmbral, "umbralRuido", number_t(datos.umbralRuido, 1));

    escritor.create(objEstado, "estado", static_cast<int>(datos.estado));

    escritor.create(objVersionFirmware, "versionFirmware", string_t(Version::VERSION)
);

    escritor.join(
        json,
        11,
        objTimestamp,
        objFecha,
        objHora,
        objTemperatura,
        objHumedad,
        objPresion,
        objRuido,
        objRuidoBase,
        objUmbral,
        objEstado,
        objVersionFirmware
    );

    return true;
}




bool GestorFirebase::enviarActual(
    const DatosSensores& datos,
    const char* fecha,
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

object_t json;

if (!construirJsonDatos(
        json,
        datos,
        fecha,
        hora,
        timestamp))
{
    snprintf(
        mensajeError,
        sizeof(mensajeError),
        "No se pudo construir el JSON"
    );

    return false;
}
    char ruta[96];

    if (!RutasFirebase::actual(ruta, sizeof(ruta)))
    {
        snprintf(
            mensajeError,
            sizeof(mensajeError),
            "No se pudo construir la ruta de datos actuales"
        );

        return false;
    }

    bool resultado = baseDatos.set<object_t>(
        clienteAsync,
        ruta,
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

bool GestorFirebase::enviarHistorico(
    const DatosSensores& datos,
    const char* fecha,
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

    object_t json;

    if (!construirJsonDatos(
            json,
            datos,
            fecha,
            hora,
            timestamp))
    {
        snprintf(
            mensajeError,
            sizeof(mensajeError),
            "No se pudo construir el JSON"
        );

        return false;
    }

    char ruta[128];

    if (!RutasFirebase::historico(
            ruta,
            sizeof(ruta),
            fecha,
            timestamp))
    {
        snprintf(
            mensajeError,
            sizeof(mensajeError),
            "No se pudo construir la ruta del historico"
        );

        return false;
    }

    bool resultado = baseDatos.set<object_t>(
        clienteAsync,
        ruta,
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


bool GestorFirebase::consultarInformacionOTA()
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

    char rutaVersion[96];
    char rutaFirmware[96];
    char rutaFecha[96];

    if (!RutasFirebase::versionOTA(
            rutaVersion,
            sizeof(rutaVersion)) ||
        !RutasFirebase::firmwareOTA(
            rutaFirmware,
            sizeof(rutaFirmware)) ||
        !RutasFirebase::fechaOTA(
            rutaFecha,
            sizeof(rutaFecha)))
    {
        snprintf(
            mensajeError,
            sizeof(mensajeError),
            "No se pudieron construir las rutas OTA"
        );

        return false;
    }

    String version = baseDatos.get<String>(
        clienteAsync,
        rutaVersion
    );

    if (clienteAsync.lastError().code() != 0)
    {
        guardarUltimoError();
        return false;
    }

    String firmware = baseDatos.get<String>(
        clienteAsync,
        rutaFirmware
    );

    if (clienteAsync.lastError().code() != 0)
    {
        guardarUltimoError();
        return false;
    }

    String fecha = baseDatos.get<String>(
        clienteAsync,
        rutaFecha
    );

    if (clienteAsync.lastError().code() != 0)
    {
        guardarUltimoError();
        return false;
    }

    snprintf(
        versionRemota,
        sizeof(versionRemota),
        "%s",
        version.c_str()
    );

    snprintf(
        firmwareRemoto,
        sizeof(firmwareRemoto),
        "%s",
        firmware.c_str()
    );

    snprintf(
        fechaRemota,
        sizeof(fechaRemota),
        "%s",
        fecha.c_str()
    );

    snprintf(
        mensajeError,
        sizeof(mensajeError),
        "OK"
    );

    return true;
}

const char* GestorFirebase::versionOTADisponible() const
{
    return versionRemota;
}

const char* GestorFirebase::rutaFirmwareOTA() const
{
    return firmwareRemoto;
}

const char* GestorFirebase::fechaOTA() const
{
    return fechaRemota;
}


bool GestorFirebase::crearConfiguracionOTATemporal()
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

    char rutaVersion[96];
    char rutaFirmware[96];
    char rutaFecha[96];

    if (!RutasFirebase::versionOTA(
            rutaVersion,
            sizeof(rutaVersion)) ||
        !RutasFirebase::firmwareOTA(
            rutaFirmware,
            sizeof(rutaFirmware)) ||
        !RutasFirebase::fechaOTA(
            rutaFecha,
            sizeof(rutaFecha)))
    {
        snprintf(
            mensajeError,
            sizeof(mensajeError),
            "No se pudieron construir las rutas OTA"
        );

        return false;
    }

    bool versionCorrecta = baseDatos.set<String>(
        clienteAsync,
        rutaVersion,
        "0.23.0"
    );

    if (!versionCorrecta ||
        clienteAsync.lastError().code() != 0)
    {
        guardarUltimoError();
        return false;
    }

    bool firmwareCorrecto = baseDatos.set<String>(
        clienteAsync,
        rutaFirmware,
        "firmware/firmware.bin"
    );

    if (!firmwareCorrecto ||
        clienteAsync.lastError().code() != 0)
    {
        guardarUltimoError();
        return false;
    }

    bool fechaCorrecta = baseDatos.set<String>(
        clienteAsync,
        rutaFecha,
        "2026-07-16"
    );

    if (!fechaCorrecta ||
        clienteAsync.lastError().code() != 0)
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

bool GestorFirebase::descomponerVersion(
    const char* version,
    int& mayor,
    int& menor,
    int& parche
)
{
    if (version == nullptr || version[0] == '\0')
    {
        return false;
    }

    mayor = 0;
    menor = 0;
    parche = 0;

    int elementosLeidos = sscanf(
        version,
        "%d.%d.%d",
        &mayor,
        &menor,
        &parche
    );

    return elementosLeidos == 3;
}

bool GestorFirebase::hayActualizacionDisponible(
    const char* versionInstalada
) const
{
    if (versionInstalada == nullptr ||
        versionInstalada[0] == '\0' ||
        versionRemota[0] == '\0' ||
        strcmp(versionRemota, "null") == 0)
    {
        return false;
    }

    int mayorInstalada;
    int menorInstalada;
    int parcheInstalada;

    int mayorRemota;
    int menorRemota;
    int parcheRemota;

    if (!descomponerVersion(
            versionInstalada,
            mayorInstalada,
            menorInstalada,
            parcheInstalada) ||
        !descomponerVersion(
            versionRemota,
            mayorRemota,
            menorRemota,
            parcheRemota))
    {
        return false;
    }

    if (mayorRemota != mayorInstalada)
    {
        return mayorRemota > mayorInstalada;
    }

    if (menorRemota != menorInstalada)
    {
        return menorRemota > menorInstalada;
    }

    return parcheRemota > parcheInstalada;
}