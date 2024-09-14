//
// Created by johnk on 2024/8/23.
//

#include <Editor/Theme.h>

namespace Editor {
    Theme::Theme()
        // TODO
        : colorPrimary()
        , colorSecondary()
        , colorBackground(0x24, 0x29, 0x2f)
    {
    }

    Theme& Theme::SetColorPrimary(const QColor& color)
    {
        colorPrimary = color;
        return *this;
    }

    Theme& Theme::SetColorSecondary(const QColor& color)
    {
        colorSecondary = color;
        return *this;
    }

    Theme& Theme::SetColorBackground(const QColor& color)
    {
        colorBackground = color;
        return *this;
    }

    QColor Theme::GetColorPrimary() const
    {
        return colorPrimary;
    }

    QColor Theme::GetColorSecondary() const
    {
        return colorSecondary;
    }

    QColor Theme::GetColorBackground() const
    {
        return colorBackground;
    }

    Theme defaultTheme = Theme(); // NOLINT

    void ThemeSwitcher::Set(const Theme& inTheme)
    {
        current = inTheme;
    }

    const Theme& ThemeSwitcher::Current()
    {
        return current;
    }

    Theme ThemeSwitcher::current = defaultTheme;
}
