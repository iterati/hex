/*
The MIT License (MIT)

Copyright (c) 2015 John Joseph Miller

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "elapsedMillis.h"
#include "anim.h"

#define DEBUG

#define NUM_MODES 16
#define NUM_BUNDLES 4
const uint8_t current_version = 152;

// MODE CONFIGURATION
// Mode mode# = Mode(MEMORY_ADDRESS, PRIME, NUM_COLORS, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12);
Mode mode0  = Mode(520, PRIME_STROBE,   12, 0x09, 0x0B, 0x0D, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F, 0x00, 0x00, 0x00, 0x00);
Mode mode1  = Mode(540, PRIME_HYPER,    12, 0x09, 0x0B, 0x0D, 0x00, 0x11, 0x13, 0x15, 0x00, 0x19, 0x1B, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00);
Mode mode2  = Mode(560, PRIME_DOPS,     12, 0x09, 0x0B, 0x0D, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F, 0x00, 0x00, 0x00, 0x00);
Mode mode3  = Mode(580, PRIME_STROBIE,   8, 0x08, 0x0B, 0x0E, 0x11, 0x14, 0x17, 0x1A, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
Mode mode4  = Mode(600, PRIME_PULSE,    12, 0x09, 0x0B, 0x0D, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F, 0x00, 0x00, 0x00, 0x00);
Mode mode5  = Mode(620, PRIME_SEIZURE,  12, 0x09, 0x11, 0x19, 0x0B, 0x13, 0x1B, 0x0D, 0x15, 0x1D, 0x0F, 0x17, 0x1F, 0x00, 0x00, 0x00, 0x00);
Mode mode6  = Mode(640, PRIME_TRACER,    9, 0xBD, 0x08, 0x0B, 0x4E, 0x11, 0x14, 0x17, 0x1A, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
Mode mode7  = Mode(660, PRIME_DASHDOPS,  9, 0x38, 0x08, 0x0B, 0x0E, 0x11, 0x14, 0x17, 0x1A, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
Mode mode8  = Mode(680, PRIME_BLINKE,    7, 0x38, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
Mode mode9  = Mode(700, PRIME_EDGE,      9, 0x08, 0x5F, 0x5E, 0x9D, 0x9C, 0xDB, 0xDA, 0xD9, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
Mode mode10 = Mode(720, PRIME_LEGO,      6, 0x09, 0x0D, 0x11, 0x15, 0x19, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
Mode mode11 = Mode(740, PRIME_CHASE,    12, 0x09, 0x13, 0x1D, 0x0F, 0x19, 0x0B, 0x15, 0x1F, 0x11, 0x1B, 0x0D, 0x17, 0x00, 0x00, 0x00, 0x00);
Mode mode12 = Mode(760, PRIME_MORPH,    12, 0x09, 0x0B, 0x0D, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F, 0x00, 0x00, 0x00, 0x00);
Mode mode13 = Mode(780, PRIME_RIBBON,   15, 0x20, 0x21, 0x22, 0x23, 0x00, 0x28, 0x29, 0x2A, 0x2B, 0x00, 0x30, 0x31, 0x32, 0x33, 0x00, 0x00);
Mode mode14 = Mode(800, PRIME_COMET,    12, 0x09, 0x0B, 0x0D, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F, 0x00, 0x00, 0x00, 0x00);
Mode mode15 = Mode(820, PRIME_CANDY,    12, 0x09, 0x0B, 0x0D, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F, 0x00, 0x00, 0x00, 0x00);

Mode *modes[NUM_MODES] = {
  &mode0,  &mode1,  &mode2,  &mode3,  &mode4,  &mode5,  &mode6,  &mode7,
  &mode8,  &mode9, &mode10, &mode11, &mode12, &mode13, &mode14, &mode15,
};
Mode *mode;

uint8_t bundles[NUM_BUNDLES][NUM_MODES] = {
  { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15},
  { 1,  2,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
  { 2,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
  {10, 11,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
};
uint8_t bundle_slots[NUM_BUNDLES] = {16, 3, 2, 2};
uint8_t cur_bundle = 0;
uint8_t bundle_idx = 0;

uint8_t edit_color;

elapsedMicros limiter = 0;

uint8_t button_state = 0;
uint8_t config_state = 0;
uint16_t since_press = 0;
bool transitioned = true;

bool conjure = false;
bool conjure_toggle = false;

bool bpm_enabled = false;
uint32_t bpm_tracker = 0;
uint32_t bpm_trigger = 32000;

const PROGMEM uint8_t gamma_table[256] = {
    0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,
    2,   2,   2,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   5,   5,
    5,   5,   6,   6,   6,   6,   7,   7,   7,   7,   8,   8,   8,   8,   9,   9,
    9,  10,  10,  10,  10,  11,  11,  11,  12,  12,  12,  13,  13,  14,  14,  14,
   15,  15,  15,  16,  16,  17,  17,  17,  18,  18,  19,  19,  20,  20,  20,  21,
   21,  22,  22,  23,  23,  24,  24,  25,  25,  26,  26,  27,  27,  28,  28,  29,
   30,  30,  31,  31,  32,  32,  33,  33,  34,  35,  35,  36,  37,  37,  38,  38,
   39,  40,  40,  41,  42,  42,  43,  44,  44,  45,  46,  46,  47,  48,  49,  49,
   50,  51,  51,  52,  53,  54,  54,  55,  56,  57,  58,  58,  59,  60,  61,  62,
   62,  63,  64,  65,  66,  67,  67,  68,  69,  70,  71,  72,  73,  74,  75,  75,
   76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,
   92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103, 104, 106, 107, 108,
  109, 110, 111, 112, 113, 114, 116, 117, 118, 119, 120, 121, 123, 124, 125, 126,
  127, 129, 130, 131, 132, 134, 135, 136, 137, 139, 140, 141, 142, 144, 145, 146,
  148, 149, 150, 152, 153, 154, 156, 157, 158, 160, 161, 163, 164, 165, 167, 168,
  170, 171, 172, 174, 175, 177, 178, 180, 181, 183, 184, 185, 187, 188, 190, 192,
};

// ********************************************************************
// **** SETUP CODE ****************************************************
// ********************************************************************
#define PIN_R 9
#define PIN_G 6
#define PIN_B 5
#define PIN_BUTTON 2
#define PIN_LDO A3
#define BUNDLE_EEPROM_ADDR 900
#define FRAME_TICKS 32000

void saveBundles() {
  for (uint8_t b = 0; b < NUM_BUNDLES; b++) {
    EEPROM.update(BUNDLE_EEPROM_ADDR + 64 + b, bundle_slots[b]);
    for (uint8_t i = 0; i < NUM_MODES; i++) {
      EEPROM.update(BUNDLE_EEPROM_ADDR + (b * NUM_MODES) + i, bundles[b][i]);
    }
  }
}

void loadBundles() {
  for (uint8_t b = 0; b < NUM_BUNDLES; b++) {
    bundle_slots[b] = EEPROM.read(BUNDLE_EEPROM_ADDR + 64 + b);
    for (uint8_t i = 0; i < NUM_MODES; i++) {
      bundles[b][i] = EEPROM.read(BUNDLE_EEPROM_ADDR + (b * NUM_MODES) + i);
    }
  }
}

void saveModes() {
  for (uint8_t i = 0; i < NUM_MODES; i++) modes[i]->save();
}

void loadModes() {
  for (uint8_t i = 0; i < NUM_MODES; i++) modes[i]->load();
}

void setup() {
#ifndef DEBUG
  power_usart0_disable();
#endif
  power_spi_disable();

  Serial.begin(57600);
  randomSeed(analogRead(0));

  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LDO, OUTPUT);
  digitalWrite(PIN_LDO, HIGH);

  Serial.println(F("\nWelcome to Hex!"));
  if (current_version != EEPROM.read(512)) {
    Serial.println(F("Version mismatch. Writing defaults to EEPROM."));
    saveModes();
    saveBundles();
    EEPROM.update(512, current_version);
  } else {
    Serial.println(F("Version match. Reading saved settings."));
    loadModes();
    loadBundles();
  }
  Serial.print(F("Hex v0.1. EEPROM Version: ")); Serial.println(current_version);

  for (uint8_t i = 0; i < NUM_MODES; i++) modes[i]->init();

  noInterrupts();
  ADCSRA = 0; // Disable ADC
  TCCR0B = (TCCR0B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  TCCR1B = (TCCR1B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  interrupts();

  delay(4000);
  limiter = 0;
  resetMode();
  transitioned = true;
}

void loop() {
  uint8_t r, g, b;

  handlePress(digitalRead(PIN_BUTTON) == LOW);

  if (button_state < 10) {
    mode->render(&r, &g, &b);
  } else if (button_state < 30) {
    unpackColor(mode->palette[edit_color], &r, &g, &b);
  } else {
    mode->render(&r, &g, &b);
  }

  if (conjure && conjure_toggle) {
    r = 0; g = 0; b = 0;
  }

  if (bpm_enabled && bpm_tracker > bpm_trigger) {
    incMode();
    Serial.print(F("bpm next mode: ")); printCurMode(); Serial.println();
  }

  writeFrame(r, g, b);
  bpm_tracker++;
}

void flash(uint8_t r, uint8_t g, uint8_t b, uint8_t flashes) {
  for (uint8_t i = 0; i < flashes; i++) {
    for (uint8_t j = 0; j < 200; j++) {
      if (j < 100) {
        writeFrame(r, g, b);
      } else {
        writeFrame(0, 0, 0);
      }
    }
  }
}

void writeFrame(uint8_t r, uint8_t g, uint8_t b) {
  while (limiter < FRAME_TICKS) {}
  limiter = 0;

  analogWrite(PIN_R, pgm_read_byte(&gamma_table[r]));
  analogWrite(PIN_G, pgm_read_byte(&gamma_table[g]));
  analogWrite(PIN_B, pgm_read_byte(&gamma_table[b]));
}

void resetMode() {
  bundle_idx = 0;
  mode = modes[bundles[cur_bundle][bundle_idx]];
  bpm_tracker = 0;
  mode->init();
}

void incMode() {
  bundle_idx = (bundle_idx + 1) % bundle_slots[cur_bundle];
  mode = modes[bundles[cur_bundle][bundle_idx]];
  bpm_tracker = 0;
  mode->init();
}


// ********************************************************************
// **** BUTTON CODE ***************************************************
// ********************************************************************
#define PRESS_DELAY 100   // 0.05s
#define SHORT_HOLD  2000  // 1.0s
#define LONG_HOLD   3000  // 1.5s
#define FLASH_TIME  1000  // 0.5s


#define S_PLAY_OFF                    0
#define S_PLAY_PRESSED                1
#define S_PLAY_SLEEP_WAIT             2
#define S_PLAY_CONJURE_WAIT           3
#define S_PLAY_CONFIG_PALETTE_WAIT    4
#define S_PLAY_CONFIG_PRIME_WAIT      5

#define S_COLOR_SELECT_OFF            20
#define S_COLOR_SELECT_PRESSED        21
#define S_COLOR_SELECT_PRESS_WAIT     22
#define S_COLOR_SELECT_SHADE_WAIT     23
#define S_COLOR_SELECT_RELEASE_WAIT   24

#define S_COLOR_CONFIRM_OFF           30
#define S_COLOR_CONFIRM_PRESSED       31
#define S_COLOR_CONFIRM_REJECT_WAIT   32
#define S_COLOR_CONFIRM_EXIT_WAIT     33

#define S_PRIME_SELECT_OFF            40
#define S_PRIME_SELECT_PRESSED        41
#define S_PRIME_SELECT_WAIT           42

#define S_BUNDLE_SELECT_START         70
#define S_BUNDLE_SELECT_OFF           71
#define S_BUNDLE_SELECT_PRESSED       72
#define S_BUNDLE_SELECT_WAIT          73
#define S_BUNDLE_SELECT_EDIT          74
#define S_BUNDLE_SELECT_BPM           75

#define S_BUNDLE_EDIT_OFF             80
#define S_BUNDLE_EDIT_PRESSED         81
#define S_BUNDLE_EDIT_WAIT            82
#define S_BUNDLE_EDIT_SAVE            83

void handlePress(bool pressed) {
  switch (button_state) {
    //******************************************************
    //** PLAY **********************************************
    //******************************************************
    case S_PLAY_OFF:
      if (transitioned) {
        if (bpm_enabled) { Serial.print(F("playing bpm: ")); }
        else {             Serial.print(F("playing: ")); }
        printCurMode(); Serial.println();
        transitioned = false;
      }
      if (pressed && since_press > PRESS_DELAY) {
        since_press = 0;
        button_state = S_PLAY_PRESSED;
      }
      break;

    case S_PLAY_PRESSED:
      if (since_press > SHORT_HOLD) {
        Serial.print(F("will sleep... "));
        flash(128, 128, 128, 5); since_press = FLASH_TIME;
        button_state = S_PLAY_SLEEP_WAIT;
      } else if (!pressed) {
        if (conjure) {
          conjure_toggle = !conjure_toggle;
          if (conjure_toggle) { Serial.println(F("conjure: light off")); }
          else {                Serial.println(F("conjure: light on")); }
        } else {
          incMode();
          transitioned = true;
        }
        since_press = 0;
        button_state = S_PLAY_OFF;
      }
      break;

    case S_PLAY_SLEEP_WAIT:
      if (since_press > LONG_HOLD) {
        Serial.print(F("toggle conjure... "));
        flash(0, 0, 128, 5); since_press = FLASH_TIME;
        button_state = S_PLAY_CONJURE_WAIT;
      } else if (!pressed) {
        Serial.println(F("sleeping"));
        enterSleep();
      }
      break;

    case S_PLAY_CONJURE_WAIT:
      if (since_press > LONG_HOLD) {
        Serial.print(F("config palette... "));
        flash(128, 128, 0, 5); since_press = FLASH_TIME;
        button_state = S_PLAY_CONFIG_PALETTE_WAIT;
      } else if (!pressed) {
        conjure = !conjure;
        conjure_toggle = false;
        if (conjure) { Serial.println(F("conjure on")); }
        else {         Serial.println(F("conjure off")); }
        since_press = 0;
        button_state = S_PLAY_OFF;
      }
      break;

    case S_PLAY_CONFIG_PALETTE_WAIT:
      if (since_press > LONG_HOLD) {
        Serial.print(F("config prime... "));
        flash(0, 128, 0, 5); since_press = FLASH_TIME;
        button_state = S_PLAY_CONFIG_PRIME_WAIT;
      } else if (!pressed) {
        mode->init();
        Serial.print(F("config palette mode: ")); Serial.println(bundles[cur_bundle][bundle_idx] + 1);
        edit_color = 0;
        since_press = 0;
        transitioned = true;
        button_state = S_COLOR_SELECT_OFF;
      }
      break;

    case S_PLAY_CONFIG_PRIME_WAIT:
      if (!pressed) {
        mode->init();
        Serial.print(F("config prime mode: ")); Serial.println(bundles[cur_bundle][bundle_idx] + 1);
        since_press = 0;
        transitioned = true;
        button_state = S_PRIME_SELECT_OFF;
      }
      break;

    //******************************************************
    //** COLOR SELECT **************************************
    //******************************************************
    case S_COLOR_SELECT_OFF:
      if (transitioned) {
        Serial.print(F("edit: ")); printPaletteSlot(); Serial.println();
        transitioned = false;
      }
      if (pressed && since_press > PRESS_DELAY) {
        since_press = 0;
        button_state = S_COLOR_SELECT_PRESSED;
      }
      break;

    case S_COLOR_SELECT_PRESSED:
      if (!pressed) {
        since_press = 0;
        button_state = S_COLOR_SELECT_PRESS_WAIT;
      } else if (since_press > SHORT_HOLD) {
        Serial.print(F("select: ")); printPaletteSlot(); Serial.println();
        flash(64, 64, 64, 5); since_press = FLASH_TIME;
        button_state = S_COLOR_SELECT_SHADE_WAIT;
      }
      break;

    case S_COLOR_SELECT_PRESS_WAIT:
      if (since_press > 700) {
        mode->palette[edit_color] = (mode->palette[edit_color] + 1) % 63;
        Serial.print(F("next color: ")); printPaletteSlot(); Serial.println();
        since_press = 0;
        button_state = S_COLOR_SELECT_OFF;
      } else if (pressed) {
        mode->palette[edit_color] = (mode->palette[edit_color] + 62) % 63;
        Serial.print(F("prev color: ")); printPaletteSlot(); Serial.println();
        button_state = S_COLOR_SELECT_RELEASE_WAIT;
      }
      break;

    case S_COLOR_SELECT_SHADE_WAIT:
      if (since_press > SHORT_HOLD) {
        mode->palette[edit_color] += 0b01000000;
        Serial.print(F("select: ")); printPaletteSlot(); Serial.println();
        flash(64, 64, 64, 5); since_press = FLASH_TIME;
      } else if (!pressed) {
        mode->num_colors = edit_color + 1;
        Serial.print(F("selected: ")); printPaletteSlot(); Serial.println();
        since_press = 0;
        transitioned = true;
        button_state = S_COLOR_CONFIRM_OFF;
      }
      break;

    case S_COLOR_SELECT_RELEASE_WAIT:
      if (!pressed) {
        since_press = 0;
        button_state = S_COLOR_SELECT_OFF;
      }
      break;

    //******************************************************
    //** COLOR CONFIRM *************************************
    //******************************************************
    case S_COLOR_CONFIRM_OFF:
      if (transitioned) {
        Serial.print(F("confirm: ")); printPaletteSlot(); Serial.println();
        transitioned = false;
      }
      if (pressed && since_press > PRESS_DELAY) {
        since_press = 0;
        button_state = S_COLOR_CONFIRM_PRESSED;
      }
      break;

    case S_COLOR_CONFIRM_PRESSED:
      if (since_press > LONG_HOLD) {
        Serial.print(F("will reject... "));
        flash(128, 0, 0, 5); since_press = FLASH_TIME;
        button_state = S_COLOR_CONFIRM_REJECT_WAIT;
      } else if (!pressed) {
        edit_color++;
        since_press = 0;
        Serial.print(F("confirmed: ")); printPaletteSlot(); Serial.println();
        if (edit_color == PALETTE_SIZE) {
          mode->save();
          mode->init();
          Serial.print(F("saving: ")); Serial.print(mode->num_colors); Serial.println(F(" slots"));
          flash(128, 128, 128, 5);
          button_state = S_PLAY_OFF;
        } else {
          button_state = S_COLOR_SELECT_OFF;
        }
      }
      break;

    case S_COLOR_CONFIRM_REJECT_WAIT:
      if (since_press > LONG_HOLD) {
        Serial.print(F("will save... "));
        flash(128, 128, 128, 5);
        button_state = S_COLOR_CONFIRM_EXIT_WAIT;
      } else if (!pressed) {
        since_press = 0;
        Serial.println(F("reject"));
        if (edit_color == 0) {
          transitioned = true;
          button_state = S_COLOR_SELECT_OFF;
        } else {
          edit_color--;
          mode->num_colors--;
          transitioned = true;
          button_state = S_COLOR_CONFIRM_OFF;
        }
        printPaletteSlot(); Serial.println();
      }
      break;

    case S_COLOR_CONFIRM_EXIT_WAIT:
      if (!pressed) {
        Serial.print(F("confirmed: ")); printPaletteSlot(); Serial.println();
        mode->num_colors = edit_color + 1;
        mode->save();
        mode->init();
        Serial.print(F("saving: ")); Serial.print(mode->num_colors); Serial.println(F(" slots"));
        since_press = 0;
        transitioned = true;
        button_state = S_PLAY_OFF;
      }
      break;


    //******************************************************
    //** PRIME SELECT **************************************
    //******************************************************
    case S_PRIME_SELECT_OFF:
      if (transitioned) {
        Serial.print(F("editing: ")); printPrime(); Serial.println();
        transitioned = false;
      }
      if (pressed && since_press > PRESS_DELAY) {
        since_press = 0;
        button_state = S_PRIME_SELECT_PRESSED;
      }
      break;

    case S_PRIME_SELECT_PRESSED:
      if (since_press >= LONG_HOLD) {
        Serial.print(F("will save.. "));
        flash(128, 128, 128, 5);
        button_state = S_PRIME_SELECT_WAIT;
      } else if (!pressed) {
        mode->prime = (mode->prime + 1) % NUM_PRIMES;
        Serial.print(F("switched: ")); printPrime(); Serial.println();
        mode->init();
        since_press = 0;
        button_state = S_PRIME_SELECT_OFF;
      }
      break;

    case S_PRIME_SELECT_WAIT:
      if (!pressed) {
        mode->save();
        mode->init();
        Serial.print(F("saving: ")); printPrime(); Serial.println();
        since_press = 0;
        transitioned = true;
        button_state = S_PLAY_OFF;
      }
      break;


    //******************************************************
    //** BUNDLE SELECT *************************************
    //******************************************************
    case S_BUNDLE_SELECT_START:
      if (!pressed) {
        bpm_enabled = false;
        since_press = 0;
        button_state = S_BUNDLE_SELECT_OFF;
      }
      break;

    case S_BUNDLE_SELECT_OFF:
      if (pressed && since_press > PRESS_DELAY) {
        since_press = 0;
        button_state = S_BUNDLE_SELECT_PRESSED;
      }
      break;

    case S_BUNDLE_SELECT_PRESSED:
      if (since_press >= LONG_HOLD) {
        Serial.print(F("will select bundle... "));
        flash(0, 0, 128, 5); since_press = FLASH_TIME;
        button_state = S_BUNDLE_SELECT_WAIT;
      } else if (!pressed) {
        cur_bundle = (cur_bundle + 1) % NUM_BUNDLES;
        Serial.print(F("switched: bundle ")); Serial.println(cur_bundle + 1);
        resetMode();
        since_press = 0;
        button_state = S_BUNDLE_SELECT_OFF;
      }
      break;

    case S_BUNDLE_SELECT_WAIT:
      if (since_press >= LONG_HOLD) {
        Serial.print(F("will edit bundle... "));
        flash(128, 128, 0, 5); since_press = FLASH_TIME;
        button_state = S_BUNDLE_SELECT_EDIT;
      } else if (!pressed) {
        Serial.print(F("select: bundle ")); Serial.println(cur_bundle + 1);
        since_press = 0;
        transitioned = true;
        button_state = S_PLAY_OFF;
      }
      break;

    case S_BUNDLE_SELECT_EDIT:
      if (since_press >= LONG_HOLD) {
        Serial.print(F("will toggle bpm switching... "));
        flash(128, 0, 0, 1); flash(0, 128, 0, 1); flash(0, 0, 128, 1); flash(0, 128, 0, 1); flash(0, 0, 128, 1);
        button_state = S_BUNDLE_SELECT_BPM;
      } else if (!pressed) {
        resetMode();
        Serial.println(F("edit"));
        since_press = 0;
        transitioned = true;
        button_state = S_BUNDLE_EDIT_OFF;
      }
      break;

    case S_BUNDLE_SELECT_BPM:
      if (!pressed) {
        bpm_enabled = true;
        if (bpm_enabled) { Serial.println(F("bpm switching enabled")); }
        else {             Serial.println(F("bpm switching disabled")); }
        bpm_tracker = 0;
        since_press = 0;
        transitioned = true;
        button_state = S_PLAY_OFF;
      }
      break;


    //******************************************************
    //** BUNDLE EDIT ***************************************
    //******************************************************
    case S_BUNDLE_EDIT_OFF:
      if (transitioned) {
        Serial.print(F("editing: ")); printCurMode(); Serial.println();
        transitioned = false;
      }
      if (pressed && since_press > PRESS_DELAY) {
        since_press = 0;
        button_state = S_BUNDLE_EDIT_PRESSED;
      }
      break;

    case S_BUNDLE_EDIT_PRESSED:
      if (since_press >= LONG_HOLD) {
        Serial.print(F("will set... "));
        flash(128, 0, 128, 5); since_press = FLASH_TIME;
        button_state = S_BUNDLE_EDIT_WAIT;
      } else if (!pressed) {
        bundles[cur_bundle][bundle_idx] = (bundles[cur_bundle][bundle_idx] + 1) % NUM_MODES;
        mode = modes[bundles[cur_bundle][bundle_idx]];
        mode->init();
        Serial.print(F("switched: ")); printCurMode(); Serial.println();
        since_press = 0;
        button_state = S_BUNDLE_EDIT_OFF;
      }
      break;

    case S_BUNDLE_EDIT_WAIT:
      if (since_press >= LONG_HOLD) {
        Serial.print(F("will save... "));
        flash(128, 128, 128, 5);
        button_state = S_BUNDLE_EDIT_SAVE;
      } else if (!pressed) {
        bundle_idx++;
        since_press = 0;
        Serial.print(F("set: ")); printCurMode(); Serial.println();
        if (bundle_idx == NUM_MODES) {
          bundle_slots[cur_bundle] = bundle_idx;
          saveBundles();
          resetMode();
          Serial.print(F("saved: ")); printCurBundle(); Serial.println();
          button_state = S_PLAY_OFF;
        } else {
          mode = modes[bundles[cur_bundle][bundle_idx]];
          mode->init();
          transitioned = true;
          button_state = S_BUNDLE_EDIT_OFF;
        }
      }
      break;

    case S_BUNDLE_EDIT_SAVE:
      if (!pressed) {
        bundle_slots[cur_bundle] = bundle_idx + 1;
        saveBundles();
        resetMode();
        Serial.print(F("saved: ")); printCurBundle(); Serial.println();
        since_press = 0;
        transitioned = true;
        button_state = S_PLAY_OFF;
      }
      break;


    default:
      since_press = 0;
      transitioned = true;
      button_state = S_PLAY_OFF;
      break;
  }
  since_press++;
}

void printCurBundle() {
  Serial.print(F("bundle ")); Serial.print(cur_bundle + 1);
  Serial.print(F(" slots-")); Serial.print(bundle_slots[cur_bundle]);
}

void printCurMode() {
  Serial.print(F("bundle ")); Serial.print(cur_bundle + 1);
  Serial.print(F(" slot ")); Serial.print(bundle_idx + 1);
  Serial.print(F(": ")); printPrime();
}

void printPaletteSlot() {
  Serial.print(F("mode ")); Serial.print(bundles[cur_bundle][bundle_idx] + 1);
  Serial.print(F(" slot ")); Serial.print(edit_color + 1);
  Serial.print(F(": i-")); Serial.print(mode->palette[edit_color] & 0b00111111, HEX);
  Serial.print(F(" s-")); Serial.print(mode->palette[edit_color] >> 6);
}

void printPrime() {
  Serial.print(F("mode ")); Serial.print(bundles[cur_bundle][bundle_idx] + 1);
  switch (mode->prime) {
    case PRIME_STROBE:
      Serial.print(F(" STROBE"));
      break;
    case PRIME_HYPER:
      Serial.print(F(" HYPER STROBE"));
      break;
    case PRIME_DOPS:
      Serial.print(F(" DOPS"));
      break;
    case PRIME_STROBIE:
      Serial.print(F(" STROBIE"));
      break;
    case PRIME_PULSE:
      Serial.print(F(" PULSE"));
      break;
    case PRIME_SEIZURE:
      Serial.print(F(" SEIZURE STROBE"));
      break;
    case PRIME_TRACER:
      Serial.print(F(" TRACER"));
      break;
    case PRIME_DASHDOPS:
      Serial.print(F(" DASH DOPS"));
      break;
    case PRIME_BLINKE:
      Serial.print(F(" BLINK-E"));
      break;
    case PRIME_EDGE:
      Serial.print(F(" EDGE"));
      break;
    case PRIME_LEGO:
      Serial.print(F(" LEGO"));
      break;
    case PRIME_CHASE:
      Serial.print(F(" CHASE"));
      break;
    case PRIME_MORPH:
      Serial.print(F(" MORPH STROBE"));
      break;
    case PRIME_RIBBON:
      Serial.print(F(" RIBBON"));
      break;
    case PRIME_COMET:
      Serial.print(F(" COMET STROBE"));
      break;
    default: // case PRIME_CANDY:
      Serial.print(F(" CANDY STROBE"));
      break;
  }
}


// ********************************************************************
// **** SLEEP CODE ****************************************************
// ********************************************************************
void enterSleep() {
  Serial.println(F("Going to sleep"));

  writeFrame(0, 0, 0);
  digitalWrite(PIN_LDO, LOW);
  delay(4000);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  noInterrupts();
  attachInterrupt(0, pushInterrupt, FALLING);
  EIFR = bit(INTF0);
  MCUCR = bit(BODS) | bit(BODSE);
  MCUCR = bit(BODS);
  interrupts ();
  sleep_cpu ();

  // Wait until button is releaed
  uint16_t held_count = 0;
  limiter = 0;
  while (digitalRead(PIN_BUTTON) == LOW) {
    if (limiter > FRAME_TICKS) {
      limiter = 0;
      held_count++;
    }
    if (held_count > LONG_HOLD) {
      break;
    }
  }

  if (held_count > LONG_HOLD) {
    Serial.println(F("select bundle"));
    flash(0, 0, 128, 5);
    button_state = S_BUNDLE_SELECT_START;
  } else {
    button_state = S_PLAY_OFF;
  }

  digitalWrite(PIN_LDO, HIGH);
  resetMode();
  transitioned = true;
  conjure = conjure_toggle = false;
  delay(4000);
}

void pushInterrupt() {
  sleep_disable();
  detachInterrupt(0);
}
