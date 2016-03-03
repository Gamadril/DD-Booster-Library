# DIGI-DOT-Booster-Library

This is a library for the DIGI-DOT-BOOSTER which can control LED stripes using the single wire protocol - ws2812 and compatible as well as RGBW LEDs like SK6812.

Detailed information including the datasheet and protocol description are available here:
http://www.led-genial.de/DIGI-DOT-Booster-WS2812-und-SK6812-ueber-SPI-Schnittstelle-ansteuern

DIGI-DOT-BOOSTER acts as a SPI slave and waits for commands sent by a SPI master - an Arduino in that case.
This Library provides an easy to use abstraction layer for commands supported by the DD-Booster and adds some additional
effects.

See [class documentation](http://gamadril.github.io/DD-Booster-Library) for detailed description of the functions.

You are welcome to extend the library with your own effects.

License: MIT