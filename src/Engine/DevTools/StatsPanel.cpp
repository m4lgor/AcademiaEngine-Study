#include "StatsPanel.h"

#include <algorithm>
#include <string>
#include <vector>

#include "../AcademiaEngine.h"
#include "DevToolsStyle.h"
#include "FpsCounter.h"

using namespace DevToolsStyle;

namespace
{
    constexpr int RowHeight = CharSize + 6;
    constexpr int SectionGap = 10;       // Space around the separator lines
    constexpr int ColumnGap = CharSize;  // Between the label and value columns
    constexpr int HintGap = 6;           // Between a key box and its action
}

void StatsPanel::Draw(AcademiaEngine& engine, const FpsCounter& fpsCounter) const
{
    const Camera& camera = engine.GetCamera();
    const Vector2f mouse = engine.ConvertPixelsToWorldPosition(engine.GetMousePos());
    const float fps = fpsCounter.GetFps();
    const bool paused = engine.GetTimeControl().IsPaused();

    // Label and value columns. A row can have a dimmed suffix after its value.
    struct Row { std::string Label; std::string Value; olc::Pixel ValueColor; std::string Suffix; };
    const std::vector<Row> rows =
    {
        { "FPS", Format("%.0f", fps), FpsColor(fps), Format(" %.2f ms", fpsCounter.GetFrameTime() * 1000.0f) },
        { "Objects", std::to_string(engine.GetScene().GetObjectCount()), ValueColor, "" },
        { "Camera", Format("%.2f, %.2f", camera.Position.x, camera.Position.y), ValueColor, "" },
        { "Zoom", Format("x%.2f", camera.Zoom), ValueColor, "" },
        { "Mouse", Format("%.2f, %.2f", mouse.x, mouse.y), ValueColor, "" },
    };

    // Footer key hints. F3 only works while paused, so it is dimmed otherwise.
    struct KeyHint { std::string Key; std::string Action; bool Enabled; };
    const KeyHint hints[] =
    {
        { "F1", "Hide", true },
        { "F2", paused ? "Resume" : "Pause ", true },  // Same length, so the panel keeps its width
        { "F3", "Step", paused },
    };

    const std::string title = "DEBUG";
    const std::string status = paused ? "PAUSED" : "RUNNING";

    // Measure everything to size the panel
    // Values get a minimum width so the panel does not resize every time a number gains a digit
    int labelWidth = 0;
    int valueWidth = TextWidth("-000.00, -000.00");
    for (const Row& row : rows)
    {
        labelWidth = std::max(labelWidth, TextWidth(row.Label));
        valueWidth = std::max(valueWidth, TextWidth(row.Value + row.Suffix));
    }

    int hintsWidth = 0;
    for (const KeyHint& hint : hints)
        hintsWidth += TextWidth(hint.Key) + ChipPadding * 2 + HintGap + TextWidth(hint.Action) + CharSize;
    hintsWidth -= CharSize;

    const int headerWidth = TextWidth(title) + CharSize + TextWidth(status) + ChipPadding * 2;
    const int contentWidth = std::max({ labelWidth + ColumnGap + valueWidth, hintsWidth, headerWidth });
    const int rowsHeight = static_cast<int>(rows.size()) * RowHeight - (RowHeight - CharSize);
    const int contentHeight = ChipHeight + SectionGap * 2 + rowsHeight + SectionGap * 2 + ChipHeight;

    const Vector2i panelPosition = { ScreenMargin, ScreenMargin };
    const Vector2i panelSize = { contentWidth + Padding * 2, contentHeight + Padding * 2 };
    DrawPanel(engine, panelPosition, panelSize);

    const int left = panelPosition.x + Padding;
    const int right = left + contentWidth;
    int y = panelPosition.y + Padding;

    // Header: title on the left, status badge on the right
    engine.DrawString({ left, y + ChipPadding }, title, TitleColor, TextScale);
    const int statusWidth = TextWidth(status) + ChipPadding * 2;
    DrawChip(engine, { right - statusWidth, y }, status, paused ? WarningColor : GoodColor, olc::BLACK);
    y += ChipHeight + SectionGap;

    engine.DrawLine({ left, y }, { right, y }, BorderColor);
    y += SectionGap;

    // Info rows
    for (const Row& row : rows)
    {
        const int valueX = left + labelWidth + ColumnGap;
        engine.DrawString({ left, y }, row.Label, LabelColor, TextScale);
        engine.DrawString({ valueX, y }, row.Value, row.ValueColor, TextScale);
        if (!row.Suffix.empty())
            engine.DrawString({ valueX + TextWidth(row.Value), y }, row.Suffix, LabelColor, TextScale);
        y += RowHeight;
    }
    y += SectionGap - (RowHeight - CharSize);

    engine.DrawLine({ left, y }, { right, y }, BorderColor);
    y += SectionGap;

    // Footer: key hints
    int x = left;
    for (const KeyHint& hint : hints)
    {
        const olc::Pixel keyText = hint.Enabled ? ValueColor : DisabledColor;
        const olc::Pixel actionText = hint.Enabled ? LabelColor : DisabledColor;
        x += DrawChip(engine, { x, y }, hint.Key, KeyColor, keyText) + HintGap;
        engine.DrawString({ x, y + ChipPadding }, hint.Action, actionText, TextScale);
        x += TextWidth(hint.Action) + CharSize;
    }
}
