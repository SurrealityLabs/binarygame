#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "ui.h"
#include "demo.h"
#include "game.h"

typedef enum binaryGameState {
	STATE_DEMO,
	STATE_GAME
} binaryGameState_t;

static binaryGameState_t state;

volatile uint16_t msec_timer;

uint16_t get_timer(void);
void init_timer(void);

int main(void) {
	msec_timer = 0;

	ui_init();
	init_timer();

	sei();

	state = STATE_DEMO;

	demo_start();

	while(1) {
		switch(state) {
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
	TIMSK |= (1 << TOIE1) | (1 << OCIE1A) | (1 << OCIE1B);
}

ISR(TIMER1_OVF_vect) {
	msec_timer++;
	ui_overflowISR();
}

ISR(TIMER1_COMPA_vect) {
	ui_matchISR1();
}

ISR(TIMER1_COMPB_vect) {
	ui_matchISR2();
}
