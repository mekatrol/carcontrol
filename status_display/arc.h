#include <Arduino.h>
#include <TFT_eSPI.h>

enum DIRECTION {
  CW = 0x01,
  CCW = 0x02
};

class Arc {
public:
  Arc(int32_t outerRadius, int32_t innerRadius, int32_t startAngle, int32_t endAngle, int32_t startValue, int32_t endValue, int32_t centerX, int32_t centerY, enum DIRECTION direction);
  void render(TFT_eSPI *tft, int32_t value, uint32_t foreColor, uint32_t backColor, uint32_t tickColor);

private:
  int32_t outerRadius;
  int32_t innerRadius;
  int32_t startAngle;
  int32_t endAngle;
  int32_t startValue;
  int32_t endValue;
  int32_t centerX;
  int32_t centerY;
  enum DIRECTION direction;

  double valueToDegreeScale;

  int32_t valueToDegree(int32_t value);
};