#include "settings.h"
#include "sensorDS18B20.h"

#include "src/OneWire/OneWire.h"
#include "src/DallasTemperature/DallasTemperature.h"

OneWire oneWire(SENSOR_DS18B20_PIN);
DallasTemperature sensors(&oneWire);

void setupSensorDS18B20() {
  Serial.println(F("Starting DS18B20..."));
  sensors.begin();
  sensors.setResolution(SENSOR_DS18B20_PRECISION);
}

void loopSensorDS18B20(Conn* conn, float *temp) {
  static unsigned long lastSensorRead = 0;

  if (millis() - lastSensorRead > SENSOR_DS18B20_NOTIFY_INTERVAL) {
    lastSensorRead = millis();    
      
    sensors.requestTemperatures();
    *temp = sensors.getTempCByIndex(0);
    Serial.print("DS18B20: ");
    Serial.print(*temp);
    Serial.println("ºC");

    conn->notify_sensor("DS18B20/temperature", *temp);    
  }
}
