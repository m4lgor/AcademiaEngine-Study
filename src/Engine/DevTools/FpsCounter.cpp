#include "FpsCounter.h"

#include "../AcademiaEngine.h"
#include "DevToolsStyle.h"

using namespace DevToolsStyle;

namespace
{
    constexpr float AveragingWindow = 0.25f;
}

void FpsCounter::Update(float elapsedTime)
{
    _AccumulatedTime += elapsedTime;
    ++_AccumulatedFrames;

    if (_AccumulatedTime >= AveragingWindow)
    {
        _AverageFrameTime = _AccumulatedTime / static_cast<float>(_AccumulatedFrames);
        _AccumulatedTime = 0.0f;
        _AccumulatedFrames = 0;
    }
}

void FpsCounter::Draw(AcademiaEngine& engine) const
{
    // Discreet: dim text on a faint background, colored only when the frame rate drops
    const float fps = GetFps();
    const olc::Pixel color = fps >= GoodFps ? LabelColor : FpsColor(fps);
    const std::string text = Format("%.0f FPS", fps);

    const olc::vi2d size = { TextWidth(text) + ChipPadding * 2, ChipHeight };
    const olc::vi2d position = { engine.ScreenWidth() - ScreenMargin - size.x, ScreenMargin };

    engine.SetPixelMode(olc::Pixel::ALPHA);
    FillPanel(engine, position, size, olc::Pixel(0, 0, 0, 120));
    engine.SetPixelMode(olc::Pixel::NORMAL);
    engine.DrawString(position + olc::vi2d{ ChipPadding, ChipPadding }, text, color, TextScale);
}
