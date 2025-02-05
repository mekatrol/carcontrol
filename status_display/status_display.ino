#include <Arduino.h>
#include <TFT_eSPI.h>

#include "touch.h"

// Switch position and size
#define FRAME_X 0
#define FRAME_Y 0
#define FRAME_W 240
#define FRAME_H 240

// Green zone size
#define BUTTON_X 80
#define BUTTON_Y 80
#define BUTTON_W 80
#define BUTTON_H 80

static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 240;

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

Touch touch(/* sda */ 6, /* scl */ 7, /* rst */ 13, /* irq*/ 5);

bool button_touched = false;

void drawFrame() {
  tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, TFT_BLACK);
}

void setup() {
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  touch.start();
}

void loop() {
  // Get current millis
  long now_millis = millis();

  // Debounce any touch changes
  touch.debounce(now_millis);

  // Only toggle if is a 'new' touch
  if (touch.getStateChanged()) {
    // Touch point must be within bounds of button
    int x = touch.getX();
    int y = touch.getY();

    if ((x > BUTTON_X) && (x < (BUTTON_X + BUTTON_W))) {
      if ((y > BUTTON_Y) && (y < (BUTTON_Y + BUTTON_H))) {
        button_touched = !button_touched;
      }
    }
  }

  // tft.fillScreen(TFT_BLACK);
  tft.fillRect(BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H, button_touched ? TFT_GREEN : TFT_DARKGREY);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(button_touched ? "ON" : "OFF", BUTTON_X + (BUTTON_W / 2) + 1, BUTTON_Y + (BUTTON_H / 2));

  delay(100);
}
