#pragma once

#include <Arduino.h>

#include "src/U8g2/U8g2lib.h"

//wave10px logo
static const unsigned char u8g2_logo_wave[] U8X8_PROGMEM = {
  0xE0, 0x03, 0x18, 0x00, 0xC4, 0x01, 0x32, 0x00, 0x8A, 0x01, 0x69, 0x00,
  0x25, 0x00, 0x95, 0x01, 0x95, 0x01, 0x01, 0x00,
};

void setupDisplay();

void loopDisplay();

void print_page1();

void print_Clock();

void print_speed();

void print_location();

void print_temperatura();

void print_ar();

static void printDate();

static void printTime();

void print_Trip();
