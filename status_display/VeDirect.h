#include <Arduino.h>

#include "CircularBuffer.h"
#include "StateData.h"

#define COMMAND_BUFFER_SIZE 2048

#ifndef __VEDIRECT_H__
#define __VEDIRECT_H__

class VeDirect {
  public:
    VeDirect(uint8_t rxPin, uint8_t txPin, uint16_t baudRate);
    bool ProcessMessageBuffer(CircularBuffer *buffer, StateDataStruct *stateData);
  private:
    void ResetCommand();
    bool ProcessCommand(StateDataStruct *stateData);

    char commandLine[COMMAND_BUFFER_SIZE + 1];
    uint16_t commandIndex;
};

#endif