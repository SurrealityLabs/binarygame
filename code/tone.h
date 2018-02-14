#pragma once
#include <stdint.h>

#define NOTE_C      239
#define NOTE_CSHARP 225
#define NOTE_D      213
#define NOTE_EFLAT  201
#define NOTE_E      190
#define NOTE_F      179
#define NOTE_FSHARP 169
#define NOTE_G      159
#define NOTE_GSHARP 150
#define NOTE_A      142
#define NOTE_BFLAT  134
#define NOTE_B      127

void tone_init(void);
void tone_start(uint8_t tone, uint8_t octave, uint16_t duration);
void tone_loop(void);
void tone_stop(void);
uint8_t tone_playing(void);
