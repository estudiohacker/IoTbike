/*
  Versão inicial baseada no projeto "Mini GPS Display using Ublox neo-6m and ESP8266"
  por Pranay SS, eTechPath, disponível em
  http://www.etechpath.com/mini-gps-display-using-ublox-neo-6m-module-and-esp8266-nodemcu/
*/

#include <SoftwareSerial.h>

#include "src/TinyGPSPlus/TinyGPS++.h"
#include "src/Time/TimeLib.h"

#include "Display.h"
#include "sensorDS18B20.h"
#include "sensorMQ135.h"

//Definitions
float ppm = 0.0;

double Home_LAT = 0.0;
double Home_LNG = 0.0;

// The serial connection to the GPS device
static const int RXPin = D4, TXPin = D3;
static const uint32_t GPSBaud = 9600;
SoftwareSerial ss(RXPin, TXPin);

//GPS Library
TinyGPSPlus gps;

//Program variables
double Lat;
double Long;
double Alt;
int num_sat, gps_speed;
String heading;

float temp;

void setup() {
  // Mantemos o LED interno acesso para indicar que estamos no setup().
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.begin(74880);
  Serial.println();
  Serial.println();
  Serial.println(F("**************************"));
  Serial.println(F("IoT Bike by Estúdio Hacker"));
  
  Serial.println(F("Iniciando GPS..."));
  ss.begin(GPSBaud);

  Serial.println(F("Iniciando DS18B20..."));
  setupSensorDS18B20();

  Serial.println(F("Iniciando Display..."));
  setupDisplay();  

  digitalWrite(LED_BUILTIN, HIGH);  
}

void loop() {
  loopSensorMQ135(&ppm);
  loopSensorDS18B20(&temp);
  
  Get_GPS(); //Get GPS data

  loopDisplay();
}

String getISOTime() {
  static char isoTime[20];
  time_t t = now();
  sprintf(isoTime, "%04d-%02d-%02dT%02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));
  return String(isoTime);
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available()) {
      char c = ss.read();
//      Serial.write(c);
      gps.encode(c);
    }
  } while (millis() - start < ms);
}


void Get_GPS() {
  num_sat = gps.satellites.value();
  
  Serial.print(F("GPS:"));
  
  Serial.print(F(" Satélites="));
  Serial.print(gps.satellites.value());

  if (gps.time.isValid() && gps.date.isValid()) {
    if ((timeStatus() == timeNotSet) || (timeStatus() == timeNeedsSync)) {
      setTime(gps.time.hour(), gps.time.minute(), gps.time.second(), gps.date.day(), gps.date.month(), gps.date.year());    
      adjustTime(-3 * SECS_PER_HOUR);
    }
    Serial.print(F(" Data/Hora="));
    Serial.print(getISOTime().c_str());
  }

  if (gps.location.isValid()) {    
    Serial.print(F(" Lat="));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(" Long="));
    Serial.print(gps.location.lng(), 6);
    
    Serial.print(F(" Altitude="));
    Serial.print(gps.altitude.meters());
    Serial.print(F("m"));
    
    Serial.print(F(" Velocidade="));
    Serial.print(gps.speed.kmph());
    Serial.print(F("km/h"));

  
    Lat = gps.location.lat();
    Long = gps.location.lng();
    Alt = gps.altitude.meters();
    
    gps_speed = gps.speed.kmph();
    
    heading = gps.cardinal(gps.course.value());    

    if ((Home_LAT == 0.0) && (Home_LNG == 0.0)) {
      Home_LAT = gps.location.lat();
      Home_LNG = gps.location.lng();
    }
  }
  Serial.println();

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("GPS: não detectado. Verifique a conexão dos fios."));
  }
}
