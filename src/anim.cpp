#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "anim.h"

uint8_t _r, _g, _b;
uint8_t r0, g0, b0;
uint8_t r1, g1, b1;


const PROGMEM uint8_t color_palette[64][3] = {
  // Dims
  {  0,   0,   0}, // 0x00
  { 32,  32,  32}, // 0x01
  { 48,   0,   0}, // 0x02
  { 40,  40,   0}, // 0x03
  {  0,  48,   0}, // 0x04
  {  0,  40,  40}, // 0x05
  {  0,   0,  48}, // 0x06
  { 40,   0,  40}, // 0x07

  // Red -> green
  {255,   0,   0}, // 0x08
  {252,  64,   0}, // 0x09
  {248, 128,   0}, // 0x0A
  {244, 192,   0}, // 0x0B
  {240, 240,   0}, // 0x0C
  {192, 244,   0}, // 0x0D
  {128, 248,   0}, // 0x0E
  { 64, 252,   0}, // 0x0F

  // Green -> blue
  {  0, 255,   0}, // 0x10
  {  0, 252,  64}, // 0x11
  {  0, 248, 128}, // 0x12
  {  0, 244, 192}, // 0x13
  {  0, 240, 240}, // 0x14
  {  0, 192, 244}, // 0x15
  {  0, 128, 248}, // 0x16
  {  0,  64, 252}, // 0x17

  // Blue -> red
  {  0,   0, 255}, // 0x18
  { 64,   0, 252}, // 0x19
  {128,   0, 248}, // 0x1A
  {192,   0, 244}, // 0x1B
  {240,   0, 240}, // 0x1C
  {244,   0, 192}, // 0x1D
  {248,   0, 128}, // 0x1E
  {252,   0,  64}, // 0x1F

  // Red saturated
  {240,  32,  32}, // 0x20
  {216,  64,  64}, // 0x21
  {192,  96,  96}, // 0x22
  {160, 128, 128}, // 0x23

  // Yellow saturated
  {224, 224,  32}, // 0x24
  {200, 200,  64}, // 0x25
  {176, 176,  96}, // 0x26
  {152, 152, 128}, // 0x27

  // Green saturated
  { 32, 240,  32}, // 0x28
  { 64, 216,  64}, // 0x29
  { 96, 192,  96}, // 0x2A
  {128, 160, 128}, // 0x2B

  // Cyan saturated
  { 32, 224, 224}, // 0x2C
  { 64, 200, 200}, // 0x2D
  { 96, 176, 176}, // 0x2E
  {128, 152, 152}, // 0x2F

  // Blue saturated
  { 32,  32, 240}, // 0x30
  { 64,  64, 216}, // 0x31
  { 96,  96, 192}, // 0x32
  {128, 128, 160}, // 0x33

  // Magenta saturated
  {224,  32, 224}, // 0x34
  {200,  64, 200}, // 0x35
  {176,  96, 176}, // 0x36
  {152, 128, 152}, // 0x37

  // Whites
  {160, 160, 160}, // 0x38
  { 50, 114,  96}, // 0x39
  { 50, 147, 135}, // 0x3A
  {128,  50,  96}, // 0x3B
  { 96,  50, 128}, // 0x3C
  {128,  96,  50}, // 0x3D
  { 96, 128,  50}, // 0x3E
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

void Mode::prime_strobe(uint8_t c_time, uint8_t b_time) {
  if (tick >= c_time + b_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
  }

  if (tick < c_time) {
    unpackColor(palette[cur_color], &_r, &_g, &_b);
  } else {
    _r = 0; _g = 0; _b = 0;
  }
}

void Mode::prime_pulse(uint8_t c_time, uint8_t b_time) {
  if (tick >= c_time + b_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
  }

  if (tick < c_time / 2) {
    unpackColor(palette[cur_color], &r0, &g0, &b0);
    morphColor(tick, c_time / 2, 0, 0, 0, r0, g0, b0, &_r, &_g, &_b);
  } else if (tick < c_time) {
    unpackColor(palette[cur_color], &r0, &g0, &b0);
    morphColor(tick - (c_time / 2), c_time - (c_time / 2), r0, g0, b0, 0, 0, 0, &_r, &_g, &_b);
  } else {
    _r = 0; _g = 0; _b = 0;
  }
}

void Mode::prime_tracer(uint8_t c_time, uint8_t b_time) {
  if (tick >= c_time + b_time) {
    tick = 0;
    cur_color = (cur_color + 1) % (num_colors - 1);
  }

  if (tick < c_time) {
    unpackColor(palette[(cur_color + 1) % num_colors], &_r, &_g, &_b);
  } else {
    unpackColor(palette[0], &_r, &_g, &_b);
  }
}

void Mode::prime_dashdops(uint8_t c_time, uint8_t d_time, uint8_t b_time, uint8_t dops) {
  counter1 = num_colors - 1;
  if (tick >= (counter1 * c_time) + ((d_time + b_time) * dops) + b_time) {
    tick = 0;
  }

  if (tick < counter1 * c_time) {
    unpackColor(palette[(tick / c_time) + 1], &_r, &_g, &_b);
  } else {
    counter0 = tick - (counter1 * c_time);
    if (counter0 % (d_time + b_time) >= b_time) {
      unpackColor(palette[0], &_r, &_g, &_b);
    } else {
      _r = 0; _g = 0; _b = 0;
    }
  }
}

void Mode::prime_blinke(uint8_t c_time, uint8_t b_time) {
  if (tick >= (num_colors * c_time) + b_time) {
    tick = 0;
  }

  if (tick < num_colors * c_time) {
    unpackColor(palette[tick / c_time], &_r, &_g, &_b);
  } else {
    _r = 0; _g = 0; _b = 0;
  }
}

void Mode::prime_edge(uint8_t c_time, uint8_t e_time, uint8_t b_time) {
  if (counter0 == 0) counter0 = num_colors - 1;
  if (tick >= (counter0 * c_time * 2) + e_time + b_time) {
    tick = 0;
  }

  if (tick < counter0 * c_time) {
    unpackColor(palette[counter0 - (tick / c_time)], &_r, &_g, &_b);
  } else if (tick < (counter0 * c_time) + e_time) {
    unpackColor(palette[0], &_r, &_g, &_b);
  } else if (tick < (counter0 * c_time * 2) + e_time) {
    unpackColor(palette[((tick - ((counter0 * c_time) + e_time)) / c_time) + 1], &_r, &_g, &_b);
  } else {
    _r = 0; _g = 0; _b = 0;
  }
}

void Mode::prime_lego(uint8_t b_time) {
  if (counter0 == 0) counter0 = getLegoTime();
  if (tick >= counter0 + b_time) {
    tick = 0;
    cur_color = (cur_color + 1) % num_colors;
    counter0 = getLegoTime();
  }

  if (tick < counter0) {
    unpackColor(palette[cur_color], &_r, &_g, &_b);
  } else {
    _r = 0; _g = 0; _b = 0;
  }
}

void Mode::prime_chase(uint8_t c_time, uint8_t b_time, uint8_t steps) {
  if (tick >= c_time + b_time) {
    tick = 0;
    counter0++;
    if (counter0 >= steps - 1) {
      counter0 = 0;
      cur_color = (cur_color + 1) % num_colors;
    }
  }

  if (tick < c_time) {
    counter1 = tick / (c_time / steps);
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
}

void Mode::prime_morph(uint8_t c_time, uint8_t b_time, uint8_t steps) {
  if (tick >= c_time + b_time) {
    tick = 0;
    counter0++;
    if (counter0 >= steps) {
      counter0 = 0;
      cur_color = (cur_color + 1) % num_colors;
    }
  }

  if (tick < c_time) {
    unpackColor(palette[cur_color],                    &r0, &g0, &b0);
    unpackColor(palette[(cur_color + 1) % num_colors], &r1, &g1, &b1);
    morphColor(tick + ((c_time + b_time) * counter0), (c_time + b_time) * steps, r0, g0, b0, r1, g1, b1, &_r, &_g, &_b);
  } else {
    _r = 0; _g = 0; _b = 0;
  }
}

void Mode::prime_comet(uint8_t c_time, uint8_t b_time, uint8_t per_step) {
  if (tick >= c_time + b_time) {
    tick = 0;
    counter0 += (counter1 == 0) ? per_step : -1 * per_step;
    if (counter0 <= 0) {
      counter1 = 0;
      cur_color = (cur_color + 1) % num_colors;
    } else if (counter0 >= c_time) {
      counter1 = 1;
    }
  }

  if (tick <= counter0) {
    unpackColor(palette[cur_color], &_r, &_g, &_b);
  } else {
    _r = 0; _g = 0; _b = 0;
  }
}

void Mode::prime_candy(uint8_t c_time, uint8_t b_time, uint8_t pick, uint8_t repeat) {
  if (tick >= c_time + b_time) {
    tick = 0;
    counter0++;
    if (counter0 >= pick) {
      counter0 = 0;
      counter1++;
      if (counter1 >= repeat) {
        counter1 = 0;
        cur_color = (cur_color + 1) % num_colors;
      }
    }
  }

  if (tick < c_time) {
    unpackColor(palette[(cur_color + counter0) % num_colors], &_r, &_g, &_b);
  } else {
    _r = 0; _g = 0; _b = 0;
  }
}

void Mode::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  switch (prime) {
    case PRIME_STROBE:
      prime_strobe(10, 16);
      break;
    case PRIME_HYPER:
      prime_strobe(34, 34);
      break;
    case PRIME_DOPS:
      prime_strobe(3, 20);
      break;
    case PRIME_STROBIE:
      prime_strobe(6, 46);
      break;
    case PRIME_PULSE:
      prime_pulse(200, 50);
      break;
    case PRIME_SEIZURE:
      prime_strobe(10, 190);
      break;
    case PRIME_TRACER:
      prime_tracer(6, 46);
      break;
    case PRIME_DASHDOPS:
      prime_dashdops(22, 3, 20, 7);
      break;
    case PRIME_BLINKE:
      prime_blinke(10, 100);
      break;
    case PRIME_EDGE:
      prime_edge(4, 16, 40);
      break;
    case PRIME_LEGO:
      prime_lego(16);
      break;
    case PRIME_CHASE:
      prime_chase(100, 20, 5);
      break;
    case PRIME_MORPH:
      prime_morph(34, 34, 4);
      break;
    case PRIME_RIBBON:
      prime_strobe(22, 0);
      break;
    case PRIME_COMET:
      prime_comet(30, 16, 2);
      break;
    case PRIME_CANDY:
      prime_candy(10, 16, 3, 8);
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
