#pragma once
#include "../../external/olc/olcPixelGameEngine.h"
#include "../Math/Vector.h"

// Converts between world space (units, Y up, Position at the center of the screen)
// and screen space (pixels, Y down, origin at the top-left corner).
class Camera
{
public:

    Vector2i WorldToScreen(const Vector2f& worldPosition, const Vector2i& screenSize) const;
    Vector2f ScreenToWorld(const Vector2i& screenPosition, const Vector2i& screenSize) const;

    // For sizes (radius, width...) rather than positions
    float WorldToScreenLength(float worldLength) const;
    float ScreenToWorldLength(float screenLength) const;

    // Pixels per world unit, zoom included
    float GetScale() const;

    // World position shown at the center of the screen
    Vector2f Position = { 0.0f, 0.0f };

    // 2.0 shows everything twice as big
    float Zoom = 1.0f;

    // How many pixels one world unit covers at zoom 1
    float PixelsPerUnit = 50.0f;
};
