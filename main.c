#include <bcm2835.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stropts.h>
#include <unistd.h>

#define PIN RPI_GPIO_P1_03
#define DISPLAY_COMMON_GROUND

#include "types.h"
#include "configuration.h"

uint8_t symbols[DISPLAY_LINES] = {};

/*
 * Return 1 in have anything at stdin, else - 0
 */
int _kbhit() {
	static int initialized = 0;
	if (! initialized) {
		struct termios term;
		tcgetattr(STDIN_FILENO, &term);
		term.c_lflag &= ~ICANON;
		tcsetattr(STDIN_FILENO, TCSANOW, &term);
		setbuf(stdin, NULL);
		initialized = 1;
	}
	int bytesWaiting;
	ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
	return bytesWaiting;
}

/*
 * Turn on one line of matrix keyboard and one indicator
 */
int switch_line(int line) {
	for (int i=0; i<LINE_COUNT; i++)
		bcm2835_gpio_write(LINES[i], i==line ? !LINE_NORMAL_STATE : LINE_NORMAL_STATE);
}

/*
 * Turn off all lines and indicators
 */
void off_lines() {
	for (int i=0; i<LINE_COUNT; i++)
		bcm2835_gpio_write(LINES[i], LINE_NORMAL_STATE);
}

/*
 * Read keyboard line state
 */
int read_line(int line) {
	int result = 0;
	switch_line(line);
	for (int i=0; i<KEYBOARD_COLLUMN_COUNT; i++) {
		uint8_t keypress = LINE_STATE_CONVERSION(bcm2835_gpio_lev(KEYBOARD_PINS[i]));
		result = result | (keypress << i);
	}
	return result;
}

/*
 * Show symbol on indicator
 */
int show_symbol(int code, int line) {
	off_lines();
	for (int i=0; i<DISPLAY_PIN_COUNT; i++) {
		uint8_t mode = code & (1 << i);
		bcm2835_gpio_write(DISPLAY_PINS[i], DISPLAY_STATE_CONVERSION(mode));
	}
	switch_line(line);
}

/*
 * Read keyboard state (one-time) and show symbols stored at 'symbols' array
 */
int state() {
	int result = 0;
	for (int  i=0; i<DISPLAY_LINES; i++) {
		if (i < KEYBOARD_LINES) {
			int line_state = read_line(i);
			if (line_state != 0) {
				result = line_state | (1 << (KEYBOARD_COLLUMN_COUNT + i));
			}
		}
		if (i < DISPLAY_LINES) {
			show_symbol(symbols[i], i);
		}
		bcm2835_delay(DELAY);
	}
	return result;
}

/*
 * Read keyboard state multiple times and shows symbols stored at 'symbols' array
 */ 
int stage() {
	int result = state();
	for (int i=0; i<READ_COUNT - 1; i++) {
		result = result & state();
	}
	return result;
}

/*
 * Key by code
 */
KeyMask* mask(int keyCode) {
	if (keyCode == 0)
		return NULL;
	for (int i=0; i<KEYBOARD_MASK_COUNT; i++)
		if (KEYBOARD_MASKS[i].keyCode == keyCode)
			return &KEYBOARD_MASKS[i];
	return NULL;
}

/*
 * Symbol by code
 */
SymbolMask* symbol(char sym) {
	for (int i=0; i<SYMBOL_COUNT; i++) {
		if (SYMBOL_MASKS[i].symbol == sym) {
			return &SYMBOL_MASKS[i];
		}
	}
	return NULL;
}

/*
 * stdin handler
 */
void input_handler(int key) {
	static char buffer[LINE_COUNT] = {' ', ' ', ' '};
	if (key == 10 || key == 13) {
		SymbolMask* space = symbol(' ');
		for (int i=0; i<LINE_COUNT; i++) {
			SymbolMask* mask = symbol(buffer[i]);
			if (mask == NULL)
				mask = space;
			symbols[i] = mask->code;
			buffer[i] = 0;
		}
	} else {
		for (int i=1; i<DISPLAY_LINES; i++) {
			buffer[i - 1] = buffer[i];
		}
		buffer[DISPLAY_LINES - 1] = key;
	}
}

/*
 * initialize GPIO
 */
void initialize() {
	for (int i=0; i<LINE_COUNT; i++) {
		bcm2835_gpio_fsel(LINES[i], BCM2835_GPIO_FSEL_OUTP);
		bcm2835_gpio_write(DISPLAY_PINS[i], LINE_NORMAL_STATE);
	}
	for (int i=0; i<KEYBOARD_COLLUMN_COUNT; i++) {
		bcm2835_gpio_fsel(KEYBOARD_PINS[i], BCM2835_GPIO_FSEL_INPT);
		bcm2835_gpio_set_pud(KEYBOARD_PINS[i], KEYBOARD_PINS_MODE);
	}
	for (int i=0; i<DISPLAY_PIN_COUNT; i++) {
		bcm2835_gpio_fsel(DISPLAY_PINS[i], BCM2835_GPIO_FSEL_OUTP);
		bcm2835_gpio_write(DISPLAY_PINS[i], DISPLAY_NORMAL_STATE);
	}
}

int main() {
	if (!bcm2835_init())
		return 1;
	initialize();
	int keyboard_state = -1;
	while(1) {
		int key = _kbhit();
		if (key != 0) {
			input_handler(getchar());
		}
		int new_keyboard_state = stage();
		if (new_keyboard_state != keyboard_state) {
			keyboard_state = new_keyboard_state;
			KeyMask* key = mask(keyboard_state);
			if (key != NULL) {
				printf("%s\n", key->key);
				fflush(stdout);
			}
		}
	}
	return 0;
}
