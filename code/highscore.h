#pragma once
#include <stdint.h>

void init_highscore(void);
void set_highscore(uint8_t score, uint8_t base);
uint8_t get_highscore(uint8_t base);
void set_sound(uint8_t en);
uint8_t get_sound(void);