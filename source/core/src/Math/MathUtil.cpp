#include "Math/MathUtil.h"

float MathUtil::lerp(float x, float y, float t)
{
     return x * (1.f - t) + y * t;
}