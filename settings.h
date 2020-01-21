#pragma once

#include <Arduino.h>

#define DEBUG

static const char* STATION_PREFIX = "IoTBike";

static const char* WIFI_SSID = "WIFI_SSID";
static const char* WIFI_PASSWORD = "WIFI_PASSWORD";
static const char* WIFI_AP_PASSWORD = "";
static const uint8_t WIFI_CONNECTION_RETRIES = 20;

static const unsigned long MQTT_CONNECTION_RETRY = 60000;

static const char* MQTT_SERVER = "MQTT_SERVER";
static const uint16_t MQTT_PORT = 1883;
static const char* MQTT_USER = "";
static const char* MQTT_PASSWORD = "";
static const char* MQTT_TOPIC_PREFIX = "";

static const uint8_t SENSOR_DS18B20_PIN = D5;
static const uint8_t SENSOR_DS18B20_PRECISION = 12;
static const unsigned long SENSOR_DS18B20_NOTIFY_INTERVAL = 5000;

static const uint8_t SENSOR_MQ135_PIN = A0;
static const unsigned long SENSOR_MQ135_NOTIFY_INTERVAL = 5000;
