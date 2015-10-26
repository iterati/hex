#ifndef __ANIM_H
#define __ANIM_H

#include <Arduino.h>

#define PALETTE_SIZE      12

#define PRIME_STROBE      0
#define PRIME_HYPER       1
#define PRIME_DOPS        2
#define PRIME_STROBIE     3
#define PRIME_PULSE       4
#define PRIME_SEIZURE     5
#define PRIME_TRACER      6
#define PRIME_DASHDOPS    7
#define PRIME_BLINKE      8
#define PRIME_EDGE        9
#define PRIME_LEGO        10
#define PRIME_CHASE       11
#define PRIME_MORPH       12
#define PRIME_RIBBON      13
#define PRIME_COMET       14
#define PRIME_CANDY       15


void unpackColor(uint8_t color, uint8_t *r, uint8_t *g, uint8_t *b);

class Mode {
  public:
    Mode(uint16_t user_eeprom_addr, uint8_t user_prime0, uint8_t user_num_colors,
        uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5,
        uint8_t c6, uint8_t c7, uint8_t c8, uint8_t c9, uint8_t c10, uint8_t c11);

    void init();
    void reset();
    void load();
    void save();

    void render(uint8_t *r, uint8_t *g, uint8_t *b);

    uint32_t tick;
    uint16_t eeprom_addr;
    uint8_t cur_color;
    int16_t counter0, counter1;

    uint8_t prime;
    uint8_t num_colors;
    uint8_t palette[PALETTE_SIZE];
};

#endif
