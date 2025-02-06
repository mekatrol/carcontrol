#include <Arduino.h>

#include "VeDirect.h"

#define VALUE_BUFFER_SIZE 100

VeDirect::VeDirect(uint8_t rxPin, uint8_t txPin, uint16_t baudRate) {
  Serial1.begin(baudRate, SERIAL_8N1, rxPin, txPin);

  this->commandStarted = false;
  this->ResetCommand();
}

void VeDirect::ResetCommand() {
  this->commandIndex = 0;
  this->command[0] = '\0';
}

bool VeDirect::ProcessCommand(StateDataStruct *stateData) {
  Serial.println(this->command);
  
  char commandCode[VALUE_BUFFER_SIZE + 2];
  char commandValue[VALUE_BUFFER_SIZE + 2];

  uint16_t i = 0;
  uint16_t j = 0;  

  // Get command code
  while (j < this->commandIndex && !isWhitespace(this->command[j]) && i < VALUE_BUFFER_SIZE) {
    commandCode[i++] = this->command[j++];
    commandCode[i] = '\0';
  }

  // Skip whitespace
  while (j < this->commandIndex && isWhitespace(this->command[j]) && i < VALUE_BUFFER_SIZE) {
    j++;
  }

  // Get value
  i = 0;
  while (j < this->commandIndex && !isWhitespace(this->command[j]) && i < VALUE_BUFFER_SIZE) {
    commandValue[i++] = this->command[j++];
    commandValue[i] = '\0';
  }

  if (strcmp(commandCode, "V") == 0) {
    stateData->voltage = atof(commandValue) / 1000.0f;
    return true;
  }

  if (strcmp(commandCode, "I") == 0) {
    stateData->current = atof(commandValue)  / 1000.0f;
    return true;
  }

  if (strcmp(commandCode, "SOC") == 0) {
    stateData->stateOfCharge = atof(commandValue) / 10.0f;
    return true;
  }

  return false;
}

bool VeDirect::ProcessMessageBuffer(CircularBuffer *buffer, StateDataStruct *stateData) {
  while (buffer->HasChar()) {
    char ch = buffer->GetChar();

    // New line?
    if (ch == '\n') {
      // Toggle command started
      this->commandStarted = !this->commandStarted;

      bool dataUpdated = false;

      // Is this the end of a command?
      if (!this->commandStarted) {
        dataUpdated = this->ProcessCommand(stateData);
      }

      this->ResetCommand();

      // Data was updated if true
      return dataUpdated;
    }

    this->command[this->commandIndex++] = ch;
    this->command[this->commandIndex] = '\0';

    // About to overrun command buffer so reset
    if (this->commandIndex >= COMMAND_BUFFER_SIZE - 2) {
      this->commandStarted = false;
      this->ResetCommand();
    }

    return false;
  }
}