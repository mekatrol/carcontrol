#include "CST816S.h"

struct touch_state_struct {
  int x;
  int y;
  bool state_changed;
  bool debounced_state;
  bool debouncing_state;
  long prev_debounce_millis;
};

class Touch {
public:
  Touch(int sda, int scl, int rst, int irq);
  void start();
  void debounce(long now_millis);

  const int getX();
  const int getY();
  const bool getStateChanged();

private:
  touch_state_struct touch_state;
  CST816S touch;
};