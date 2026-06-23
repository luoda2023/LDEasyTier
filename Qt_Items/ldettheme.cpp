#include "ldettheme.h"

#include <QApplication>
#include <QStyleHints>
#include <QPalette>

LDETTheme::LDETTheme(QObject *parent)
    : QObject(parent)
{
}

LDETTheme *LDETTheme::instance()
{
    static LDETTheme theme;
    return &theme;
}

bool LDETTheme::isDarkMode() const
{
    return qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark;
}

QColor LDETTheme::backgroundColor() const
{
    return isDarkMode() ? QColor(35, 35, 35) : QApplication::palette().color(QPalette::Window);
}

QColor LDETTheme::widgetBackgroundColor() const
{
    return isDarkMode() ? QColor(45, 45, 45) : QColor(240, 240, 240);
}

QColor LDETTheme::selectedBackgroundColor() const
{
    return isDarkMode() ? QColor(70, 70, 70, 50) : QColor(102, 204, 255, 30);
}

QColor LDETTheme::normalBorderColor() const
{
    return isDarkMode() ? QColor(70, 70, 70) : QColor(180, 180, 180);
}

QColor LDETTheme::highlightBorderColor() const
{
    return AccentColor;
}

QColor LDETTheme::pressedBorderColor() const
{
    return AccentColorDark;
}

QColor LDETTheme::focusBorderColor() const
{
    return AccentColor;
}

QColor LDETTheme::textColor() const
{
    return isDarkMode() ? QColor(220, 220, 220) : QColor(30, 30, 30);
}

QColor LDETTheme::textInactiveColor() const
{
    return isDarkMode() ? QColor(140, 140, 140) : QColor(130, 130, 130);
}

QColor LDETTheme::selectedTextColor() const
{
    return isDarkMode() ? QColor(220, 220, 220) : QColor(0, 0, 0);
}

QColor LDETTheme::switchActiveColor() const
{
    return AccentColor;
}

QColor LDETTheme::switchInactiveColor() const
{
    return isDarkMode() ? QColor(80, 80, 80) : QColor(200, 200, 200);
}

QColor LDETTheme::switchSliderColor() const
{
    return Qt::white;
}

QColor LDETTheme::hoverBackgroundColor() const
{
    return isDarkMode() ? QColor(60, 60, 60, 120) : QColor(0, 0, 0, 25);
}

QColor LDETTheme::hoverFillColor(const QColor &base) const
{
    return QColor::fromRgbF(base.redF(), base.greenF(), base.blueF(), 0.15);
}
