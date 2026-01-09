#ifndef MAIN_H
#define MAIN_H

#include <NeoPixelBus.h>

// ==================== Utility Functions ====================
void GetMacAddress();
int readValue();
long readHex();
void readBluetooth();

// ==================== LED Display Functions ====================
uint16_t getLedIndex(int x, int y);
void ClearDisplay();
void CycleColor();
void DrawPixel(int posX, int posY, RgbColor color);

// ==================== Rendering Functions ====================
void DrawCharacter(int posX, int posY, int index, RgbColor color);
void DrawLoadingAnimation();
void DrawWeather();
void DrawClock();
void UpdateDisplay(int brt);

// ==================== Task Functions ====================
void MainLoop(void *pvParameters);
void TaskGetWifiTime(void *pvParameters);

#endif // MAIN_H
