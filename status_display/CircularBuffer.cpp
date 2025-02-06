#include "CircularBuffer.h"

CircularBuffer::CircularBuffer() {
  this->head = 0;
  this->tail = 0;
}

void CircularBuffer::AddChar(char ch) {
  this->buffer[this->head++] = ch;

  if (this->head >= BUFFER_SIZE) {
    this->head = 0;
  }

  // Buffer overrun?
  if (this->head == this->tail) {
    this->tail++;

    if (this->tail >= BUFFER_SIZE) {
      this->tail = 0;
    }
  }
}

bool CircularBuffer::HasChar() {
  return this->head != this->tail;
}

char CircularBuffer::GetChar() {
  if (!this->HasChar()) {
    return '\0';
  }

  char ch = this->buffer[this->tail++];

  if (this->tail >= BUFFER_SIZE) {
    this->tail = 0;
  }

  return ch;
}