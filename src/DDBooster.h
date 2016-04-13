/*
 * DDBoster.h - Library to control the Digi-Dot-Booster using a high-level API
 *
 * https://github.com/Gamadril/DD-Booster-Library
 * MIT License
 */
#ifndef DD_BOOSTER_LIBRARY_DDBOOSTER_H
#define DD_BOOSTER_LIBRARY_DDBOOSTER_H

#include <Arduino.h>
#include <SPI.h>

/**
 * @brief Class acts as a wrapper around SPI calls to control the Digi-Dot-Booster.
 * 
 * After creation of the class instance SPI is configured with default values (12MHz, MSB first, mode 0).
 * By default the library uses the SS pin of the Arduino platform for the communication with the Digi-Dot-Booster.
 * If you prefer to use another digital pin for chip select use configurePins to set it.
 * Using configurePins you can also set a reset pin used to toggle the hardware reset of the DD-Booster.
 *  
 * Before calling any functions you have first to initialize the DD-Booster by calling init() with the number of the LEDs (max. 256),
 * their type (RGB or RGBW) and the color order (RGB or GRB). Both last parameters are optional - DD-Booster is
 * configured for the ws2812 LEDS (RGB type with GRB color order).
 * 
 * When calling the functions the corresponding values are sent to the DD-Booster, but only
 * after the show() call the LEDs are really addressed with the current state of the values buffer. 
 */
class DDBooster {
public:

    /**
     * LED type. Stores the number of bits used for the color of one LED.
     * LED_RGB is the default value used by the DD-Booster
     */
    enum LedType {
        LED_RGB = 24,
        LED_RGBW = 32
    };

    /**
     * LED color order. ws2812 is using GRB order and it's the default color order 
     * for the DD-Booster.
     */
    enum LedColorOrder {
        ORDER_RGB,
        ORDER_GRB
    };

    /**
     * Default constructor. Initializes SPI interface at 12MHz, MSB first, mode 0
     */
    DDBooster();

    /**
     * Reassigns used pins for SPI chip select and reset pin.
     * By default the library uses the default SS pin of the SPI interface.
     * To be able to do a hardware reset of the DD-Booster, connect a digital IO
     * pin to the RESET pin of the DD-Booster.
     * @param csPin - Digital Arduino pin to use as chip select
     * @param resetPin - Digital Arduino pin connected to the RESET pin of the DD-Booster
     */
    void configurePins(uint8_t csPin, uint8_t resetPin = 0xFF);

    /**
     * Performs initial configuration of the DD-Booster to set the number of used LEDs and their type.
     * DD-Booster supports max. 256 LEDs.
     * @param ledCount - Number of used LEDs
     * @param ledType - Type of LEDs used. RGB is default
     * @param colorOrder - LED color order. GRB is default
     */
    void init(uint16_t ledCount, LedType ledType = LED_RGB, LedColorOrder colorOrder = ORDER_GRB);

    /**
     * Performs a hardware reset of the DD-Booster by toggling it's RESET pin.
     * To use this function, set the corresponding pin first using configurePins otherwise
     * the call of reset() does nothing.
     */
    void reset();

    /**
     * Sets a LED color for next operations using RGB format until another color
     * set operation overwrites it.
     * @param r - Red part of the color value (0 - 255)
     * @param g - Green part of the color value (0 - 255)
     * @param b - Blue part of the color value (0 - 255)
     */
    void setRGB(uint8_t r, uint8_t g, uint8_t b);
    
    /**
     * Sets a LED color for next operations using RGBW format until another color
     * set operation overwrites it.
     * @param r - Red part of the color value (0 - 255)
     * @param g - Green part of the color value (0 - 255)
     * @param b - Blue part of the color value (0 - 255)
     * @param w - White LED level (0 - 255)
     */
    void setRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w);

    /**
     * Sets a LED color for next operations using HSV format until another color
     * set operation overwrites it.
     * @param h - Hue part of the color value (0 - 359)
     * @param s - Saturation part of the color value (0 - 255)
     * @param v - Value part of the color value (0 - 255)
     */
    void setHSV(uint16_t h, uint8_t s, uint8_t v);

    /**
     * Assign the previously set color value to a single LED.
     * @param index - Index of of the LED to set. Index starts with 0
     */
    void setLED(uint8_t index);

    /**
     * Clears a single LED by setting its color to RGB(0,0,0).
     * Internally it simply sends setRGB(0,0,0) and setLED(index).
     * @param index - Index of of the LED to clear. Index starts with 0
     */
    void clearLED(uint8_t index);

    /**
     * Assign the previously set color value to a all LEDs.
     */
    void setAll();

    /**
     * Clears all LEDs by setting their color to RGB(0,0,0).
     * Internally it simply sends setRGB(0,0,0) and setAll().
     */
    void clearAll();

    /**
     * Assign the previously set color value to a range of LEDs.
     * @param start - Index of the first LED in the range to set. Index starts with 0
     * @param end - Index of the last LED in the range to set
     */
    void setRange(uint8_t start, uint8_t end);

    /**
     * Creates a rainbow effect in a range.
     * @param h - Hue part of the color value (0 - 359)
     * @param s - Saturation part of the color value (0 - 255)
     * @param v - Value part of the color value (0 - 255)
     * @param start - Index of the first LED in the range to set. Index starts with 0
     * @param end - Index of the last LED in the range to set
     * @param step - Step value to increment between 2 LEDs. Recommended values 2 - 20
     */
    void setRainbow(uint16_t h, uint8_t s, uint8_t v, uint8_t start, uint8_t end, uint8_t step);

    /**
     * Creates a gradient from one color to another. start and end index can have negative
     * values to make a gradient starting outside the visible area showing only it's 
     * currently visible part considering the intermediate color values.
     * @param start - Index of the first LED in the range. Can be negative.
     * @param end  - Index of the last LED in the range. Can be greater than the number of LEDs
     * @param from - RGB value of the start color
     * @param to - RGB value of the end color
     */
    void setGradient(int start, int end, uint8_t from[3], uint8_t to[3]);

    /**
     * Shifts up the color values of the LEDs in a range.
     * @param start - Index of the first LED in the range. Index starts with 0
     * @param end - Index of the last LED in the range
     * @param count - Number of LEDs/steps to shift up
     */
    void shiftUp(uint8_t start, uint8_t end, uint8_t count);

    /**
     * Shifts down the color values of the LEDs in a range.
     * @param start - Index of the first LED in the range. Index starts with 0
     * @param end - Index of the last LED in the range
     * @param count - Number of LEDs/steps to shift down
     */
    void shiftDown(uint8_t start, uint8_t end, uint8_t count);

    /**
     * Copies a color value of a LED to another one.
     * @param from - Index of the LED to copy from
     * @param to - Index of the LED to copy to
     */
    void copyLED(uint8_t from, uint8_t to);

    /**
     * Copies the whole range several times in a row. 
     * @param start - Index of the first LED in the range. Index starts with 0
     * @param end - Index of the last LED in the range
     * @param count - Number of copy operation
     */
    void repeat(uint8_t start, uint8_t end, uint8_t count);

    /**
     * Shows the changes previously made by sending all values to the LEDs.
     */
    void show();

private:
    void sendData(uint8_t* buffer, uint8_t length);

private:
    uint8_t _csPin;
    uint8_t _resetPin;
    uint8_t _lastIndex;
};

#endif //DD_BOOSTER_LIBRARY_DDBOOSTER_H
