#pragma once
#include <functional>
#include <string>
#include <vector>

#include "Engine/AcademiaEngine.h"

class VisualTestEngine;

// One step of a visual test: Setup changes the scene, then after a few frames
// Check looks at what was actually drawn on screen.
struct VisualStep
{
    std::string Name;
    std::function<void(VisualTestEngine&)> Setup;
    std::function<void(VisualTestEngine&)> Check;
};

// Runs the real AcademiaEngine window with a scripted list of steps. The game does nothing: only the steps change the Scene.
// The window closes by itself once every step is done.
class VisualTestEngine : public AcademiaEngine
{
public:
    VisualTestEngine(const std::string& testName, std::vector<VisualStep> steps);

    bool OnUserUpdate(float elapsedTime) override;
    bool OnUserDestroy() override;

    // Color currently drawn at a world position
    olc::Pixel GetPixelAtWorld(const Vector2f& worldPosition);

    size_t GetCompletedStepCount() const { return _CompletedSteps; }

    // How long each step stays on screen. 0 = as fast as possible. Set with --visual-hold=<seconds>.
    static float HoldSeconds;

private:
    void DrawStepLabel();

    std::string _TestName;
    std::vector<VisualStep> _Steps;
    size_t _CurrentStep = 0;
    size_t _CompletedSteps = 0;
    bool _StepStarted = false;
    int _StepFrames = 0;
    float _StepTime = 0.0f;
};

// Opens the engine window, plays every step, and checks they all completed.
void RunVisualTest(const std::string& testName, std::vector<VisualStep> steps);
