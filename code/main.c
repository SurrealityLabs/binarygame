#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "ui.h"
#include "demo.h"
#include "game.h"
#include "highscore.h"
#include "tone.h"

typedef enum binaryGameState {
	STATE_SOUND_OFF,
	STATE_SOUND_ON,
	STATE_START,
	STATE_DEMO,
	STATE_GAME
} binaryGameState_t;

static binaryGameState_t state;

volatile uint16_t msec_timer;

static const __flash uint8_t startString[] = {0x00, 0x00, 0x00, 0x00, 0xB6, 0x05, 0x86, 0xD7, 0x82, 0x37, 0x00, 0x77, 0xD7, 0xC6, 0xF2, 0x00, 0x0D, 0xF5, 0x00, 0x00, 0x00, 0x00};
static const __flash uint8_t startStringLen = 22;
static const __flash uint8_t soundOffString[] = {0x00, 0xF5, 0xD2, 0xD2};
static const __flash uint8_t soundOnString[] = {0x00, 0xF5, 0x86, 0x00};

uint16_t get_timer(void);
void init_timer(void);

int main(void) {
	msec_timer = 0;
	uint8_t snd_enabled;

	ui_init();
	init_timer();
	init_highscore();

	snd_enabled = get_sound();

	if(!(PIND & 0x04)) {
		if(snd_enabled) {
			snd_enabled = 0;
			state = STATE_SOUND_OFF;
		} else {
			snd_enabled = 1;
			state = STATE_SOUND_ON;
		}
		set_sound(snd_enabled);
	} else {
		state = STATE_START;
	}

	if(snd_enabled) {
		tone_init();
	}

	sei();

	uint8_t i = 0;
	uint16_t timerStart = get_timer();

	while(1) {
		switch(state) {
			case STATE_SOUND_OFF:
				ui_setDisplayRaw(soundOffString);
				if(get_timer() - timerStart > 250) {
					timerStart = get_timer();
					state = STATE_START;
				}
				break;
			case STATE_SOUND_ON:
				ui_setDisplayRaw(soundOnString);
				if(get_timer() - timerStart > 250) {
					timerStart = get_timer();
					state = STATE_START;
				}
				break;
			case STATE_START:
				ui_setDisplayRaw(&(startString[i]));
				if(get_timer() - timerStart > 250) {
					timerStart = get_timer();
					i++;
					if(i > (startStringLen - 4)) {
						state = STATE_DEMO;
						demo_start();
					}
				}
				break;
			case STATE_DEMO:
				if(demo_update()) {
					state = STATE_GAME;
					game_start();
				}
				break;
			case STATE_GAME:
				if(game_update()) {
					state = STATE_DEMO;
					demo_start();
				}
				break;
			default:
				state = STATE_DEMO;
				demo_start();
				break;
		}
		tone_loop();
	}

	return 0;
}

uint16_t get_timer(void) {
	return msec_timer;
}

void init_timer(void) {
	TCCR1A = 0x0;
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);
	TCCR1C = 0x0;
	ICR1 = 8000;
	OCR1A = 1000;
	OCR1B = 7000;
	TIMSK |= (1 << ICIE1) | (1 << OCIE1A) | (1 << OCIE1B);
}

ISR(TIMER1_CAPT_vect) {
	msec_timer++;
	ui_overflowISR();
}

ISR(TIMER1_COMPA_vect) {
	ui_matchISR1();
}

ISR(TIMER1_COMPB_vect) {
	ui_matchISR2();
}
