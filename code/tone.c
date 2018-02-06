#include <stdint.h>
#include <avr/interrupt.h>
#include "tone.h"

static uint16_t toneStart;
static uint8_t toneDuration;

extern uint16_t get_timer(void);

void tone_init(void) {
	OCR0A = 0;
	OCR0B = 50;
	TCCR0B = 0x05; // set prescaler clk/1024
	TCCR0B|=(1<<WGM02);
	TCCR0A|=((0<<COM0A1)|(1<<COM0A0)|(1<<WGM01)|(1<<WGM00)); // set PWM mode
	toneStart = 0;
}

void tone_start(uint8_t tone, uint8_t octave, uint8_t duration) {
	toneDuration = duration;
	if(octave == 3) {
		OCR0B = tone;
		TCCR0B &= 0xF8;
		TCCR0B |= 0x04;
	} else if(octave == 4) {
		OCR0B = tone / 2;
		TCCR0B &= 0xF8;
		TCCR0B |= 0x04;
	} else if(octave == 5) {
		OCR0B = tone;
		TCCR0B &= 0xF8;
		TCCR0B |= 0x03;
	}
	OCR0A = OCR0A / 2;
}

void tone_loop(void) {
	if(get_timer() - toneStart > toneDuration) {
		toneDuration = 0;
		OCR0A = 0;
	}
}

uint8_t tone_playing(void) {
	return OCR0A ? 1 : 0;
}

void tone_stop(void) {
	OCR0A = 0;
}
