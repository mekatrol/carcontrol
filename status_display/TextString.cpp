#include "TextString.h"

TextString::TextString(uint16_t x, uint16_t y, uint8_t textSize, const char *format) : x(x), y(y), textSize(textSize), format(format)
{
    // Initialise empty strings
    sprintf(this->prev, this->format, 0);
    sprintf(this->curr, this->format, 0);
}

void TextString::Render(TFT_eSPI *tft, float value, uint32_t foreColor, uint32_t backColor)
{
    // Convert value to text
    sprintf(this->curr, this->format, value);

    // Set text size
    tft->setTextSize(this->textSize);

    // Blank out prev text
    tft->setTextColor(backColor);
    tft->drawString(this->prev, this->x, this->y);

    // Render current text
    tft->setTextColor(foreColor);
    tft->drawString(this->curr, this->x, this->y);

    // Copy current to previous
    memcpy(this->prev, this->curr, sizeof(this->curr));
}