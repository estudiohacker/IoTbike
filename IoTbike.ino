/*
  Versão inicial baseada no projeto "Mini GPS Display using Ublox neo-6m and ESP8266"
  por Pranay SS, eTechPath, disponível em
  http://www.etechpath.com/mini-gps-display-using-ublox-neo-6m-module-and-esp8266-nodemcu/
*/

#include <U8g2lib.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Definitions
#define pinoMQ135 A0 //Analog input 0 of your arduino
float ppm = 0.0;

// GPIO where the DS18B20 is connected to
const int oneWireBus = D5;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


int key = 0;
double Home_LAT = 0.0;
double Home_LNG = 0.0;

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ D6, /* data=*/ D5, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

//sat20x20px logo
static const unsigned char u8g_logo_sat[] U8X8_PROGMEM = {
  0x00, 0x01, 0x00, 0x80, 0x07, 0x00, 0xc0, 0x06, 0x00, 0x60, 0x30, 0x00,
  0x60, 0x78, 0x00, 0xc0, 0xfc, 0x00, 0x00, 0xfe, 0x01, 0x00, 0xff, 0x01,
  0x80, 0xff, 0x00, 0xc0, 0x7f, 0x06, 0xc0, 0x3f, 0x06, 0x80, 0x1f, 0x0c,
  0x80, 0x4f, 0x06, 0x19, 0xc6, 0x03, 0x1b, 0x80, 0x01, 0x73, 0x00, 0x00,
  0x66, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x70, 0x00, 0x00
};

//wave10px logo
static const unsigned char u8g2_logo_wave[] U8X8_PROGMEM = {
  0xE0, 0x03, 0x18, 0x00, 0xC4, 0x01, 0x32, 0x00, 0x8A, 0x01, 0x69, 0x00,
  0x25, 0x00, 0x95, 0x01, 0x95, 0x01, 0x01, 0x00,
};

// The serial connection to the GPS device
#include <SoftwareSerial.h>
static const int RXPin = D4, TXPin = D3;
static const uint32_t GPSBaud = 9600;
SoftwareSerial ss(RXPin, TXPin);

//GPS Library
#include <TinyGPS++.h>
TinyGPSPlus gps;

//Program variables
double Lat;
double Long;
double Alt;
int day, month, year;
int num_sat, gps_speed;
String heading;

float temp;
int qualidade_ar;

unsigned long previousMillis = 0;
const long interval = 5000; //Intervalo para leitura temp e ar

#include "MQ135.h"
MQ135 mq135_sensor = MQ135(pinoMQ135);

void setup() {
  // OLED GND
  pinMode(D8, OUTPUT);
  digitalWrite(D8, LOW);

  // OLED VCC
  pinMode(D7, OUTPUT);
  digitalWrite(D7, HIGH);
  
  Serial.begin(9600);
  ss.begin(GPSBaud);
  u8g2.begin();

  u8g2.firstPage();
  do {
    print_page1();
  } while ( u8g2.nextPage() );
  delay(5000);
}

void loop() {
  readTemperaturaAr();
  
  Get_GPS(); //Get GPS data

  key = (key + 1);
  if (key<0 or key>5)
    key = 0;

  switch (key) {
    case 0:
      u8g2.firstPage();
      do {
        print_Clock();
      } while ( u8g2.nextPage() );
      delay(10);
      
      break;      
    case 1:
      u8g2.firstPage();
      do {
        print_speed();
      } while ( u8g2.nextPage() );
      delay(10);
      
      break;      
    case 2:
      u8g2.firstPage();
      do {
        print_location();
      } while ( u8g2.nextPage() );
      delay(10);
      
      break;      
    case 3:
      u8g2.firstPage();
      do {
        print_Trip();
      } while ( u8g2.nextPage() );
      delay(10);
      
      break;      
    case 4:
      u8g2.firstPage();
      do {
        print_temperatura();
      } while ( u8g2.nextPage() );      
      delay(10);

      break;
    case 5:
      u8g2.firstPage();
      do {
        print_ar();
      } while ( u8g2.nextPage() );
      delay(10);

      break;
  }
}

void print_page1() {
  u8g2.drawXBMP(0, 0, 20, 20, u8g_logo_sat);
  u8g2.setFont( u8g2_font_crox1cb_tf);
  //u8g2.setFont(u8g2_font_helvB12_tf);
  //u8g2.setFont(u8g2_font_timB12_tf);
  u8g2.setCursor(45, 20);
  u8g2.print("BIKE IoT");
  //u8g2.setFont(u8g2_font_7x13B_tf);
  u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
  u8g2.setCursor(25, 35);
  u8g2.print("by  Estudio Hacker");
  u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
  u8g2.setCursor(0, 60);
  u8g2.print("Carregando");
  u8g2.setFont(u8g2_font_glasstown_nbp_tf);
  u8g2.setCursor(40, 60);
  u8g2.print(" . . . . . ");
}

void print_Clock() {
  u8g2.setFont(u8g2_font_courB08_tn);
  u8g2.setCursor(105, 64);
  u8g2.print( num_sat);
  u8g2.drawXBMP(118, 54, 10, 10, u8g2_logo_wave);

  u8g2.setFont(u8g2_font_crox1cb_tf);
  u8g2.setCursor(20, 10);
  u8g2.print("GPS RELOGIO");
  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setFont(u8g2_font_t0_22b_tn);
  u8g2.setCursor(20, 42);
  printTime(gps.time);
  // u8g.print(gps.date);
  //Get_Date();
  u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
  u8g2.setCursor(0, 64);
  printDate(gps.date);
}

void print_speed() {
  u8g2.setFont(u8g2_font_crox1cb_tf);
  u8g2.setCursor(16, 10);
  u8g2.print("VELOCIDADE");
  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setFont(u8g2_font_t0_22b_tn);
  u8g2.setCursor(5, 42);
  u8g2.print(gps_speed , DEC);
  u8g2.setFont(u8g2_font_glasstown_nbp_tf);
  u8g2.setCursor(62, 42);
  u8g2.print("km/h");

  u8g2.setFont(u8g2_font_courB08_tn);
  u8g2.setCursor(105, 64);
  u8g2.print( num_sat);
  u8g2.drawXBMP(118, 54, 10, 10, u8g2_logo_wave);

  u8g2.setFont(u8g2_font_glasstown_nbp_tf);
  u8g2.setCursor(0, 64);
  u8g2.print("Direcao:");
  u8g2.setCursor(45, 64);
  u8g2.print( heading);
}

void print_location() {
  u8g2.setFont(u8g2_font_crox1cb_tf);
  u8g2.setCursor(10, 10);
  u8g2.print("COORDENADAS");
  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
  u8g2.setCursor(5, 28);
  u8g2.print("Long: ");
  u8g2.setCursor(40, 28);
  u8g2.print( Long, 6);

  u8g2.setCursor(5, 43);
  u8g2.print("Lat: ");
  u8g2.setCursor(40, 43);
  u8g2.print( Lat, 6);

  u8g2.setCursor(0, 64);
  u8g2.print("Alt: ");
  u8g2.setCursor(20, 64);
  u8g2.print( Alt, 3);

  u8g2.setFont(u8g2_font_courB08_tn);
  u8g2.setCursor(105, 64);
  u8g2.print( num_sat);
  u8g2.drawXBMP(118, 54, 10, 10, u8g2_logo_wave);
}

void print_temperatura() {
  u8g2.setFont(u8g2_font_crox1cb_tf);
  u8g2.setCursor(10, 10);
  u8g2.print("TEMPERATURA");
  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setFont(u8g2_font_fub20_tn);
  u8g2.setCursor(14, 45);
  u8g2.print(temp, 1);
  u8g2.setFont(u8g2_font_inb16_mf);
  u8g2.drawGlyph(72, 45, 0x00b0); // degree
  u8g2.drawStr(88, 45, "C");
}

void print_ar() {
  u8g2.setFont(u8g2_font_crox1cb_tf);
  u8g2.setCursor(10, 10);
  u8g2.print("QUALIDADE AR");
  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setFont(u8g2_font_fub14_tn);
  u8g2.setCursor(14, 45);
  u8g2.print(ppm, 2);
  u8g2.setFont(u8g2_font_inb16_mf);
  
  u8g2.drawStr(88, 45, "ppm");
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
  if (gps.location.isValid() == 1) {
    if ((Home_LAT == 0.0) && (Home_LNG == 0.0)) {
      Home_LAT = gps.location.lat();
      Home_LNG = gps.location.lng();
    }
    
    Lat = gps.location.lat();
    Long = gps.location.lng();
    Alt = gps.altitude.meters();
    
    gps_speed = gps.speed.kmph();
    
    heading = gps.cardinal(gps.course.value());
  }

  Serial.print(F("GPS: "));
  Serial.print(F("Lat="));
  Serial.print(gps.location.lat(), 6);
  Serial.print(F(" Long="));
  Serial.print(gps.location.lng(), 6);

  Serial.print(F(" Dia="));
  Serial.print(gps.date.year());
  Serial.print(F("/"));
  Serial.print(gps.date.month());
  Serial.print(F("/"));
  Serial.print(gps.date.day());

  Serial.print(F(" Hora="));
  Serial.print(gps.time.hour());
  Serial.print(F(":"));
  Serial.print(gps.time.minute());
  Serial.print(F(":"));
  Serial.print(gps.time.second());

  Serial.print(F(" Velocidade="));
  Serial.print(gps.speed.kmph());
  Serial.print(F("km/h="));

  Serial.print(F(" Altitude="));
  Serial.print(gps.altitude.meters());
  Serial.print(F("m"));
  
  Serial.print(F(" Satélites="));
  Serial.print(gps.satellites.value());

  Serial.println();

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
  }
}

void readTemperaturaAr() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      
      sensors.requestTemperatures();
      temp = sensors.getTempCByIndex(0);
      Serial.print("Temperatura: ");
      Serial.print(temp);
      Serial.println("ºC");
    
      Serial.print("MQ-135:");
      
      float rzero = mq135_sensor.getRZero();
//      float correctedRZero = mq135_sensor.getCorrectedRZero(temp, humidity);
      float resistance = mq135_sensor.getResistance();
      ppm = mq135_sensor.getPPM();
//      float correctedPPM = mq135_sensor.getCorrectedPPM(temp, humidity);
    
      Serial.print(" RZero=");
      Serial.print(rzero);
//      Serial.print(" Corrected RZero=");
//      Serial.print(correctedRZero);
      Serial.print(" Resistance=");
      Serial.print(resistance);
      Serial.print(" PPM=");
      Serial.println(ppm);
//      Serial.print(" Corrected PPM=");
//      Serial.print(correctedPPM);
//      Serial.println("ppm");
  }  
}

static void printDate(TinyGPSDate &d) {
  if (!d.isValid()) {
    u8g2.print(F("******** "));
  }
  else {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.day(), d.month(), d.year());
    u8g2.print(sz);
  }
}
static void printTime(TinyGPSTime &t) {
  if (!t.isValid()) {
    u8g2.print(F("******** "));
  }
  else {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    u8g2.print(sz);
  }

  // printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

void print_Trip() {
  unsigned long distanceKm =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      Home_LAT,
      Home_LNG ) / 1000.0;
  u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
  u8g2.setCursor(0, 20);
  u8g2.print("Distancia: ");
  u8g2.setCursor(60, 20);
  u8g2.print(distanceKm);
  u8g2.setCursor(90, 20);
  u8g2.print("Km");

  double courseTo =
    TinyGPSPlus::courseTo(
      gps.location.lat(),
      gps.location.lng(),
      Home_LAT,
      Home_LNG );

  u8g2.setCursor(0, 30);
  u8g2.print("Curso: ");
  u8g2.setCursor(60, 30);
  u8g2.print(courseTo);
  u8g2.setCursor(90, 30);
  u8g2.print("Km");

  String cardinalTo = TinyGPSPlus::cardinal(courseTo);
  u8g2.setCursor(0, 40);
  u8g2.print("Direcao: ");
  u8g2.setCursor(60, 40);
  u8g2.print(cardinalTo);
}
