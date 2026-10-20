#include "Camera.h"

#include <algorithm>
#include <cmath>

olc::vi2d Camera::WorldToScreen(const olc::vf2d& worldPosition, const olc::vi2d& screenSize) const
{
    const float scale = GetScale();
    const float pixelx = (worldPosition.x - Position.x) * scale + static_cast<float>(screenSize.x) * 0.5f;
    const float pixely = -(worldPosition.y - Position.y) * scale + static_cast<float>(screenSize.y) * 0.5f;

    // Round instead of truncating, otherwise objects jitter around 0
    return { static_cast<int>(std::lround(pixelx)), static_cast<int>(std::lround(pixely)) };
}

olc::vf2d Camera::ScreenToWorld(const olc::vi2d& screenPosition, const olc::vi2d& screenSize) const
{
    const float scale = GetScale();
    const float positionx = (static_cast<float>(screenPosition.x) - static_cast<float>(screenSize.x) * 0.5f) / scale + Position.x;
    const float positiony = (static_cast<float>(screenSize.y) * 0.5f - static_cast<float>(screenPosition.y)) / scale + Position.y;
    return { positionx, positiony };
}

float Camera::WorldToScreenLength(float worldLength) const
{
    return worldLength * GetScale();
}

float Camera::ScreenToWorldLength(float screenLength) const
{
    return screenLength / GetScale();
}

float Camera::GetScale() const
{
    // Guard against a zero or negative zoom flipping or collapsing the view
    constexpr float minZoom = 0.001f;
    return PixelsPerUnit * std::max(Zoom, minZoom);
}
