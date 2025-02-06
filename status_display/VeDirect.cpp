#include <Arduino.h>

#include "VeDirect.h"

#define VALUE_BUFFER_SIZE 100

VeDirect::VeDirect(uint8_t rxPin, uint8_t txPin, uint16_t baudRate) {
  Serial1.begin(baudRate, SERIAL_8N1, rxPin, txPin);
  this->ResetCommand();
}

void VeDirect::ResetCommand() {
  this->commandIndex = 0;
  this->commandLine[0] = '\0';
}

bool VeDirect::ProcessCommand(StateDataStruct *stateData) {
  char commandCode[VALUE_BUFFER_SIZE + 2];
  char commandValue[VALUE_BUFFER_SIZE + 2];

  uint16_t i = 0;
  uint16_t j = 0;

  // Get command code
  while (j < this->commandIndex && !isWhitespace(this->commandLine[j]) && i < VALUE_BUFFER_SIZE) {
    commandCode[i++] = this->commandLine[j++];
    commandCode[i] = '\0';
  }

  // Skip whitespace
  while (j < this->commandIndex && isWhitespace(this->commandLine[j]) && i < VALUE_BUFFER_SIZE) {
    j++;
  }

  // Get value
  i = 0;
  while (j < this->commandIndex && !isWhitespace(this->commandLine[j]) && i < VALUE_BUFFER_SIZE) {
    commandValue[i++] = this->commandLine[j++];
    commandValue[i] = '\0';
  }

  if (strcmp(commandCode, "V") == 0) {
    stateData->voltage = atof(commandValue) / 1000.0f;
    return true;
  }

  if (strcmp(commandCode, "I") == 0) {
    stateData->current = atof(commandValue) / 1000.0f;
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

    // A '\r' character flags the start of the next message so process any previous command
    if (ch == '\r') {
      if (this->commandIndex > 0) {
        // Return if a message is processed to update the display with any new value
        return this->ProcessCommand(stateData);
      }
    }

    // A '\n' follows the '\r' for a new message so reset command line state when received
    else if (ch == '\n') {
      this->ResetCommand();
    }

    // Append the character to the command line
    else {
      this->commandLine[this->commandIndex++] = ch;
      this->commandLine[this->commandIndex] = '\0';

      // About to overrun command buffer so reset
      if (this->commandIndex >= COMMAND_BUFFER_SIZE - 2) {
        this->ResetCommand();
      }
    }
  }

  return false;
}