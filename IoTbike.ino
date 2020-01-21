/*
  Versão inicial baseada no projeto "Mini GPS Display using Ublox neo-6m and ESP8266"
  por Pranay SS, eTechPath, disponível em
  http://www.etechpath.com/mini-gps-display-using-ublox-neo-6m-module-and-esp8266-nodemcu/
*/

#include "settings.h"
#include "debug.h"
#include "version.h"
#include "conn.h"
#include "Display.h"
#include "sensorDS18B20.h"
#include "sensorMQ135.h"

#include <SoftwareSerial.h>
#include "src/TinyGPSPlus/TinyGPS++.h"
#include "src/Time/TimeLib.h"

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

Conn* conn;
String stationID;

void setup() {
  // Mantemos o LED interno acesso para indicar que estamos no setup().
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  setupDebug();

  stationID = String(STATION_PREFIX) + String(ESP.getChipId());

  Serial.println();
  Serial.println();
  Serial.println(F("********************************************************************"));
  Serial.print(F("*****                IoT Bike by Estúdio Hacker                *****\n"));
  debugMsg("***** %s *****\n", title());

  setupDisplay();

  conn = new Conn(stationID);
  
  Serial.println(F("Starting GPS..."));
  ss.begin(GPSBaud);

  setupSensorDS18B20();

  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);  
}

void loop() {
  conn->loop();
    
  loopSensorMQ135(conn, &ppm);
  loopSensorDS18B20(conn, &temp);
  
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
  conn->notify_sensor("GPS/sat", num_sat);
  
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

    Serial.println();

    Lat = gps.location.lat();
    Long = gps.location.lng();
    Alt = gps.altitude.meters();
    
    gps_speed = gps.speed.kmph();
    
    heading = gps.cardinal(gps.course.value());

    conn->notify_sensor("GPS/lat", Lat);
    conn->notify_sensor("GPS/lng", Long);
    conn->notify_sensor("GPS/alt", Alt);
    conn->notify_sensor("GPS/speed", gps.speed.kmph());

    if ((Home_LAT == 0.0) && (Home_LNG == 0.0)) {
      Home_LAT = gps.location.lat();
      Home_LNG = gps.location.lng();
      
      conn->notify_sensor("GPS/home_lat", Home_LAT);
      conn->notify_sensor("GPS/home_lng", Home_LNG);
    }
  } else {
    Serial.println(); 
  }

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("GPS: não detectado. Verifique a conexão dos fios."));
  }
}
