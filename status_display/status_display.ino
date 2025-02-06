#include <Arduino.h>
#include <TFT_eSPI.h>

#include "touch.h"
#include "arc.h"

static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 240;

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

Touch touch(/* sda */ 6, /* scl */ 7, /* rst */ 13, /* irq*/ 5);

Arc capacityStatus(118, 108, 30, 330, 0, 100, 120, 120, CW);

int pct = 0;
char text_buffer[10];
char old_text_buffer[10];

void setup() {
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(4);
  tft.setTextDatum(MC_DATUM);

  touch.start();

  sprintf(text_buffer, "%3d%%", 0);
  sprintf(old_text_buffer, "%3d%%", 0);
}

bool redraw = true;
int dir = 1;

void loop() {
  // Get current millis
  long now_millis = millis();

  // Debounce any touch changes
  touch.debounce(now_millis);

  // Only toggle if is a 'new' touch
  if (touch.getStateChanged() && touch.getState()) {
    pct += 10;

    if (pct > 100) {
      pct = 0;
    }

    sprintf(text_buffer, "%3d%%", pct);
    redraw = true;
  }

  pct += (1 * dir);

  if (pct > 100) {
    pct = 100;
    dir *= -1;
  } else if (pct < 0) {
    pct = 0;
    dir *= -1;
  }
  sprintf(text_buffer, "%3d%%", pct);
  redraw = true;

  if (redraw) {
    redraw = false;

    tft.setTextColor(TFT_BLACK);
    tft.drawString(old_text_buffer, 120, 120);

    tft.setTextColor(TFT_WHITE);
    tft.drawString(text_buffer, 120, 120);

    memcpy(old_text_buffer, text_buffer, sizeof(text_buffer));

    uint32_t color = TFT_BLACK;

    if (pct >= 60) {
      color = TFT_GREENYELLOW;
    } else if (pct >= 30) {
      color = TFT_ORANGE;
    } else {
      color = TFT_RED;
    }

    capacityStatus.render(&tft, pct, color, TFT_BLACK, TFT_DARKGREY);
  }

  delay(10);
}
