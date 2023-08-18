#include "Math/MathUtil.h"

float Core::Math::lerp(float x, float y, float t)
{
    return x * (1.f - t) + y * t;
}