#pragma once

#include <cstdio>
#include <string>

#include "../../../external/olc/olcPixelGameEngine.h"

class AcademiaEngine;

// Look shared by every dev tools panel: sizes, colors and drawing helpers
namespace DevToolsStyle
{
    // Layout, in pixels
    constexpr int TextScale = 2;
    constexpr int CharSize = 8 * TextScale;
    constexpr int ScreenMargin = 12;     // Between the panels and the screen edges
    constexpr int Padding = 12;          // Inside a panel
    constexpr int Corner = 3;            // Size of the cut corners
    constexpr int ChipPadding = 4;       // Inside badges and key boxes
    constexpr int ChipHeight = CharSize + ChipPadding * 2;

    // Palette
    const olc::Pixel PanelColor(16, 18, 26, 215);
    const olc::Pixel BorderColor(70, 80, 105);
    const olc::Pixel TitleColor(110, 180, 255);
    const olc::Pixel LabelColor(135, 145, 165);
    const olc::Pixel ValueColor(235, 238, 245);
    const olc::Pixel GoodColor(90, 215, 120);
    const olc::Pixel WarningColor(255, 190, 60);
    const olc::Pixel BadColor(240, 90, 90);
    const olc::Pixel KeyColor(50, 58, 78);
    const olc::Pixel DisabledColor(80, 88, 108);
    const olc::Pixel HoverColor(70, 82, 110);
    const olc::Pixel ActiveColor(60, 125, 210);

    template<typename... Args>
    std::string Format(const char* format, Args... args)
    {
        char buffer[128];
        std::snprintf(buffer, sizeof(buffer), format, args...);
        return buffer;
    }

    int TextWidth(const std::string& text);

    // Rectangle with cut corners
    void FillPanel(AcademiaEngine& engine, const olc::vi2d& position, const olc::vi2d& size, const olc::Pixel& color);
    void DrawPanelBorder(AcademiaEngine& engine, const olc::vi2d& position, const olc::vi2d& size, const olc::Pixel& color);

    // Semi-transparent panel with a border, so the scene stays visible behind it
    void DrawPanel(AcademiaEngine& engine, const olc::vi2d& position, const olc::vi2d& size);

    // Small filled box with text inside, used for badges and key hints. Returns its width.
    int DrawChip(AcademiaEngine& engine, const olc::vi2d& position, const std::string& text, const olc::Pixel& background, const olc::Pixel& textColor);
}
