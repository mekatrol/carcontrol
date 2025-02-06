#include <Arduino.h>
#include <TFT_eSPI.h>

#include "Touch.h"
#include "Arc.h"
#include "VeDirect.h"
#include "CircularBuffer.h"
#include "StateData.h"
#include "TextString.h"

static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 240;

#define RXD1 18
#define TXD1 17
#define TEXT_BUFFER_SIZE 10

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

Touch touch = Touch(/* sda */ 6, /* scl */ 7, /* rst */ 13, /* irq*/ 5);

Arc capacityStatus = Arc(118, 108, 30, 330, 0, 100, 120, 120, CW);

VeDirect shunt(RXD1, TXD1, 19200);

CircularBuffer rxBuffer = CircularBuffer();

StateDataStruct stateData;

TextString soc = TextString(120, 45, 3, "%0.1f%%");
TextString amps = TextString(120, 100, 4, "%0.2fA");
TextString volts = TextString(120, 165, 4, "%0.2fV");

void setup() {
  // For debugging
  // Serial.begin(115200);

  memset(&stateData, 0, sizeof(StateDataStruct));

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);

  touch.start();
}

bool redraw = true;

void loop() {
  while (Serial1.available()) {
    // Read a character
    char ch = Serial1.read();
    rxBuffer.AddChar(ch);
  }

  // Redraw if data was updated
  bool dataUpdated = shunt.ProcessMessageBuffer(&rxBuffer, &stateData);

  // Get current millis
  long now_millis = millis();

  // Debounce any touch changes
  touch.debounce(now_millis);

  // Only redraw if is a 'new' touch
  if (touch.getStateChanged() && touch.getState()) {
    redraw = true;
  }

  if (redraw || dataUpdated) {
    redraw = false;

    volts.Render(&tft, stateData.voltage, TFT_WHITE, TFT_BLACK);
    amps.Render(&tft, stateData.current, TFT_WHITE, TFT_BLACK);
    soc.Render(&tft, stateData.stateOfCharge, TFT_WHITE, TFT_BLACK);

    uint32_t color = TFT_BLACK;

    if (stateData.stateOfCharge >= 60) {
      color = TFT_GREENYELLOW;
    } else if (stateData.stateOfCharge >= 30) {
      color = TFT_ORANGE;
    } else {
      color = TFT_RED;
    }

    capacityStatus.render(&tft, stateData.stateOfCharge, color, TFT_BLACK, TFT_DARKGREY);
  }

  delay(5);
}
