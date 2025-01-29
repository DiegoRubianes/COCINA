// Definir variables globales
float SET_temperature = 29.0;
float DHT_humedad;
float DHT_temperature;
float MAX6675_temperature;
float tempMax = 50;
float tempMin = 10;

// Librerías
#include <max6675.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Ethernet.h>

// Pines y configuración
#define CONFIG_TCSCK_PIN 10
#define CONFIG_TCCS_PIN 11
#define CONFIG_TCDO_PIN 12
MAX6675 thermocouple(CONFIG_TCSCK_PIN, CONFIG_TCCS_PIN, CONFIG_TCDO_PIN);

int SENSOR = 2;
DHT dht(SENSOR, DHT11);

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Configuración de Ethernet
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(10, 0, 0, 177);
EthernetServer server(80);

void setup() {
  Serial.begin(9600);

  // Inicializar LCD
  lcd.init();
  lcd.backlight();

  // Inicializar DHT
  dht.begin();

  // Inicializar Ethernet
  //Ethernet.begin(mac);
  server.begin();
  Ethernet.begin(mac, ip);
  IPAddress newIp(192, 168, 1, 15);
  Ethernet.setLocalIP(newIp);  // change the IP addres

  Serial.print("Servidor iniciado en IP: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Leer sensores
  DHT_humedad = dht.readHumidity();
  DHT_temperature = dht.readTemperature();
  MAX6675_temperature = thermocouple.readCelsius();

  

  Serial.println(DHT_humedad);
  Serial.println(DHT_temperature);
  Serial.println(MAX6675_temperature);

  // Mostrar datos en LCD
  printlcd(SET_temperature, MAX6675_temperature, DHT_humedad, DHT_temperature);

  // Manejar clientes web
  EthernetClient client = server.available();
  if (client) {
    handleClient(client);
  }

  delay(1000); // Esperar un segundo
}

// Función para manejar el cliente
void handleClient(EthernetClient client) {
  String request = client.readStringUntil('\r');
  client.flush();

  // Procesar solicitudes de botones
  if (request.indexOf("GET /incrementar") >= 0) {
    SET_temperature += 0.5;
    
    if (SET_temperature > tempMax) {  // Límite máximo
      SET_temperature = tempMax;
    }
  } else if (request.indexOf("GET /decrementar") >= 0) {
    SET_temperature -= 0.5;
     if (SET_temperature < tempMin) {  // Límite mínimo
    SET_temperature = tempMin;
  }
  }

  // Enviar respuesta HTML
  client.print(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html; charset=UTF-8\r\n"
      "Connection: close\r\n"
      "\r\n");
  client.print("<!DOCTYPE HTML>");
  client.print("<html>");
  client.print("<head>");
  client.print("<title>Control de Temperatura</title>");
  client.print("<style>");
  client.print("body { background-color: #E5E7E9; color: #333; font-family: Arial, sans-serif; }");
  client.print("h1, h2 { text-align: center; }");
  client.print(".data { text-align: center ; font-size: 1.5em; }");
  client.print(".buttons { text-align: center; margin-top: 50px; }");
  client.print("button { padding: 10px 20px; font-size: 1em; }");
  client.print("</style>");
  client.print("</head>");
  client.print("<body>");
  client.print("<h1>Control de Temperatura</h1>");
  client.print("<h2>Temperatura actual: ");
  client.print(SET_temperature);
  client.print(" °C</h2>");
  client.print("<div class=\"data\">");
  client.print("<p><b>Temperatura termocupla:</b> ");
  client.print(MAX6675_temperature);
  client.print(" °C</p>");
  client.print("<p><b>Temperatura:</b> ");
  client.print(DHT_temperature);
  client.print(" °C</p>");
  client.print("<p><b>Humedad:</b> ");
  client.print(DHT_humedad);
  client.print(" %</p>");
  client.print("<p><b>Temperatura Máxima:</b> ");
  client.print(tempMax);
  client.print(" °C</p>");
  client.print("<p><b>Temperatura Mínima:</b> ");
  client.print(tempMin);
  client.print(" °C</p>");
  client.print("<div class=\"buttons\">");
  client.print("<form action=\"/incrementar\" method=\"GET\" style=\"display:inline;\">");
  client.print("<button type=\"submit\">Aumentar</button>");
  client.print("</form>");
  client.print("<form action=\"/decrementar\" method=\"GET\" style=\"display:inline; margin-left: 10px;\">");
  client.print("<button type=\"submit\">Disminuir</button>");
  client.print("</form>");
  client.print("</div>");
  client.print("</body>");
  client.print("</html>");

  client.stop(); // Cerrar conexión
}

// Función para mostrar datos en LCD
void printlcd(float t1, float t2, float h1, float t3) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S ");
  lcd.print(t1, 2);
  lcd.setCursor(0, 1);
  lcd.print("M ");
  lcd.print(t2, 2);
  lcd.setCursor(8, 0);
  lcd.print("H ");
  lcd.print(h1, 2);
  lcd.setCursor(8, 1);
  lcd.print("D ");
  lcd.print(t3, 2);
}
