#include "ExampleGameObject.h"

#include <cmath>

#include "../Engine/AcademiaEngine.h"

void ExampleGameObject::Draw(AcademiaEngine& engine)
{
    Vector2i pixelPos = engine.ConvertWorldPositionToPixels(Position);
    const float pixelRadius = engine.ConvertWorldLengthToPixels(Radius);
    engine.FillCircle(pixelPos, static_cast<int32_t>(std::lround(pixelRadius)), Color);
}
