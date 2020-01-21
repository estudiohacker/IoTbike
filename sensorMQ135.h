#pragma once

#include <Arduino.h>
#include "conn.h"

void setupSensorMQ135();

void loopSensorMQ135(Conn* conn, float *ppm);
