#include "ui.h"
#include "game.h"
#include <stdlib.h>
#include <stdint.h>
#include "highscore.h"

const __flash uint8_t yesString[]   = {0x00, 0x37, 0xF2, 0x76};
const __flash uint8_t noString[]    = {0x00, 0x86, 0xA6, 0x00};
const __flash uint8_t gameString[]  = {0x77, 0xD7, 0xC6, 0xF2};
const __flash uint8_t overString[]  = {0xF5, 0xB5, 0xF2, 0x82};
const __flash uint8_t timeString[]  = {0x45, 0x05, 0xC6, 0xF2};
const __flash uint8_t goString[]    = {0x00, 0x77, 0xA6, 0xC3};

#define MAX_FAILURES 3
#define MAX_WINS 24

typedef enum gameBinaryState {
	GAME_IDLE,
	GAME_ROUND_START,
	GAME_ROUND,
	GAME_CORRECT,
	GAME_INCORRECT,
	GAME_TIME_UP,
	GAME_OVER_START,
	GAME_OVER,
	GAME_SHOW_SCORE
} gameBinaryState_t;

static gameBinaryState_t state;
static uint8_t base;
static uint8_t modeButtonArmed = 0;
static uint8_t goButtonArmed = 0;

static uint8_t numberToGuess;
static uint8_t numFailures;
static uint8_t numWins;
static uint16_t timerStart;
static uint8_t playerScore;
static uint8_t highScore;

extern uint16_t get_timer(void);

static void lightup_base(void) {
	if(base == 8) {
		ui_setOctLED(1);
	} else {
		ui_setOctLED(0);
	}

	if(base == 10) {
		ui_setDecLED(1);
	} else {
		ui_setDecLED(0);
	}

	if(base == 16) {
		ui_setHexLED(1);
	} else {
		ui_setHexLED(0);
	}
}

void game_start(void) {
	state = GAME_IDLE;
	timerStart = get_timer();
	modeButtonArmed = 0;
	goButtonArmed = 0;
	base = 8;
	highScore = get_highscore(base);
	lightup_base();
	ui_setGameLED(1);
}

static uint8_t game_next(void) {
	if(base == 8) {
		base = 10;
	} else if(base == 10) {
		base = 16;
	} else {
		base = 8;
		return 1;
	}

	highScore = get_highscore(base);
	modeButtonArmed = 0;
	goButtonArmed = 0;
	lightup_base();
	ui_setGameLED(1);
	return 0;
}

uint8_t game_update(void) {
	uint8_t input;

	input = ui_readBinarySwitches();
	ui_setBinaryLEDs(input);

	switch(state) {
		case GAME_IDLE:
			// TODO: Display old high score, with h prefix
			if(get_timer() - timerStart < 2000) {
				ui_setDisplayRaw(goString);
			} else if(get_timer() - timerStart < 4000) {
				ui_setDisplayDigits(highScore, 10);
			} else {
				timerStart = get_timer();
			}

			if(ui_readModeButton()) {
				modeButtonArmed = 1;
			} else if(modeButtonArmed && !ui_readModeButton()) {
				modeButtonArmed = 0;
				// next mode
				if(game_next()) {
					return 1;
				} else {
					state = GAME_IDLE;
				}
			}

			if(ui_readGoButton()) {
				goButtonArmed = 1;
			} else if(goButtonArmed && !ui_readGoButton()) {
				goButtonArmed = 0;
				
				srand(get_timer());
				numFailures = 0;
				numWins = 0;
				playerScore = 0;

				state = GAME_ROUND_START;
			}

			break;
		case GAME_ROUND_START:
			numberToGuess = rand() & 0xFF;
			timerStart = get_timer();
			state = GAME_ROUND;
			break;
		case GAME_ROUND:
			ui_setDisplayDigits(numberToGuess, base);

			// press GO button
			if(ui_readGoButton()) {
				goButtonArmed = 1;
			} else if(goButtonArmed && !ui_readGoButton()) {
				if(input == numberToGuess) {
					timerStart = get_timer();
					state = GAME_CORRECT;
				} else {
					timerStart = get_timer();
					state = GAME_INCORRECT;
				}
				goButtonArmed = 0;
				break;
			}

			// time up
			if(get_timer() - timerStart > 20000) {
				timerStart = get_timer();
				state = GAME_TIME_UP;
				break;
			}

			// 
			break;
		case GAME_CORRECT:
			ui_setDisplayRaw(yesString);
			if(get_timer() - timerStart > 2000) {
				playerScore += 1;
				numWins++;
				if(numWins >= MAX_WINS) {
					timerStart = get_timer();
					state = GAME_OVER_START;
				} else {
					state = GAME_ROUND_START;
					// TODO show increment score
				}
			}
			break;
		case GAME_INCORRECT:
			ui_setDisplayRaw(noString);
			if(get_timer() - timerStart > 2000) {
				numFailures++;
				if(numFailures >= MAX_FAILURES) {
					timerStart = get_timer();
					state = GAME_OVER_START;
				} else {
					state = GAME_ROUND_START;
					// TODO show score
				}
			}
			break;
		case GAME_TIME_UP:
			ui_setDisplayRaw(timeString);
			if(get_timer() - timerStart > 2000) {
				numFailures++;
				if(numFailures >= MAX_FAILURES) {
					timerStart = get_timer();
					state = GAME_OVER_START;
				} else {
					state = GAME_ROUND_START;
					// TODO show score
				}
			}
			break;
		case GAME_OVER_START:
			// Save new high score
			if(playerScore > highScore) {
				set_highscore(playerScore, base);
			}
			state = GAME_OVER;
			break;
		case GAME_OVER:
			if(get_timer() - timerStart < 1000) {
				ui_setDisplayRaw(gameString);
			} else if(get_timer() - timerStart < 2000) {
				ui_setDisplayRaw(overString);
			} else {
				goButtonArmed = 0;
				modeButtonArmed = 0;
				state = GAME_SHOW_SCORE;
			}
			break;
		case GAME_SHOW_SCORE:
			ui_setDisplayDigits(playerScore, 10);

			if(ui_readModeButton()) {
				modeButtonArmed = 1;
			} else if(modeButtonArmed && !ui_readModeButton()) {
				modeButtonArmed = 0;
				// next mode
				timerStart = get_timer();
				state = GAME_IDLE;
			}

			if(ui_readGoButton()) {
				goButtonArmed = 1;
			} else if(goButtonArmed && !ui_readGoButton()) {
				goButtonArmed = 0;
				// next mode
				timerStart = get_timer();
				state = GAME_IDLE;
			}
			break;
	}

	return 0;
}