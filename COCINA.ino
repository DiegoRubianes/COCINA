
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
}
