#include "PantallaOLED.h"
#include "configuracion.h"

PantallaOLED::PantallaOLED()
    : display(Config::OLED_ANCHO, Config::OLED_ALTO, &Wire, Config::OLED_RESET)
{
}

bool PantallaOLED::iniciar()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, Config::OLED_DIRECCION))
    {
        return false;
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.display();

    return true;
}

void PantallaOLED::mostrarInicio()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("aula-secundaria");
    display.println("----------------");
    display.println("Iniciando...");
    display.display();
}

void PantallaOLED::mostrarLecturas(const DatosSensores& datos)
{
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("aula-secundaria");
    display.println("----------------");

    display.setCursor(0, 18);
    display.print("Temp : ");
    display.print(datos.temperatura, 1);
    display.println(" C");

    display.setCursor(0, 30);
    display.print("Hum  : ");
    display.print(datos.humedad, 0);
    display.println(" %");

    display.setCursor(0, 42);
    display.print("Ruido: ");
    display.print(datos.ruido, 1);
    display.println(" dBA");

    display.setCursor(0, 54);
    display.print("Pres : ");
    display.print(datos.presion, 0);
    display.println(" hPa");

    display.display();
}