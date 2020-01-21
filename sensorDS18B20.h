#pragma once

#include <Arduino.h>
#include "conn.h"

void setupSensorDS18B20();

void loopSensorDS18B20(Conn* conn, float *temp);
