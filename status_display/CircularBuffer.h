#include <Arduino.h>

#ifndef __CIRCULARBUFFER_H__
#define __CIRCULARBUFFER_H__

#define BUFFER_SIZE 8192

class CircularBuffer {
public:
  CircularBuffer();

  void AddChar(char ch);
  bool HasChar();
  char GetChar();

private:
  char buffer[BUFFER_SIZE];
  uint16_t head;
  uint16_t tail;
  bool lineStarted;
};

#endif