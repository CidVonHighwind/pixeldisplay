#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "BluetoothSerial.h"
#include "time.h"
#include "config.h"
#include "main.h"

// ==================== Global Variables ====================
int color[3] = {255, 255, 0};
int colorMode;
int brightness = 100;

Preferences preferences;
BluetoothSerial SerialBT;
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);

int currentTemperature = -99;
int temperatures[] = {0, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11};

bool wifiConnected = false;
int loadingIndex = 0;
int loadingState = 1;
bool loadingAnimation = true;
unsigned long lastDraw;

// ==================== Utility Functions ====================

void GetMacAddress()
{
    Serial.println();
    Serial.print("ESP Board MAC Address:  ");
    Serial.println(WiFi.macAddress());
}

int readValue()
{
    int readValue = 0;
    while (SerialBT.available())
    {
        int value = SerialBT.read();
        if (value < '0' || '9' < value)
            continue;
        readValue *= 10;
        readValue += value - '0';
    }
    return readValue;
}

long readHex()
{
    long hexValue = 0;
    while (SerialBT.available())
    {
        int value = SerialBT.read();
        if ('0' <= value && value <= '9')
        {
            hexValue *= 16;
            hexValue += value - '0';
        }
        if ('A' <= value && value <= 'F')
        {
            hexValue *= 16;
            hexValue += value - 'A' + 10;
        }
        if ('a' <= value && value <= 'f')
        {
            hexValue *= 16;
            hexValue += value - 'a' + 10;
        }
    }
    return hexValue;
}

void readBluetooth()
{
    if (SerialBT.available())
    {
        char mode = SerialBT.read();

        if (mode == 'b')
        {
            brightness = readValue();
            if (brightness > 100)
                brightness = 100;
            if (brightness < 0)
                brightness = 0;

            // Save brightness to NVS
            preferences.begin("display", false);
            preferences.putInt("brightness", brightness);
            preferences.end();
        }
        else if (mode == 'c')
        {
            char row = SerialBT.read();
            long receivedValue = readHex();
            RgbColor newColor = RgbColor(
                (byte)((receivedValue >> 16) & 0xFF),
                (byte)((receivedValue >> 8) & 0xFF),
                (byte)((receivedValue >> 0) & 0xFF));

            if (row == '0')
                colorTop = newColor;
            else if (row == '1')
                colorMiddle = newColor;
            else if (row == '2')
                colorBottom = newColor;
            else if (row == '3')
                colorTempWarm = newColor;
            else if (row == '4')
                colorTempCold = newColor;
            else if (row == '5')
                colorTimeDot = newColor;
        }
    }
}

uint16_t getLedIndex(int x, int y)
{
    if (x < 0 || DisplayWidth <= x || y < 0 || DisplayHeight <= y)
        return -1;

    if (x % 2 == 0)
        y = 17 - y;
    return x * 18 + y;
}

void ClearDisplay()
{
    for (int x = 0; x < PixelCount; x++)
        strip.SetPixelColor(x, black);
}

void CycleColor()
{
    if (colorMode % 2 == 0)
    {
        int index = (colorMode / 2) % 3;
        color[index] -= 1;
        if (color[index] <= 0)
        {
            color[index] = 0;
            colorMode++;
        }
    }
    else
    {
        int index = (colorMode / 2 + 2) % 3;
        color[index] += 1;
        if (color[index] >= 255)
        {
            color[index] = 255;
            colorMode++;
        }
    }
}

// ==================== Rendering Functions ====================

void DrawPixel(int posX, int posY, RgbColor color)
{
    uint16_t ledIndex = getLedIndex(posX, posY);
    if (ledIndex == (uint16_t)-1)
        return;
    float brightnessScale = brightness / 100.0;
    strip.SetPixelColor(ledIndex, RgbColor(
                                      color.R * brightnessScale,
                                      color.G * brightnessScale,
                                      color.B * brightnessScale));
}

void DrawCharacter(int posX, int posY, int index, RgbColor color)
{
    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            int pictureIndex = x + (y + index * 5) * 3;
            long pixelData = pgm_read_dword(&numbers[pictureIndex]);

            if (pixelData > 0)
                DrawPixel(posX + x, posY + y, color);
            else
                DrawPixel(posX + x, posY + y, black);
        }
    }
}

void DrawLoadingAnimation()
{
    float brightnessScale = brightness / 100.0;
    strip.SetPixelColor(loadingIndex, RgbColor(
                                          loadingState * color[0] * brightnessScale,
                                          loadingState * color[1] * brightnessScale,
                                          loadingState * color[2] * brightnessScale));
    strip.Show();

    loadingIndex++;
    if (loadingIndex >= PixelCount)
    {
        loadingIndex = 0;
        loadingState = 1 - loadingState;

        if (loadingState == 1 && wifiConnected)
        {
            loadingAnimation = false;
            ClearDisplay();
            strip.Show();
        }
    }
}

void DrawWeather()
{
    int posX = 10;
    DrawCharacter(posX, 1, abs(currentTemperature) % 10, colorTemp);
    posX -= 4;

    if (abs(currentTemperature) > 9)
    {
        DrawCharacter(posX, 1, abs(currentTemperature) / 10, colorTemp);
        posX -= 4;
    }

    if (currentTemperature < 0)
        DrawCharacter(posX, 1, 11, colorTemp);

    DrawPixel(14, 1, colorTemp);

    int currentHour = 0;
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
        currentHour = timeinfo.tm_hour;

    for (int i = 0; i < 12; i++)
    {
        if (i * 2 <= currentHour && currentHour < i * 2 + 2)
            DrawPixel(2 + i, 12, colorTimeDot);
        else
            DrawPixel(2 + i, 12, colorLine);
    }

    for (int i = 0; i < 12; i++)
    {
        if (i * 2 <= currentHour && currentHour < i * 2 + 2)
            continue;

        int currentTemp = temperatures[i];
        int position = 12;
        int direction = -1;
        RgbColor color = colorTempWarm;

        if (currentTemp < 0)
        {
            direction = 1;
            currentTemp = abs(currentTemp);
            color = colorTempCold;
        }

        while (currentTemp != 0)
        {
            position += direction;

            if (currentTemp % 2 == 1)
            {
                DrawPixel(2 + i, position, color);
            }

            currentTemp /= 2;
        }
    }
}

void DrawClock()
{
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        int hours = timeinfo.tm_hour;
        int minutes = timeinfo.tm_min;
        int seconds = timeinfo.tm_sec;
        int day = timeinfo.tm_mday;
        int month = timeinfo.tm_mon + 1;

        DrawCharacter(0, 1, hours / 10, colorTop);
        DrawCharacter(4, 1, hours % 10, colorTop);

        DrawCharacter(9, 1, minutes / 10, colorTop);
        DrawCharacter(13, 1, minutes % 10, colorTop);

        DrawCharacter(9, 7, seconds / 10, colorMiddle);
        DrawCharacter(13, 7, seconds % 10, colorMiddle);

        DrawCharacter(0, 13, day / 10, colorBottom);
        DrawCharacter(4, 13, day % 10, colorBottom);

        DrawCharacter(9, 13, month / 10, colorBottom);
        DrawCharacter(13, 13, month % 10, colorBottom);
    }
}

void UpdateDisplay(int brt)
{
    ClearDisplay();
    DrawWeather();
    strip.Show();
}

// ==================== Task Functions ====================

void TaskGetWifiTime(void *pvParameters)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    wifiConnected = true;
    Serial.println(WiFi.localIP());

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    getLocalTime(&timeinfo);

    while (true)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Wi-Fi disconnected, reconnecting...");
            WiFi.reconnect();
        }

        HTTPClient http;
        http.begin(weatherURL);
        http.setTimeout(5000);
        int httpCode = http.GET();

        if (httpCode > 0)
        {
            String payload = http.getString();

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (error)
            {
                http.end();
                vTaskDelay(5000 / portTICK_PERIOD_MS);
                continue;
            }

            double cTemperature = doc["current"]["temperature_2m"];
            currentTemperature = round(cTemperature);

            for (int i = 0; i < 12; i++)
            {
                double temp0 = doc["hourly"]["temperature_2m"][i * 2];
                double temp1 = doc["hourly"]["temperature_2m"][i * 2 + 1];
                temperatures[i] = round((temp0 + temp1) / 2) - currentTemperature;
            }

            http.end();
            vTaskDelay(60000 / portTICK_PERIOD_MS);
        }
        else
        {
            http.end();
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }
}

void MainLoop(void *pvParameters)
{
    while (true)
    {
        CycleColor();

        if (loadingAnimation)
        {
            DrawLoadingAnimation();
            vTaskDelay(1 / portTICK_PERIOD_MS);
            continue;
        }

        readBluetooth();

        unsigned long targetTime = lastDraw + (1000000 / 60);
        unsigned long timeDelta = targetTime - micros();

        if (0 < timeDelta && timeDelta < 1000000l / 60)
            vTaskDelay((timeDelta / 1000) / portTICK_PERIOD_MS);

        lastDraw = micros();
        UpdateDisplay(0);
    }
}

// ==================== Arduino Entry Points ====================

void setup()
{
    Serial.begin(115200);
    GetMacAddress();

    // Load saved brightness from NVS
    preferences.begin("display", true);
    brightness = preferences.getInt("brightness", 100);
    preferences.end();

    strip.Begin();
    strip.Show();

    SerialBT.begin("ESP32Color");

    xTaskCreate(TaskGetWifiTime, "WifiTask", 4096, NULL, 1, NULL);
    xTaskCreate(MainLoop, "MainTask", 4096, NULL, 1, NULL);

    Serial.println("Finished setup");
}

void loop()
{
    // Main loop is empty as tasks run in parallel via FreeRTOS
}
