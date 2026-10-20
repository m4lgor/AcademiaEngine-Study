#pragma once
#include "GameObject.h"

class ExampleGameObject : public GameObject
{
public:
    void Draw(AcademiaEngine& engine) override;

    // In world units
    float Radius = 0.4f;
    olc::Pixel Color = olc::WHITE;
};
