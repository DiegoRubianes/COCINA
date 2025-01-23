
//DEFINIR VARIABLES
float SET_temperature = 29;
float DHT_humedad;
float DHT_temperature;
float MAX6675_temperature;

#include <max6675.h>
// CONFIGURACION DE LOS PINES UTILIZADOS PARA LA COMUNICACIÓN CON EL MAX6675
#define CONFIG_TCSCK_PIN 10
#define CONFIG_TCCS_PIN 11
#define CONFIG_TCDO_PIN 12
MAX6675 thermocouple(CONFIG_TCSCK_PIN, CONFIG_TCCS_PIN, CONFIG_TCDO_PIN); 

#include <SoftwareSerial.h>
//#define rxPin 5
//#define txPin 6
//SoftwareSerial miBlue(rxPin, txPin);

//#include <Wire.h> 

#include <DHT_U.h>
int SENSOR = 2;
DHT dht (SENSOR, DHT11);

//configur lcd
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); 



void printlcd(float t1,float t2,float h1,float t3)
{
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print(t1, 2);
  lcd.setCursor(0,1);
  lcd.print(t2, 2);  
  lcd.setCursor(6,0);
  lcd.print(t3, 2);
  lcd.setCursor(6,1);
  lcd.print(h1, 2);
  lcd.setCursor(13,0);
  //lcd.print(i,1);
  //lcd.print(c);
};

void printser (float t1, float t2, float h1)
{
Serial.println(t1);
Serial.println(t2, 2);
Serial.println(h1, 2);
};

//web server

#include "defines.h"

int reqCount = 0;                // number of requests received

EthernetServer server(80);


void setup() {
//int c = 0; 
//int b = 0; 
//arrancar puerto serie
//miBlue.begin(9600);
Serial.begin(9600);

lcd.init();
lcd.backlight();

dht.begin();

//Wire.begin();

// ESPERAR A QUE SE ESTABILICE LA ENERGIA
delay(500);

// web server

SerialDebug.begin(115200);

  while (!Serial && millis() < 5000);

  SerialDebug.print("\nStarting WebServer on ");
  SerialDebug.print(BOARD_NAME);
  SerialDebug.print(F(" with "));
  SerialDebug.println(SHIELD_TYPE);
  SerialDebug.println(ETHERNET_GENERIC_VERSION);


  ETG_LOGWARN(F("Default SPI pinout:"));
  ETG_LOGWARN1(F("MOSI:"), MOSI);
  ETG_LOGWARN1(F("MISO:"), MISO);
  ETG_LOGWARN1(F("SCK:"),  SCK);
  ETG_LOGWARN1(F("SS:"),   SS);
  ETG_LOGWARN(F("========================="));

 #define USE_THIS_SS_PIN   10   // For other boards


#if defined(BOARD_NAME)
  ETG_LOGWARN3(F("Board :"), BOARD_NAME, F(", setCsPin:"), USE_THIS_SS_PIN);
#else
  ETG_LOGWARN1(F("Unknown board setCsPin:"), USE_THIS_SS_PIN);
#endif

  
  // start the ethernet connection and the server:
  // Use DHCP dynamic IP and random mac
  uint16_t index = millis() % NUMBER_OF_MAC;
  // Use Static IP
  //Ethernet.begin(mac[index], ip);
  Ethernet.begin(mac[index]);

  //SPIClass SPI2(HSPI);
  //Ethernet.begin(mac[index], &SPI2);

  // Just info to know how to connect correctly
  // To change for other SPI

  ETG_LOGWARN(F("Currently Used SPI pinout:"));
  ETG_LOGWARN1(F("MOSI:"), MOSI);
  ETG_LOGWARN1(F("MISO:"), MISO);
  ETG_LOGWARN1(F("SCK:"),  SCK);
  ETG_LOGWARN1(F("SS:"),   SS);
  ETG_LOGWARN(F("========================="));


  SerialDebug.print(F("Using mac index = "));
  SerialDebug.println(index);

  SerialDebug.print(F("Connected! IP address: "));
  SerialDebug.println(Ethernet.localIP());

  // start the web server on port 80
  server.begin();
}
 
void loop() {
//DHT
DHT_humedad = dht.readHumidity();
DHT_temperature = dht.readTemperature();

//max6675
MAX6675_temperature = thermocouple.readCelsius();
  
// IMPRIMIR LA TEMPERATURA EN LA TERMINAL SERIAL
printser(MAX6675_temperature,DHT_temperature,DHT_humedad);

// IMPRIMIR LA TEMPERATURA EN LCD 
printlcd(SET_temperature,MAX6675_temperature,DHT_humedad,DHT_temperature);

//if (miBlue.available()) {
//c = miBlue.read(); 
//b = temperature_read;
//miBlue.write(b);
//}
  // ESPERAR UN SEGUNDO ENTRE LAS LECTURAS
delay(1000);
// web server
 // listen for incoming clients
  EthernetClient client = server.available();

  if (client)
  {
    SerialDebug.println(F("New client"));
    // an http request ends with a blank line
    bool currentLineIsBlank = true;

    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        SerialDebug.write(c);

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          SerialDebug.println(F("Sending response"));

          // send a standard http response header
          // use \r\n instead of many println statements to speedup data send
          client.print(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"  // the connection will be closed after completion of the response
            "Refresh: 20\r\n"        // refresh the page automatically every 20 sec
            "\r\n");
          client.print("<!DOCTYPE HTML>\r\n");
          client.print("<html>");
          client.print("<head>");
          client.print("<title>Este es solo un ejemplo</title>");
          client.print("</head>");
          client.print("<body bgcolor=»#E5E7E9»><h1><center>Pagina de temperaturas</center></h1><font size=»3″ face=»Comic Sans MS, Arial, MS Sans Serif»>");
          client.print("Aqui podemos colocar alguna <b>introduccion</b> al contenido que encontrarás… bla bla bla</font><h2><center><u>Este Es Un Subtitulo De La Pagina</u><center></h2><font size=»3″ face=»Comic Sans MS, Arial, MS Sans Serif»>");
          client.print("Aqui se encontrara el <i>desarrollo</i> con todo lo que quieras..</font></body>");
          client.print("</html>");
          //client.print("<html>\r\n");
          //client.print("<h2>Hello Lujan Te llamamos de World from ");
          //client.print(BOARD_NAME);
          //client.print("!</h2>\r\n");
          //client.print("Requests received: ");
          //client.print(++reqCount);
          //client.print("<br>\r\n");
          //client.print("Analog input A0: ");
          //client.print(analogRead(0));
          //client.print("<br>\r\n");
          //client.print("</html>\r\n");
          break;
        }

        if (c == '\n')
        {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }

    // give the web browser time to receive the data
    delay(10);

    // close the connection:
    client.stop();
    SerialDebug.println(F("Client disconnected"));
  }
}
