#include "ldetpushbtn.h"
#include "ldettheme.h"
#include "ldetdrawutils.h"

#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QStyleHints>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QFontMetrics>

LDETPushBtn::LDETPushBtn(QWidget *parent)
    : QPushButton(parent)
    , m_isPressed(false)
{
    init();
}

LDETPushBtn::LDETPushBtn(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
    , m_isPressed(false)
{
    init();
}

LDETPushBtn::~LDETPushBtn()
{
    if (m_borderAnimation) {
        m_borderAnimation->stop();
    }
}

void LDETPushBtn::init()
{
    m_borderAnimation = new QPropertyAnimation(this, "borderOpacity", this);
    m_borderAnimation->setDuration(BORDER_ANIMATION_DURATION);
    m_borderAnimation->setEasingCurve(QEasingCurve::OutCubic);

    updateColorScheme();

    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this]() {
        updateColorScheme();
        update();
    });
}

void LDETPushBtn::updateColorScheme()
{
    auto *theme = LDETTheme::instance();
    m_backgroundColor = theme->widgetBackgroundColor();
    m_normalBorderColor = theme->normalBorderColor();
    m_highlightBorderColor = theme->highlightBorderColor();
    m_pressedBorderColor = theme->pressedBorderColor();
    m_disabledTextColor = theme->textInactiveColor();
    update();
}

qreal LDETPushBtn::borderOpacity() const
{
    return m_borderOpacity;
}

void LDETPushBtn::setBorderOpacity(qreal opacity)
{
    if (!qFuzzyCompare(m_borderOpacity, opacity)) {
        m_borderOpacity = qBound(0.0, opacity, 1.0);
        update();
    }
}

void LDETPushBtn::startBorderAnimation(qreal targetOpacity)
{
    if (m_borderAnimation->state() == QAbstractAnimation::Running) {
        m_borderAnimation->stop();
    }
    m_borderAnimation->setStartValue(m_borderOpacity);
    m_borderAnimation->setEndValue(targetOpacity);
    m_borderAnimation->start();
}

QSize LDETPushBtn::sizeHint() const
{
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(text());
    int textHeight = fm.height();

    int width = textWidth;
    int height = textHeight;

    if (!icon().isNull()) {
        QSize iconSize = this->iconSize();
        width += iconSize.width();
        if (!text().isEmpty()) {
            width += 6;
        }
        height = qMax(height, iconSize.height());
    }

    width += CONTENT_MARGIN * 2;
    height += CONTENT_MARGIN * 2;

    return QSize(width, height);
}

QSize LDETPushBtn::minimumSizeHint() const
{
    return QSize(CONTENT_MARGIN * 4, sizeHint().height());
}

void LDETPushBtn::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect borderRect = rect().adjusted(1, 1, -1, -1);

    QColor borderColor;
    QColor bgColor = m_backgroundColor;

    if (!isEnabled()) {
        borderColor = m_normalBorderColor;
        bgColor = LDETDrawUtils::blendColors(bgColor, QColor(128, 128, 128), 0.1);
    } else if (m_isPressed) {
        borderColor = m_pressedBorderColor;
    } else {
        borderColor = LDETDrawUtils::blendColors(m_normalBorderColor, m_highlightBorderColor, m_borderOpacity);
    }

    LDETDrawUtils::drawRoundedRect(&painter, borderRect, BORDER_RADIUS, bgColor, borderColor, 1);

    if (m_isPressed && isEnabled()) {
        QColor pressedOverlay = LDETTheme::AccentColor;
        pressedOverlay.setAlphaF(0.10);
        QPainterPath overlayPath;
        overlayPath.addRoundedRect(borderRect, BORDER_RADIUS, BORDER_RADIUS);
        painter.fillPath(overlayPath, pressedOverlay);
    }

    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(text());
    int textHeight = fm.height();

    bool hasIcon = !icon().isNull();
    QSize drawIconSize = hasIcon ? iconSize() : QSize(0, 0);

    if (hasIcon) {
        int iconX = CONTENT_MARGIN;
        int iconY = (height() - drawIconSize.height()) / 2;
        QPixmap pix = icon().pixmap(drawIconSize, isEnabled() ? QIcon::Normal : QIcon::Disabled);
        painter.drawPixmap(iconX, iconY, pix);
    }

    if (!text().isEmpty()) {
        int idealTextX = (width() - textWidth) / 2;
        int minTextX = CONTENT_MARGIN;
        if (hasIcon) {
            minTextX = CONTENT_MARGIN + drawIconSize.width() + 6;
        }
        int textX = qMax(idealTextX, minTextX);
        int textY = (height() - textHeight) / 2 + fm.ascent();
        painter.setFont(font());
        painter.setPen(isEnabled() ? palette().color(QPalette::Text) : m_disabledTextColor);
        painter.drawText(textX, textY, text());
    }
}

void LDETPushBtn::enterEvent(QEnterEvent *event)
{
    QPushButton::enterEvent(event);
    if (isEnabled()) {
        startBorderAnimation(1.0);
    }
}

void LDETPushBtn::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);
    if (isEnabled()) {
        startBorderAnimation(0.0);
    }
}

void LDETPushBtn::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isEnabled()) {
        m_isPressed = true;
        update();
    }
    QPushButton::mousePressEvent(event);
}

void LDETPushBtn::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = false;
        update();
    }
    QPushButton::mouseReleaseEvent(event);
}

void LDETPushBtn::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        update();
    }
    QPushButton::changeEvent(event);
}
