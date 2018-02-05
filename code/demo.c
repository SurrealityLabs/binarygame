#include "ui.h"
#include "demo.h"
#include <stdint.h>

typedef enum demoBinaryState {
	DEMO_OCTAL,
	DEMO_DECIMAL,
	DEMO_HEXADECIMAL
} demoBinaryState_t;

static demoBinaryState_t state;
static uint8_t buttonArmed = 0;

void demo_start(void) {
	state = DEMO_OCTAL;
	buttonArmed = 0;
	ui_setGameLED(0);
}

uint8_t demo_update(void) {
	uint8_t input;

	input = ui_readBinarySwitches();
	ui_setBinaryLEDs(input);

	switch(state) {
		case DEMO_OCTAL:
			ui_setOctLED(1);
			ui_setDecLED(0);
			ui_setHexLED(0);
			ui_setDisplayDigits(input, 8);
			if(ui_readModeButton()) {
				buttonArmed = 1;
			} else if(buttonArmed && !ui_readModeButton()) {
				state = DEMO_DECIMAL;
				buttonArmed = 0;
			}
			break;
		case DEMO_DECIMAL:
			ui_setOctLED(0);
			ui_setDecLED(1);
			ui_setHexLED(0);
			ui_setDisplayDigits(input, 10);
			if(ui_readModeButton()) {
				buttonArmed = 1;
			} else if(buttonArmed && !ui_readModeButton()) {
				state = DEMO_HEXADECIMAL;
				buttonArmed = 0;
			}
			break;
		case DEMO_HEXADECIMAL:
			ui_setOctLED(0);
			ui_setDecLED(0);
			ui_setHexLED(1);
			if(ui_readModeButton()) {
				buttonArmed = 1;
			} else if(buttonArmed && !ui_readModeButton()) {
				return 1;
			}
			break;
		default:
			demo_start();
	}

	return 0;
}