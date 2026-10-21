#include "DevToolsStyle.h"

#include "../AcademiaEngine.h"

namespace DevToolsStyle
{
    int TextWidth(const std::string& text)
    {
        return static_cast<int>(text.size()) * CharSize;
    }

    void FillPanel(AcademiaEngine& engine, const olc::vi2d& position, const olc::vi2d& size, const olc::Pixel& color)
    {
        engine.FillRect({ position.x + Corner, position.y }, { size.x - Corner * 2, size.y }, color);
        engine.FillRect({ position.x, position.y + Corner }, { Corner, size.y - Corner * 2 }, color);
        engine.FillRect({ position.x + size.x - Corner, position.y + Corner }, { Corner, size.y - Corner * 2 }, color);
    }

    void DrawPanelBorder(AcademiaEngine& engine, const olc::vi2d& position, const olc::vi2d& size, const olc::Pixel& color)
    {
        const int left = position.x;
        const int top = position.y;
        const int right = position.x + size.x - 1;
        const int bottom = position.y + size.y - 1;

        engine.DrawLine({ left + Corner, top }, { right - Corner, top }, color);
        engine.DrawLine({ left + Corner, bottom }, { right - Corner, bottom }, color);
        engine.DrawLine({ left, top + Corner }, { left, bottom - Corner }, color);
        engine.DrawLine({ right, top + Corner }, { right, bottom - Corner }, color);
        engine.DrawLine({ left, top + Corner }, { left + Corner, top }, color);
        engine.DrawLine({ right - Corner, top }, { right, top + Corner }, color);
        engine.DrawLine({ left, bottom - Corner }, { left + Corner, bottom }, color);
        engine.DrawLine({ right - Corner, bottom }, { right, bottom - Corner }, color);
    }

    void DrawPanel(AcademiaEngine& engine, const olc::vi2d& position, const olc::vi2d& size)
    {
        engine.SetPixelMode(olc::Pixel::ALPHA);
        FillPanel(engine, position, size, PanelColor);
        engine.SetPixelMode(olc::Pixel::NORMAL);
        DrawPanelBorder(engine, position, size, BorderColor);
    }

    int DrawChip(AcademiaEngine& engine, const olc::vi2d& position, const std::string& text, const olc::Pixel& background, const olc::Pixel& textColor)
    {
        const olc::vi2d size = { TextWidth(text) + ChipPadding * 2, ChipHeight };
        FillPanel(engine, position, size, background);
        engine.DrawString({ position.x + ChipPadding, position.y + ChipPadding }, text, textColor, TextScale);
        return size.x;
    }
}
