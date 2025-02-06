#include <Arduino.h>
#include <TFT_eSPI.h>

#include "Touch.h"
#include "Arc.h"
#include "VeDirect.h"
#include "CircularBuffer.h"
#include "StateData.h"

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

char voltage_text_buffer[TEXT_BUFFER_SIZE + 1];
char voltage_text_buffer_prev[TEXT_BUFFER_SIZE + 1];

char current_text_buffer[TEXT_BUFFER_SIZE + 1];
char current_text_buffer_prev[TEXT_BUFFER_SIZE + 1];

char soc_text_buffer[TEXT_BUFFER_SIZE + 1];
char soc_text_buffer_prev[TEXT_BUFFER_SIZE + 1];

void setup() {
  // For debugging
  Serial.begin(115200);

  memset(&stateData, 0, sizeof(StateDataStruct));

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(4);
  tft.setTextDatum(MC_DATUM);

  touch.start();

  sprintf(voltage_text_buffer, "%0.2fV", 0);
  sprintf(voltage_text_buffer_prev, "%0.2fV", 0);
  sprintf(current_text_buffer, "%0.2fA", 0);
  sprintf(current_text_buffer_prev, "%0.2fA", 0);
  sprintf(soc_text_buffer, "%0.1f%%", 0);
  sprintf(soc_text_buffer_prev, "%0.1f%%", 0);
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

    sprintf(voltage_text_buffer, "%0.2fV", stateData.voltage);
    sprintf(current_text_buffer, "%0.2fA", stateData.current);
    sprintf(soc_text_buffer, "%0.1f%%", stateData.stateOfCharge);

    tft.setTextColor(TFT_BLACK);
    tft.drawString(voltage_text_buffer_prev, 120, 160);
    tft.drawString(current_text_buffer_prev, 120, 100);
    tft.drawString(soc_text_buffer_prev, 120, 40);

    tft.setTextColor(TFT_WHITE);
    tft.drawString(voltage_text_buffer, 120, 160);
    tft.drawString(current_text_buffer, 120, 100);
    tft.drawString(soc_text_buffer, 120, 40);

    memcpy(voltage_text_buffer_prev, voltage_text_buffer, sizeof(voltage_text_buffer));
    memcpy(current_text_buffer_prev, current_text_buffer, sizeof(current_text_buffer));
    memcpy(soc_text_buffer_prev, soc_text_buffer, sizeof(soc_text_buffer));

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

  delay(10);
}
