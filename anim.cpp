#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "anim.h"

uint8_t _r, _g, _b;
uint8_t r0, g0, b0;
uint8_t r1, g1, b1;


const PROGMEM uint8_t color_palette[64][3] = {
  // Blank and whites
  {  0,   0,   0},  // 0x00
  {144, 144, 144},  // 0x01
  {192, 140, 140},  // 0x02
  {184, 184, 148},  // 0x03
  {140, 192, 140},  // 0x04
  {148, 184, 184},  // 0x05
  {140, 140, 192},  // 0x06
  {184, 148, 184},  // 0x07

  // Red - green
  {255,   0,   0},  // 0x08
  {252,  63,   0},  // 0x09
  {248, 127,   0},  // 0x0A
  {244, 191,   0},  // 0x0B
  {240, 240,   0},  // 0x0C
  {191, 244,   0},  // 0x0D
  {127, 248,   0},  // 0x0E
  { 63, 252,   0},  // 0x0F

  // Green - blue
  {  0, 255,   0},  // 0x10
  {  0, 252,  63},  // 0x11
  {  0, 248, 127},  // 0x12
  {  0, 244, 191},  // 0x13
  {  0, 240, 240},  // 0x14
  {  0, 191, 244},  // 0x15
  {  0, 127, 248},  // 0x16
  {  0,  63, 252},  // 0x17

  // Blue - red
  {  0,   0, 255},  // 0x18
  { 63,   0, 252},  // 0x19
  {127,   0, 248},  // 0x1A
  {191,   0, 244},  // 0x1B
  {240,   0, 240},  // 0x1C
  {244,   0, 191},  // 0x1D
  {248,   0, 127},  // 0x1E
  {252,   0,  63},  // 0x1F

  // Red - green saturated
  {224,  64,  64},  // 0x20
  {216,  96,  64},  // 0x21
  {208, 128,  64},  // 0x22
  {200, 160,  64},  // 0x23
  {192, 192,  64},  // 0x24
  {160, 200,  64},  // 0x25
  {128, 208,  64},  // 0x26
  {104, 216,  64},  // 0x27

  // Green - blue saturated
  { 64, 224,  64},  // 0x28
  { 64, 216,  96},  // 0x29
  { 64, 208, 128},  // 0x2A
  { 64, 200, 160},  // 0x2B
  { 64, 192, 192},  // 0x2C
  { 64, 160, 200},  // 0x2D
  { 64, 128, 208},  // 0x2E
  { 64, 104, 216},  // 0x2F

  // Blue - red saturated
  { 64,  64, 224},  // 0x30
  { 96,  64, 216},  // 0x31
  {128,  64, 208},  // 0x32
  {160,  64, 200},  // 0x33
  {192,  64, 192},  // 0x34
  {200,  64, 160},  // 0x35
  {208,  64, 128},  // 0x36
  {216,  64, 104},  // 0x37

  // Dim colors
  { 32,  32,  32},  // 0x38
  { 48,   0,   0},  // 0x39
  { 40,  40,   0},  // 0x3A
  {  0,  48,   0},  // 0x3B
  {  0,  40,  40},  // 0x3C
  {  0,   0,  48},  // 0x3D
  { 40,   0,  40},  // 0x3E
};

void unpackColor(uint8_t color, uint8_t *r, uint8_t *g, uint8_t *b) {
  uint8_t shade = color >> 6;                                   // shade is first 2 bits
  uint8_t idx = color & 0b00111111;                             // palette index is last 6 bits
  *r = pgm_read_byte(&color_palette[idx][0]); *r = *r >> shade; // get red value and shade
  *g = pgm_read_byte(&color_palette[idx][1]); *g = *g >> shade; // get green value and shade
  *b = pgm_read_byte(&color_palette[idx][2]); *b = *b >> shade; // get blue value and shade
}

void morphColor(uint16_t tick, uint16_t morph_time,
    uint8_t r0, uint8_t g0, uint8_t b0,
    uint8_t r1, uint8_t g1, uint8_t b1,
    uint8_t *r, uint8_t *g, uint8_t *b)
{
  *r = r0 + (int)(r1 - r0) * (tick / (float)morph_time);
  *g = g0 + (int)(g1 - g0) * (tick / (float)morph_time);
  *b = b0 + (int)(b1 - b0) * (tick / (float)morph_time);
}

uint8_t getLegoTime() {
  switch (random(0, 3)) {
    case 0:
      return 4; // 2;
    case 1:
      return 16; // 8;
    default:
      return 32; // 16;
  }
}


Mode::Mode(uint16_t user_eeprom_addr, uint8_t user_prime, uint8_t user_num_colors,
    uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5,
    uint8_t c6, uint8_t c7, uint8_t c8, uint8_t c9, uint8_t c10, uint8_t c11,
    uint8_t c12, uint8_t c13, uint8_t c14, uint8_t c15)
{
  eeprom_addr = user_eeprom_addr;
  prime = user_prime;
  num_colors = user_num_colors;
  palette[0] = c0;
  palette[1] = c1;
  palette[2] = c2;
  palette[3] = c3;
  palette[4] = c4;
  palette[5] = c5;
  palette[6] = c6;
  palette[7] = c7;
  palette[8] = c8;
  palette[9] = c9;
  palette[10] = c10;
  palette[11] = c11;
  palette[12] = c12;
  palette[13] = c13;
  palette[14] = c14;
  palette[15] = c15;
}

void Mode::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  switch (prime) {
    case PRIME_STROBE:
      if (tick >= 10 + 16) { // 5 + 8) {
        tick = 0;
        cur_color = (cur_color + 1) % num_colors;
      }

      if (tick < 10) { // < 5) {
        unpackColor(palette[cur_color], &_r, &_g, &_b);
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    case PRIME_HYPER:
      if (tick >= 34 + 34) { // 17 + 17) {
        tick = 0;
        cur_color = (cur_color + 1) % num_colors;
      }

      if (tick < 34) { //  17) {
        unpackColor(palette[cur_color], &_r, &_g, &_b);
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    case PRIME_DOPS:
      if (tick >= 3 + 20) { // 1 + 10) {
        tick = 0;
        cur_color = (cur_color + 1) % num_colors;
      }

      if (tick < 3) { // 1) {
        unpackColor(palette[cur_color], &_r, &_g, &_b);
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    case PRIME_STROBIE:
      if (tick >= 6 + 46) { // 3 + 23) {
        tick = 0;
        cur_color = (cur_color + 1) % num_colors;
      }

      if (tick < 6) { // 3) {
        unpackColor(palette[cur_color], &_r, &_g, &_b);
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    case PRIME_PULSE:
      if (tick >= 200 + 50) { // 100 + 25) {
        tick = 0;
        cur_color = (cur_color + 1) % num_colors;
      }

      if (tick < 100) { // 50) {
        unpackColor(palette[cur_color], &r0, &g0, &b0);
        // morphColor(tick, 50, 0, 0, 0, r0, g0, b0, &_r, &_g, &_b);
        morphColor(tick, 100, 0, 0, 0, r0, g0, b0, &_r, &_g, &_b);
      } else if (tick < 200) { // 100) {
        unpackColor(palette[cur_color], &r0, &g0, &b0);
        //morphColor(tick - 50, 50, r0, g0, b0, 0, 0, 0, &_r, &_g, &_b);
        morphColor(tick - 100, 100, r0, g0, b0, 0, 0, 0, &_r, &_g, &_b);
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    case PRIME_SEIZURE:
      if (tick >= 10 + 190) { // 5 + 95) {
        tick = 0;
        cur_color = (cur_color + 1) % num_colors;
      }

      if (tick < 10) { // 5) {
        unpackColor(palette[cur_color], &_r, &_g, &_b);
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    case PRIME_TRACER:
      if (tick >= 6 + 46) { // 3 + 23) {
        tick = 0;
        cur_color = (cur_color + 1) % (num_colors - 1);
      }

      if (tick < 6) { // 3) {
        unpackColor(palette[(cur_color + 1) % num_colors], &_r, &_g, &_b);
      } else {
        unpackColor(palette[0], &_r, &_g, &_b);
      }
      break;

    case PRIME_DASHDOPS:
      counter1 = num_colors - 1;
      // dash + 7 dops 7 extra blank dop
      //if (tick >= (counter0 * 11) + ((1 + 10) * 7) + 10) {
      if (tick >= (counter1 * 22) + (23 * 7) + 20) {
        tick = 0;
      }

      //if (tick < counter1 * 11) {
      if (tick < counter1 * 22) {
        //counter0 = (tick / 11) + 1;
        unpackColor(palette[(tick / 22) + 1], &_r, &_g, &_b);
      } else {
        //counter0 = tick - (counter1 * 11);
        counter0 = tick - (counter1 * 22);
        //if (counter0 % 11 == 10) {
        if (counter0 % 23 > 19) {
          unpackColor(palette[0], &_r, &_g, &_b);
        } else {
          _r = 0; _g = 0; _b = 0;
        }
      }
      break;

    case PRIME_BLINKE:
      if (tick >= (num_colors * 10) + 100) { // 5) + 50) {
        tick = 0;
      }

      if (tick < num_colors * 10) { // 5)) {
        //unpackColor(palette[tick / 5], &_r, &_g, &_b);
        unpackColor(palette[tick / 10], &_r, &_g, &_b);
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    case PRIME_EDGE:
      if (counter0 == 0) counter0 = num_colors - 1;
      //if (tick >= (counter0 * 4) + 5 + 20) {
      if (tick >= (counter0 * 8) + 10 + 40) {
        tick = 0;
      }

      if (tick < counter0 * 4) { // 2) {
        //unpackColor(palette[counter0 - (tick / 2)], &_r, &_g, &_b);
        unpackColor(palette[counter0 - (tick / 4)], &_r, &_g, &_b);
      } else if (tick < (counter0 * 4) + 10) { // 2) + 5) {
        unpackColor(palette[0], &_r, &_g, &_b);
      } else if (tick < (counter0 * 8) + 10) { // 4) + 5) {
        unpackColor(palette[((tick - ((counter0 * 4) + 10)) / 4) + 1], &_r, &_g, &_b);
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    case PRIME_LEGO:
      if (counter0 == 0) counter0 = getLegoTime();
      if (tick >= counter0 + 16) { // 8) {
        tick = 0;
        cur_color = (cur_color + 1) % num_colors;
        counter0 = getLegoTime();
      }

      if (tick < counter0) {
        unpackColor(palette[cur_color], &_r, &_g, &_b);
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    case PRIME_CHASE:
      if (tick >= 120) { // 60) {
        tick = 0;
        counter0++;
        if (counter0 >= 4) {
          counter0 = 0;
          cur_color = (cur_color + 1) % num_colors;
        }
      }

      if (tick < 100) { // 50) {
        counter1 = tick / 20; // 10;
        if (counter0 == 0) {
          unpackColor(palette[cur_color], &_r, &_g, &_b);
        } else {
          if (counter1 < counter0) {
            unpackColor(palette[(cur_color + 1) % num_colors], &_r, &_g, &_b);
          } else if (counter1 == counter0) {
            _r = 0; _g = 0; _b = 0;
          } else {
            unpackColor(palette[cur_color], &_r, &_g, &_b);
          }
        }
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    case PRIME_MORPH:
      if (tick >= 34 + 34) { // 17 + 17) {
        tick = 0;
        counter0++;
        if (counter0 >= 4) {
          counter0 = 0;
          cur_color = (cur_color + 1) % num_colors;
        }
      }

      if (tick < 34) { // 17) {
        unpackColor(palette[cur_color],                    &r0, &g0, &b0);
        unpackColor(palette[(cur_color + 1) % num_colors], &r1, &g1, &b1);
        //morphColor(tick + (34 * counter0), 34 * 4, r0, g0, b0, r1, g1, b1, &_r, &_g, &_b);
        morphColor(tick + (68 * counter0), 68 * 8, r0, g0, b0, r1, g1, b1, &_r, &_g, &_b);
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    case PRIME_RIBBON:
      if (tick >= 22) { // 11) {
        tick = 0;
        cur_color = (cur_color + 1) % num_colors;
      }
      unpackColor(palette[cur_color], &_r, &_g, &_b);
      break;

    case PRIME_COMET:
      if (tick >= 30 + 16) { // 15 + 8) {
        tick = 0;
        counter0 += (counter1 == 0) ? 2 : -2; // 1 : -1;
        if (counter0 <= 0) {
          counter1 = 0;
          cur_color = (cur_color + 1) % num_colors;
        } else if (counter0 >= 30) { // 15) {
          counter1 = 1;
        }
      }

      if (tick <= counter0) {
        unpackColor(palette[cur_color], &_r, &_g, &_b);
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    case PRIME_CANDY:
      if (tick >= 10 + 16) { // 5 + 8) {
        tick = 0;
        counter0++;
        if (counter0 >= 3) {
          counter0 = 0;
          counter1++;
          if (counter1 >= 8) {
            counter1 = 0;
            cur_color = (cur_color + 1) % num_colors;
          }
        }
      }

      if (tick < 10) { // 5) {
        unpackColor(palette[(cur_color + counter0) % num_colors], &_r, &_g, &_b);
      } else {
        _r = 0; _g = 0; _b = 0;
      }
      break;

    default:
      break;
  }
  tick++;
  *r = _r; *g = _g; *b = _b;
}

void Mode::init() {
  tick = 0;
  cur_color = 0;
  counter0 = 0;
  counter1 = 0;
}

void Mode::save() {
  EEPROM.update(eeprom_addr +  0, prime);
  EEPROM.update(eeprom_addr +  1, num_colors);
  EEPROM.update(eeprom_addr +  2, palette[0]);
  EEPROM.update(eeprom_addr +  3, palette[1]);
  EEPROM.update(eeprom_addr +  4, palette[2]);
  EEPROM.update(eeprom_addr +  5, palette[3]);
  EEPROM.update(eeprom_addr +  6, palette[4]);
  EEPROM.update(eeprom_addr +  7, palette[5]);
  EEPROM.update(eeprom_addr +  8, palette[6]);
  EEPROM.update(eeprom_addr +  9, palette[7]);
  EEPROM.update(eeprom_addr + 10, palette[8]);
  EEPROM.update(eeprom_addr + 11, palette[9]);
  EEPROM.update(eeprom_addr + 12, palette[10]);
  EEPROM.update(eeprom_addr + 13, palette[11]);
  EEPROM.update(eeprom_addr + 14, palette[12]);
  EEPROM.update(eeprom_addr + 15, palette[13]);
  EEPROM.update(eeprom_addr + 16, palette[14]);
  EEPROM.update(eeprom_addr + 17, palette[15]);
}

void Mode::load() {
  prime       = EEPROM.read(eeprom_addr + 0);
  num_colors  = EEPROM.read(eeprom_addr + 1);
  palette[0]  = EEPROM.read(eeprom_addr + 2);
  palette[1]  = EEPROM.read(eeprom_addr + 3);
  palette[2]  = EEPROM.read(eeprom_addr + 4);
  palette[3]  = EEPROM.read(eeprom_addr + 5);
  palette[4]  = EEPROM.read(eeprom_addr + 6);
  palette[5]  = EEPROM.read(eeprom_addr + 7);
  palette[6]  = EEPROM.read(eeprom_addr + 8);
  palette[7]  = EEPROM.read(eeprom_addr + 9);
  palette[8]  = EEPROM.read(eeprom_addr + 10);
  palette[9]  = EEPROM.read(eeprom_addr + 11);
  palette[10] = EEPROM.read(eeprom_addr + 12);
  palette[11] = EEPROM.read(eeprom_addr + 13);
  palette[12] = EEPROM.read(eeprom_addr + 14);
  palette[13] = EEPROM.read(eeprom_addr + 15);
  palette[14] = EEPROM.read(eeprom_addr + 16);
  palette[15] = EEPROM.read(eeprom_addr + 17);
}
