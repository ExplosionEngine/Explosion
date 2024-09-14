//
// Created by johnk on 2024/8/23.
//

#pragma once

#include <QColor>

namespace Editor {
    class Theme {
    public:
        Theme();

        Theme& SetColorPrimary(const QColor& color);
        Theme& SetColorSecondary(const QColor& color);
        Theme& SetColorBackground(const QColor& color);
        QColor GetColorPrimary() const;
        QColor GetColorSecondary() const;
        QColor GetColorBackground() const;

    private:
        QColor colorPrimary;
        QColor colorSecondary;
        QColor colorBackground;
    };

    extern Theme defaultTheme;

    class ThemeSwitcher {
    public:
        static void Set(const Theme& inTheme);
        static const Theme& Current();

    private:
        static Theme current;
    };
}
