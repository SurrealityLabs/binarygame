#pragma once
#include <stdint.h>

void init_highscore(void);
void set_highscore(uint8_t score, uint8_t base);
uint8_t get_highscore(uint8_t base);