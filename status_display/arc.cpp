#include "arc.h"

Arc::Arc(int32_t outerRadius, int32_t innerRadius, int32_t startAngle, int32_t endAngle, int32_t startValue, int32_t endValue, int32_t centerX, int32_t centerY, enum DIRECTION direction)
  : outerRadius(outerRadius),
    innerRadius(innerRadius),
    startAngle(startAngle),
    endAngle(endAngle),
    startValue(startValue),
    endValue(endValue),
    centerX(centerX),
    centerY(centerY), direction(direction) {
  int32_t valueRange = endValue - startValue;
  int32_t degreeRange = endAngle - startAngle;

  this->valueToDegreeScale = degreeRange / (double) valueRange;  
}

void Arc::render(TFT_eSPI *tft, int32_t value, uint32_t foreColor, uint32_t backColor, uint32_t tickColor) {
  int32_t valueDegrees = this->valueToDegree(value);

  // Because arcs are always renderd in a clockwise direction, for arcs we want to display value in a
  // counter clockwise direction then we need to reverse colors
  // If the direction is CW then forecolor is from start angle to value angle
  // If the direction is CCW then forecolor is from value angle to end angle

  // The color from start angle to value angle
  uint32_t color1 = this->direction == CW ? foreColor : backColor;

  // The color from value angle to end angle
  uint32_t color2 = this->direction == CW ? backColor : foreColor;

  // Render from start angle to value angle
  if (valueDegrees != this->startAngle) {
    tft->drawSmoothArc(this->centerX, this->centerY, innerRadius, outerRadius, this->startAngle, valueDegrees, color1, color1, true);
  }

  // Render from value angle to end angle
  if (valueDegrees != this->endAngle) {
    tft->drawSmoothArc(this->centerX, this->centerY, innerRadius, outerRadius, valueDegrees, this->endAngle, color2, color2, true);
  }

  tft->drawSmoothArc(this->centerX, this->centerY, innerRadius, outerRadius, this->startAngle - 1, this->startAngle + 1, tickColor, tickColor, true);
  tft->drawSmoothArc(this->centerX, this->centerY, innerRadius, outerRadius, this->endAngle - 1, this->endAngle + 1, tickColor, tickColor, true);
}

int32_t Arc::valueToDegree(int32_t value) {
  int32_t scaledValue = value * this->valueToDegreeScale;
  int32_t angle = this->direction == CW ? this->startAngle + scaledValue : this->endAngle - scaledValue;

  // If rendering clockwise, the value is from start angle, if rendering CCW then is from end angle
  return angle;
}