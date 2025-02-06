#include <Arduino.h>
#include <TFT_eSPI.h>

#define TEXT_MAX_LEN 2048

#ifndef __TEXTSTRING_H__
#define __TEXTSTRING_H__

class TextString
{
public:
    TextString(uint16_t x, uint16_t y, uint8_t textSize, const char *format);
    void Render(TFT_eSPI *tft, float value, uint32_t foreColor, uint32_t backColor);

private:
    uint16_t x;
    uint16_t y;
    uint8_t textSize;
    const char *format;
    char prev[TEXT_MAX_LEN + 1];
    char curr[TEXT_MAX_LEN + 1];
};

#endif