#ifndef CONFIG_H
#define CONFIG_H

#include <NeoPixelBus.h>

// ==================== Display Configuration ====================
const int PixelCount = 18 * 16;
const int DisplayWidth = 16;
const int DisplayHeight = 18;
const uint8_t PixelPin = 15;

#define colorSaturation 80
#define pixelBrightness (colorSaturation / 255.0)

// ==================== WiFi Configuration ====================
const char *ssid = "xxx";
const char *password = "xxx";
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 0;

// ==================== Weather API Configuration ====================
const char *weatherURL = "http://api.open-meteo.com/v1/forecast?latitude=48.6955&longitude=9.3168&current=temperature_2m&hourly=temperature_2m&timezone=Europe%2FBerlin&forecast_days=1&daily=sunrise,sunset";

// ==================== Color Definitions ====================
RgbColor black(0);
RgbColor colorTop(0, 30, 77);
RgbColor colorMiddle(77, 5, 5);
RgbColor colorBottom(0, 77, 5);
RgbColor colorTemp(0x33, 0xFF, 0x22);
RgbColor colorLine(0xAA, 0xAA, 0x00);
RgbColor colorTimeDot(0x16, 0x16, 0x00);
RgbColor colorTempWarm(0xDD, 0x66, 0x00);
RgbColor colorTempCold(0x22, 0x66, 0xff);

// ==================== Font Data ====================
// Font bitmap for numbers 0-9, colon (:), and minus (-)
// Each number is 3x5 pixels
// Stored in PROGMEM to save RAM (720 bytes)
const long PROGMEM numbers[] = {
    // 0
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff,

    // 1
    0x00000000, 0x00000000, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,

    // 2
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00000000, 0x00000000,
    0x00ffffff, 0x00ffffff, 0x00ffffff,

    // 3
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff,

    // 4
    0x00ffffff, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,

    // 5
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00000000, 0x00000000,
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff,

    // 6
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00000000, 0x00000000,
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff,

    // 7
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,

    // 8
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff,

    // 9
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00000000, 0x00000000, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff,

    // Colon (:)
    0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00ffffff, 0x00000000,
    0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00ffffff, 0x00000000,
    0x00000000, 0x00000000, 0x00000000,

    // Minus (-)
    0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000,
    0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000};

#endif // CONFIG_H
