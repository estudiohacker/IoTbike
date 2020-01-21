#pragma once

#include <Arduino.h>
#include "src/PubSubClient/PubSubClient.h"

class Conn {
  public:
    Conn(String stationID);
    void loop();
    void notify_sensor(String sensor, float value);
    void notify_topic(String topic, String payload, bool retained = false);
    void listen_alarm();
  private:
    String _stationID;
    PubSubClient* _PubSubClient;
    void connect();
    String fullTopic(String topic);
    String alarmTopic();
};
