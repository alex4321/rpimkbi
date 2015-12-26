This is C programm to realize input/output with matrix keyboard and 7-segment display on Raspberry PI.
You must previously install `gcc` and `bcm2835` library to build it.
Program output pressed key to stdin and show symbols from stdout, so you can use it from your highter-level language programs (in my case - Python).

Configuration (see configuration.h):

* KEYBOARD_LINES - count of pin used to switch keyboard lines
* DISPLAY_LINES - count of PIN used to switch indicators
* LINE_COUNT - max of KEYBOARD_LINES and DISPLAY_LINES
* LINES - "line" GPIO pin numbers
KEYBOARD_PINS - keyboard "collumn" PINS
* DISPLAY_PINS - diplay LED pins
* READ_COUNT - count of needed same-value keyboard readngs
* DELAY - read/show loop delay
