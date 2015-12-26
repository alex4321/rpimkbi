#define LINE_COUNT 3
#define KEYBOARD_LINES 3
#define DISPLAY_LINES 3
#define KEYBOARD_COLLUMN_COUNT 4
#define DISPLAY_PIN_COUNT 7
#define READ_COUNT 4
#define DELAY 10
#define KEYBOARD_MASK_COUNT 12
#define SYMBOL_COUNT 11

/*
 * Pin numbers
 */
const int LINES[LINE_COUNT] = {2, 3, 17};
const int KEYBOARD_PINS[KEYBOARD_COLLUMN_COUNT] = {27, 22, 10, 9};
const int DISPLAY_PINS[DISPLAY_PIN_COUNT] = {14, 15, 18, 23, 24, 25, 8};

/*
 * Code formiration rule is :
 * (1 << lineNumber) | (1 << collumnNumber)
 * where minimal value of lineNumber and lineNumber is 0
 */
KeyMask KEYBOARD_MASKS[KEYBOARD_MASK_COUNT] = {
	{.keyCode = 72, .key="1"},
	{.keyCode = 40, .key="2"},
	{.keyCode = 24, .key="3"},
	{.keyCode = 68, .key="4"},
	{.keyCode = 36, .key="5"},
	{.keyCode = 20, .key="6"},
	{.keyCode = 66, .key="7"},
	{.keyCode = 34, .key="8"},
	{.keyCode = 18, .key="9"},
	{.keyCode = 65, .key="b"},
	{.keyCode = 33, .key="0"},
	{.keyCode = 17, .key="e"},
};
SymbolMask SYMBOL_MASKS[SYMBOL_COUNT] = {
	{.symbol = ' ', .code = 0b00000000},
	{.symbol = '0', .code = 0b01111011},
	{.symbol = '1', .code = 0b01000001},
	{.symbol = '2', .code = 0b00111101},
	{.symbol = '3', .code = 0b01101101},
	{.symbol = '4', .code = 0b01000111},
	{.symbol = '5', .code = 0b01101110},
	{.symbol = '6', .code = 0b01111110},
	{.symbol = '7', .code = 0b01001001},
	{.symbol = '8', .code = 0b01111111},
	{.symbol = '9', .code = 0b01101111},
};

#ifdef DISPLAY_COMMON_GROUND
	#define LINE_NORMAL_STATE HIGH
	#define KEYBOARD_PINS_MODE BCM2835_GPIO_PUD_UP
	#define DISPLAY_NORMAL_STATE LOW
	#define LINE_STATE_CONVERSION(x) !x
	#define DISPLAY_STATE_CONVERSION(x) x
#else
#ifdef DISPLAY_COMMON_POWER
	#define LINE_NORMAL_STATE LOW
	#define KEYBOARD_PINS_MODE BCM2835_GPIO_PUD_DOWN
	#define DISPLAY_NORMAL_STATE HIGH
	#define LINE_STATE_CONVERSION(x) x
	#define DISPLAY_STATE_CONVERSION(x) !x
#else
	#error You must define DISPLAY_COMMON_GROUND or DISPLAY_COMMON_POWER
#endif
#endif
