#include "src/TinyGPSPlus/TinyGPS++.h"
#include "src/Time/TimeLib.h"

#include "Display.h"

int key = 0;

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ D6, /* data=*/ D5, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

extern float ppm;

extern TinyGPSPlus gps;

extern double Home_LAT;
extern double Home_LNG;

extern double Lat;
extern double Long;
extern double Alt;
extern int num_sat, gps_speed;
extern String heading;

extern float temp;

void print_page1() {
  u8g2.setFont(u8g2_font_crox1cb_tf);
  u8g2.setCursor(25, 10);
  u8g2.print("BIKE IoT");
  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setFont(u8g2_font_crox1cb_tf);  
  u8g2.setCursor(0, 36);
  u8g2.print("Estudio Hacker");
  
  u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
  u8g2.setCursor(0, 60);
  u8g2.print("https://estudiohacker.io");
}

void print_Clock() {
  u8g2.setFont(u8g2_font_courB08_tn);
  u8g2.setCursor(105, 64);
  u8g2.print( num_sat);
  u8g2.drawXBMP(118, 54, 10, 10, u8g2_logo_wave);

  u8g2.setFont(u8g2_font_crox1cb_tf);
  u8g2.setCursor(35, 10);
  u8g2.print("RELOGIO");
  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setFont(u8g2_font_t0_22b_tn);
  u8g2.setCursor(20, 42);
  printTime();

  u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
  u8g2.setCursor(0, 64);
  printDate();
}

void print_speed() {
  u8g2.setFont(u8g2_font_crox1cb_tf);
  u8g2.setCursor(18, 10);
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
  u8g2.setCursor(0, 28);
  u8g2.print("Long: ");
  u8g2.setCursor(40, 28);
  u8g2.print( Long, 6);

  u8g2.setCursor(0, 43);
  u8g2.print("Lat: ");
  u8g2.setCursor(40, 43);
  u8g2.print( Lat, 6);

  u8g2.setCursor(0, 64);
  u8g2.print("Alt: ");
  u8g2.setCursor(20, 64);
  u8g2.print( Alt, 2);

  u8g2.setFont(u8g2_font_courB08_tn);
  u8g2.setCursor(105, 64);
  u8g2.print( num_sat);
  u8g2.drawXBMP(118, 54, 10, 10, u8g2_logo_wave);
}

void print_temperatura() {
  u8g2.setFont(u8g2_font_crox1cb_tf);
  u8g2.setCursor(14, 10);
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

static void printDate() {
  if (!gps.date.isValid()) {
    u8g2.print(F("******** "));
  }
  else {
    char sz[32];
    sprintf(sz, "%02d/%02d/%04d ", day(), month(), year());
    u8g2.print(sz);
  }
}
static void printTime() {
  if (!gps.time.isValid()) {
    u8g2.print(F("******** "));
  }
  else {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", hour(), minute(), second());
    u8g2.print(sz);
  }

  // printInt(d.age(), d.isValid(), 5);
  ////smartDelay(0);
}

void print_Trip() {
  u8g2.setFont(u8g2_font_crox1cb_tf);
  u8g2.setCursor(25, 10);
  u8g2.print("PERCURSO");
  u8g2.drawLine(0, 12, 128, 12);

  unsigned long distanceKm =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      Home_LAT,
      Home_LNG ) / 1000.0;
  u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
  u8g2.setCursor(0, 28);
  u8g2.print("Distancia: ");
  u8g2.setCursor(60, 28);
  u8g2.print(distanceKm);
  u8g2.setCursor(90, 28);
  u8g2.print("Km");

  double courseTo =
    TinyGPSPlus::courseTo(
      gps.location.lat(),
      gps.location.lng(),
      Home_LAT,
      Home_LNG ) / 1000.0;

  u8g2.setCursor(0, 43);
  u8g2.print("Curso: ");
  u8g2.setCursor(60, 43);
  u8g2.print(courseTo);
  u8g2.setCursor(90, 43);
  u8g2.print("Km");

  String cardinalTo = TinyGPSPlus::cardinal(courseTo);
  u8g2.setCursor(0, 64);
  u8g2.print("Direcao: ");
  u8g2.setCursor(60, 64);
  u8g2.print(cardinalTo);
}

void setupDisplay() {
  Serial.println(F("Starting Display..."));

  // Ajustando os pinos de alimentação do display
  // OLED GND
  pinMode(D8, OUTPUT);
  digitalWrite(D8, LOW);
  // OLED VCC
  pinMode(D7, OUTPUT);
  digitalWrite(D7, HIGH);  
  
  u8g2.begin();
  u8g2.firstPage();
  do {
    print_page1();
  } while ( u8g2.nextPage() );
}

void loopDisplay() {
  key = (key + 1);
  if (key<0 or key>5)
    key = 0;

  switch (key) {
    case 0:
      u8g2.firstPage();
      do {
        print_Clock();
      } while ( u8g2.nextPage() );
      break;      
    case 1:
      u8g2.firstPage();
      do {
        print_speed();
      } while ( u8g2.nextPage() );
      break;      
    case 2:
      u8g2.firstPage();
      do {
        print_location();
      } while ( u8g2.nextPage() );
      break;      
    case 3:
      u8g2.firstPage();
      do {
        print_Trip();
      } while ( u8g2.nextPage() );
      break;      
    case 4:
      u8g2.firstPage();
      do {
        print_temperatura();
      } while ( u8g2.nextPage() );      
      break;
    case 5:
      u8g2.firstPage();
      do {
        print_ar();
      } while ( u8g2.nextPage() );
      break;
  }
}
