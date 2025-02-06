#include "CST816S.h"
#include "touch.h"

const unsigned long debounce_period = 50;
const CST816S touch(6, 7, 13, 5);  // sda, scl, rst, irq

Touch::Touch(int sda, int scl, int rst, int irq)
  : touch(sda, scl, rst, irq) {
  // Clear current touch state
  memset(&this->touch_state, 0, sizeof(touch_state_struct));
}

void Touch::start() {
  // Start touch
  this->touch.begin();
}

const int Touch::getX() {
  return this->touch_state.x;
}

const int Touch::getY() {
  return this->touch_state.y;
}

const bool Touch::getState() {
  return this->touch_state.debounced_state;
}

const bool Touch::getStateChanged() {
  return this->touch_state.state_changed;
}

void Touch::debounce(long now_millis) {
  // Default to state has not changed
  this->touch_state.state_changed = false;

  // Will be true if the screen is currently being touched
  bool touched = touch.available();

  // If the state has changed since last iteration then reset debounce time
  // to restart debounce period again
  if (touched != this->touch_state.debouncing_state) {
    // Restart debounce period
    this->touch_state.prev_debounce_millis = now_millis;

    // Update debouncing state
    this->touch_state.debouncing_state = touched;
  }

  // Test if debounce time has expired, expired means the touch has not changed state for longer than
  // the debounce period (ie the touch value is stable and debounced).
  bool debounce_period_expired = (now_millis - this->touch_state.prev_debounce_millis) > debounce_period;

  // If successfully debounced then update state
  if (debounce_period_expired) {
    // Flag if the touch state has changed
    this->touch_state.state_changed = touched != this->touch_state.debounced_state;

    // Update debounced state to current state
    this->touch_state.debounced_state = touched;

    // Update the X, Y touch points (will be -1 if not currently being touched)
    this->touch_state.x = touched ? touch.data.x : -1;
    this->touch_state.y = touched ? touch.data.y : -1;
  }
}