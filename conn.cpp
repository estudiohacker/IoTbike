#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "settings.h"
#include "debug.h"
#include "conn.h"
#include "version.h"

WiFiClient wifiClient;

extern String uptime;

long lastMQTTReconnectAttempt = -MQTT_CONNECTION_RETRY;

void setupWiFi(const char* _stationID) {
  debugMsg("Starting WiFi Setup...\n");
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_AP_STA);
  WiFi.hostname(_stationID);
  WiFi.disconnect();

  WiFi.softAP(_stationID, WIFI_AP_PASSWORD);
  debugMsg("Access Point %s started at IP address %s\n", _stationID, WiFi.softAPIP().toString().c_str());  

  debugMsg("Connecting %s to SSID %s", _stationID, WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED) and (retries < WIFI_CONNECTION_RETRIES)) {
    delay(500);
    retries++;
    debugMsg(".");
  }
  debugMsg(" Ok.\n");

  debugMsg("IP address set to %s\n", WiFi.localIP().toString().c_str());

  debugMsg("Starting mDNS... ", _stationID);
  if (MDNS.begin(_stationID)) {
    debugMsg(" Ok.\n");    
  } else {
    debugMsg(" Error!.\n");    
  }
}

void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  char c_payload[length];
  memcpy(c_payload, payload, length);
  c_payload[length] = '\0';

  debugMsg("<- %s: %s\n", topic, c_payload);
}

Conn::Conn(String stationID) {
  this->_stationID = stationID;

  setupWiFi(this->_stationID.c_str());
  
  this->_PubSubClient = new PubSubClient(wifiClient);
  this->_PubSubClient->setServer(MQTT_SERVER, MQTT_PORT);
  this->_PubSubClient->setCallback(callbackMQTT);
}

void Conn::connect() {
  if (!this->_PubSubClient->connected()) {
      if (millis() - lastMQTTReconnectAttempt > MQTT_CONNECTION_RETRY) {
        lastMQTTReconnectAttempt = millis();
        debugMsg("Attempting MQTT connection to %s...", MQTT_SERVER);
        if (this->_PubSubClient->connect(this->_stationID.c_str(), MQTT_USER, MQTT_PASSWORD)) {
          debugMsg(" Ok.\n");
    
          // Once connected, publish announcements...
          this->notify_topic("version", version, true);
  
          // ... and resubscribe
          listen_alarm();
        } else {
          debugMsg(" ERROR: %d. Trying again later.\n", this->_PubSubClient->state());
        }
      }
  }
}

void Conn::loop() {
  this->connect();
  this->_PubSubClient->loop();
}

String Conn::fullTopic(String topic) {
  return String(MQTT_TOPIC_PREFIX) + "station/" + this->_stationID + "/" + topic;
}

String Conn::alarmTopic() {
  return String(MQTT_TOPIC_PREFIX) + "station/alarm";
}

void Conn::notify_topic(String topic, String payload, bool retained) {
  debugMsg("-> %s: %s\n", fullTopic(topic).c_str(), payload.c_str());
  this->_PubSubClient->publish(fullTopic(topic).c_str(), payload.c_str(), retained);
}

void Conn::notify_sensor(String sensor, float value) {
  static char payload[15];
  dtostrf(value, 15, 6, payload);
  this->notify_topic("sensor/" + sensor, String(payload), false);
}

void Conn::listen_alarm() {
  debugMsg("++ %s\n", fullTopic("alarm").c_str());
  this->_PubSubClient->subscribe(fullTopic("alarm").c_str(), 1);

  debugMsg("++ %s\n", alarmTopic().c_str());
  this->_PubSubClient->subscribe(alarmTopic().c_str(), 1);
}
