#include "VisualTest.h"

#include <iostream>

#include "doctest/doctest.h"

float VisualTestEngine::HoldSeconds = 0.0f;

namespace
{
    // Objects spawned in Setup are flushed at the end of the next frame and drawn the one after
    constexpr int MinFramesPerStep = 3;
}

namespace
{
    // The default GameBase updates the Scene and the steps do the rest
    class EmptyGame : public GameBase
    {
    public:
        void Initialize(AcademiaEngine* engineContext) override {}
        void Uninitialize() override {}
    };
}

VisualTestEngine::VisualTestEngine(const std::string& testName, std::vector<VisualStep> steps)
    : _TestName(testName)
    , _Steps(std::move(steps))
{
    sAppName = "Visual test - " + testName;
    SetGame(std::make_unique<EmptyGame>());
}

bool VisualTestEngine::OnUserDestroy()
{
    // The steps add objects without a game to remove them: clean up so the leak report stays quiet
    GetScene().Clear();
    return AcademiaEngine::OnUserDestroy();
}

bool VisualTestEngine::OnUserUpdate(float elapsedTime)
{
    // Regular engine frame: clear, update and draw the Scene
    AcademiaEngine::OnUserUpdate(elapsedTime);

    if (_CurrentStep >= _Steps.size())
        return false;

    VisualStep& step = _Steps[_CurrentStep];

    if (!_StepStarted)
    {
        std::cout << "[visual] " << _TestName << " - step " << (_CurrentStep + 1) << "/" << _Steps.size()
                  << ": " << step.Name << std::endl;

        if (step.Setup)
            step.Setup(*this);

        _StepStarted = true;
        _StepFrames = 0;
        _StepTime = 0.0f;
    }
    else
    {
        ++_StepFrames;
        _StepTime += elapsedTime;

        if (_StepFrames >= MinFramesPerStep && _StepTime >= HoldSeconds)
        {
            // Check before the label is drawn, so it only sees the Scene
            {
                INFO("Visual test: ", _TestName, ", step: ", step.Name);
                if (step.Check)
                    step.Check(*this);
            }

            ++_CompletedSteps;
            ++_CurrentStep;
            _StepStarted = false;
        }
    }

    DrawStepLabel();
    return true;
}

olc::Pixel VisualTestEngine::GetPixelAtWorld(const Vector2f& worldPosition)
{
    const Vector2i pixel = ConvertWorldPositionToPixels(worldPosition);
    return GetDrawTarget()->GetPixel(pixel.x, pixel.y);
}

void VisualTestEngine::DrawStepLabel()
{
    if (_CurrentStep >= _Steps.size())
        return;

    const std::string stepLabel = "Step " + std::to_string(_CurrentStep + 1) + "/" + std::to_string(_Steps.size())
        + ": " + _Steps[_CurrentStep].Name;

    // Bottom of the screen, the debug overlay uses the top
    const int top = ScreenHeight() - 44;
    FillRect({ 0, top }, { ScreenWidth(), 44 }, olc::VERY_DARK_GREY);
    DrawString({ 8, top + 6 }, _TestName, olc::WHITE, 2);
    DrawString({ 8, top + 26 }, stepLabel, olc::YELLOW, 2);
}

void RunVisualTest(const std::string& testName, std::vector<VisualStep> steps)
{
    const size_t stepCount = steps.size();

    VisualTestEngine engine(testName, std::move(steps));
    REQUIRE(engine.Construct(1024, 1024, 1, 1));
    engine.Start();

    CHECK(engine.GetCompletedStepCount() == stepCount);
}
