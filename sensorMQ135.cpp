#include "settings.h"
#include "sensorMQ135.h"

#include "src/MQ135/MQ135.h"

MQ135 mq135_sensor = MQ135(SENSOR_MQ135_PIN);

void setupSensorMQ135() {
  //
}

void loopSensorMQ135(Conn* conn, float *ppm) {
  static unsigned long lastSensorRead = 0;

  if (millis() - lastSensorRead > SENSOR_MQ135_NOTIFY_INTERVAL) {
    lastSensorRead = millis();    
      
    Serial.print("MQ135:");      
    float rzero = mq135_sensor.getRZero();
    float resistance = mq135_sensor.getResistance();
    *ppm = mq135_sensor.getPPM();
  
    Serial.print(" RZero=");
    Serial.print(rzero);
    Serial.print(" Resistance=");
    Serial.print(resistance);
    Serial.print(" PPM=");
    Serial.println(*ppm);

    conn->notify_sensor("MQ135/ppm", *ppm);    
  }
}
