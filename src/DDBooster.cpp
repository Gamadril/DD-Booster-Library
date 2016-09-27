/*
 * DDBoster.cpp - Library to control the Digi-Dot-Booster using a high-level API
 *
 * https://github.com/Gamadril/DD-Booster-Library
 * MIT License
 */

#include "DDBooster.h"

#define BOOSTER_CMD_DELAY    500
#define BOOSTER_LED_DELAY    30

#define BOOSTER_SETRGB       0xA1
#define BOOSTER_SETRGBW      0xA2
#define BOOSTER_SETHSV       0xA3
#define BOOSTER_SETLED       0xA4
#define BOOSTER_SETALL       0xA5
#define BOOSTER_SETRANGE     0xA6
#define BOOSTER_SETRAINBOW   0xA7
#define BOOSTER_GRADIENT     0xA8

#define BOOSTER_INIT         0xB1
#define BOOSTER_SHOW         0xB2
#define BOOSTER_SHIFTUP      0xB3
#define BOOSTER_SHIFTDOWN    0xB4
#define BOOSTER_COPYLED      0xB5
#define BOOSTER_REPEAT       0xB6

#define BOOSTER_RGBORDER     0xC1

DDBooster::DDBooster() : _csPin(SS), _resetPin(0xFF), _lastIndex(0) {
    // initialize SPI at 12MHz, MSB first, mode 0
    SPI.begin();
    SPI.beginTransaction(SPISettings(12000000, MSBFIRST, SPI_MODE0));
}

void DDBooster::configurePins(uint8_t csPin, uint8_t resetPin) {
    _csPin = csPin;
    _resetPin = resetPin;

    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
    pinMode(_resetPin, OUTPUT);
}

void DDBooster::init(uint16_t ledCount, LedType ledType, LedColorOrder colorOrder) {
    // DD Booster expects the number of LEDs to be an even value (rounded up).
    // 256 is defined as 0 to fit in one byte.
    if (ledCount > 256) {
        ledCount = 256;
    }

    _lastIndex = ledCount - 1;

    uint8_t buffer[4];
    buffer[0] = BOOSTER_INIT;
    buffer[1] = ledCount + (ledCount & 1);
    buffer[2] = ledType;
    sendData(buffer, 3);

    if (ledType == LED_RGB && colorOrder != ORDER_GRB) {
        buffer[0] = BOOSTER_RGBORDER;
        buffer[1] = 3;
        buffer[2] = 2;
        buffer[3] = 1;
        sendData(buffer, 4);
    }

    // a delay after init is not documented, but seems to be necessary
    delay(20);
}

void DDBooster::reset() {
    if (_resetPin < 0xFF) {
        digitalWrite(_resetPin, LOW);
        delay(100);
        digitalWrite(_resetPin, HIGH);
        delay(100);
    }
}

void DDBooster::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t cmd[4];
    cmd[0] = BOOSTER_SETRGB;
    cmd[1] = r;
    cmd[2] = g;
    cmd[3] = b;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::setRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    uint8_t cmd[5];
    cmd[0] = BOOSTER_SETRGBW;
    cmd[1] = r;
    cmd[2] = g;
    cmd[3] = b;
    cmd[4] = w;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::setHSV(uint16_t h, uint8_t s, uint8_t v) {
    if (h > 359) {
        h = 359;
    }
    uint8_t cmd[5];
    cmd[0] = BOOSTER_SETHSV;
    cmd[1] = h & 0xFF;
    cmd[2] = h >> 8;
    cmd[3] = s;
    cmd[4] = v;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::setLED(uint8_t index) {
    if (index > _lastIndex) {
        return;
    }
    uint8_t cmd[2];
    cmd[0] = BOOSTER_SETLED;
    cmd[1] = index;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::clearLED(uint8_t index) {
    if (index > _lastIndex) {
        return;
    }
    // optimization by sending two commands in one transaction    
    uint8_t cmd[6];
    cmd[0] = BOOSTER_SETRGB;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = BOOSTER_SETLED;
    cmd[5] = index;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::setAll() {
    uint8_t cmd[1];
    cmd[0] = BOOSTER_SETALL;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::clearAll() {
    // optimization by sending two commands in one transaction    
    uint8_t cmd[5];
    cmd[0] = BOOSTER_SETRGB;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = BOOSTER_SETALL;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::setRange(uint8_t start, uint8_t end) {
    if (start > end || end > _lastIndex || start > _lastIndex) {
        return;
    }
    uint8_t cmd[3];
    cmd[0] = BOOSTER_SETRANGE;
    cmd[1] = start;
    cmd[2] = end;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::setRainbow(uint16_t h, uint8_t s, uint8_t v, uint8_t start, uint8_t end, uint8_t step) {
    if (start > end || end > _lastIndex || start > _lastIndex) {
        return;
    }
    if (h > 359) {
        h = 359;
    }
    uint8_t cmd[8];
    cmd[0] = BOOSTER_SETRAINBOW;
    cmd[1] = h & 0xFF;
    cmd[2] = h >> 8;
    cmd[3] = s;
    cmd[4] = v;
    cmd[5] = start;
    cmd[6] = end;
    cmd[7] = step;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::setGradient(int start, int end, uint8_t from[3], uint8_t to[3]) {
    if (start > end || start > _lastIndex) {
        return;
    }

    uint8_t steps = end - start;
    if (steps == 0) {
        setRGB(from[0], from[1], from[2]);
        return;
    }

    uint8_t s = 0, e = steps;
    if (start < 0) {
        s = 0 - start;
    }
    if (end > _lastIndex) {
        e -= (end - _lastIndex);
    }

    // optimized setRGB(r,g,b) and setLED(start + i) with one transaction and shared memory
    uint8_t cmd[6];
    cmd[0] = BOOSTER_SETRGB;
    cmd[4] = BOOSTER_SETLED;
    for (; s <= e; s++) {
        cmd[1] = from[0] + (to[0] - from[0]) * s / steps;
        cmd[2] = from[1] + (to[1] - from[1]) * s / steps;
        cmd[3] = from[2] + (to[2] - from[2]) * s / steps;
        cmd[5] = start + s;
        sendData(cmd, sizeof (cmd));
    }
}

void DDBooster::shiftUp(uint8_t start, uint8_t end, uint8_t count) {
    if (start > end || end > _lastIndex || start > _lastIndex) {
        return;
    }
    uint8_t cmd[4];
    cmd[0] = BOOSTER_SHIFTUP;
    cmd[1] = start;
    cmd[2] = end;
    cmd[3] = count;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::shiftDown(uint8_t start, uint8_t end, uint8_t count) {
    if (start > end || end > _lastIndex || start > _lastIndex) {
        return;
    }
    uint8_t cmd[4];
    cmd[0] = BOOSTER_SHIFTDOWN;
    cmd[1] = start;
    cmd[2] = end;
    cmd[3] = count;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::copyLED(uint8_t from, uint8_t to) {
    if (from > _lastIndex || to > _lastIndex) {
        return;
    }
    uint8_t cmd[3];
    cmd[0] = BOOSTER_COPYLED;
    cmd[1] = from;
    cmd[2] = to;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::repeat(uint8_t start, uint8_t end, uint8_t count) {
    if (start > end || end > _lastIndex || start > _lastIndex) {
        return;
    }
    uint8_t cmd[4];
    cmd[0] = BOOSTER_REPEAT;
    cmd[1] = start;
    cmd[2] = end;
    cmd[3] = count;
    sendData(cmd, sizeof (cmd));
}

void DDBooster::show() {
    uint8_t cmd[1];
    cmd[0] = BOOSTER_SHOW;
    sendData(cmd, sizeof (cmd));
    delayMicroseconds(BOOSTER_LED_DELAY * (_lastIndex + 1));
}

void DDBooster::sendData(uint8_t *buffer, uint8_t length) {
    digitalWrite(_csPin, LOW);
    for (int i = 0; i < length; i++) {
        SPI.transfer(buffer[i]);
    }
    digitalWrite(_csPin, HIGH);
    delayMicroseconds(BOOSTER_CMD_DELAY);
}
