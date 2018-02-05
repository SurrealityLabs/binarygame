#include "highscore.h"
#include <avr/eeprom.h>
#include <stdint.h>

#define ADDRESS_GUARD 1
#define GUARD_VALUE 0x42
#define ADDRESS_HS_8 2
#define ADDRESS_HS_10 3
#define ADDRESS_HS_16 4

void init_highscore(void) {
	uint8_t temp;

	temp = eeprom_read_byte((uint8_t*)ADDRESS_GUARD);

	if(temp != GUARD_VALUE) {
		eeprom_write_byte((uint8_t*)ADDRESS_HS_8, 0);
		eeprom_write_byte((uint8_t*)ADDRESS_HS_10, 0);
		eeprom_write_byte((uint8_t*)ADDRESS_HS_16, 0);
		eeprom_write_byte((uint8_t*)ADDRESS_GUARD, GUARD_VALUE);
	}

	// dummy read to address 0 to protect against errant writes on shutdown
	temp = eeprom_read_byte((uint8_t*)0);
}

void set_highscore(uint8_t score, uint8_t base) {
	uint8_t temp;

	if(base == 8) {
		eeprom_write_byte((uint8_t*)ADDRESS_HS_8, score);
	} else if(base == 10) {
		eeprom_write_byte((uint8_t*)ADDRESS_HS_10, score);
	} else if(base == 16) {
		eeprom_write_byte((uint8_t*)ADDRESS_HS_16, score);
	}

	// dummy read to address 0 to protect against errant writes on shutdown
	temp = eeprom_read_byte((uint8_t*)0);
	temp++;
}

uint8_t get_highscore(uint8_t base) {
	uint8_t temp, temp2;

	if(base == 8) {
		temp = eeprom_read_byte((uint8_t*)ADDRESS_HS_8);
	} else if(base == 10) {
		temp = eeprom_read_byte((uint8_t*)ADDRESS_HS_10);
	} else if(base == 16) {
		temp = eeprom_read_byte((uint8_t*)ADDRESS_HS_16);
	} else {
		temp = 0;
	}

	// dummy read to address 0 to protect against errant writes on shutdown
	temp2 = eeprom_read_byte((uint8_t*)0);
	temp2++;

	return temp;
}
