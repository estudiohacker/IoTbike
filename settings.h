#pragma once

#include <Arduino.h>

static const uint8_t SENSOR_DS18B20_PIN = D5;
static const uint8_t SENSOR_DS18B20_PRECISION = 12;
static const unsigned long SENSOR_DS18B20_NOTIFY_INTERVAL = 5000;

static const uint8_t SENSOR_MQ135_PIN = A0;
static const unsigned long SENSOR_MQ135_NOTIFY_INTERVAL = 5000;
