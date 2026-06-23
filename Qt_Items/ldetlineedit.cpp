#include "ldetlineedit.h"
#include "ldettheme.h"
#include "ldetdrawutils.h"

#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QStyleHints>
#include <QFocusEvent>
#include <QResizeEvent>
#include <QIcon>
#include <QPalette>
#include <QFontMetrics>

LDETLineEdit::LDETLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , m_echoModeToggleEnabled(true)
{
    init();
}

LDETLineEdit::~LDETLineEdit()
{
    if (m_borderAnimation) {
        m_borderAnimation->stop();
    }
}

void LDETLineEdit::init()
{
    setFrame(false);
    setAttribute(Qt::WA_TranslucentBackground, true);

    m_borderAnimation = new QPropertyAnimation(this, "borderOpacity", this);
    m_borderAnimation->setDuration(BORDER_ANIMATION_DURATION);
    m_borderAnimation->setEasingCurve(QEasingCurve::OutCubic);

    m_echoToggleAction = addAction(QIcon(":/icons/eye.svg"), QLineEdit::TrailingPosition);
    m_echoToggleAction->setVisible(false);
    m_echoToggleAction->setToolTip(tr("Toggle password visibility"));

    connect(m_echoToggleAction, &QAction::triggered, this, [this]() {
        if (echoMode() == QLineEdit::Password) {
            setEchoMode(QLineEdit::Normal);
        } else {
            setEchoMode(QLineEdit::Password);
        }
        updateButtonIcon();
    });

    updateColorScheme();

    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this]() {
        updateColorScheme();
        update();
    });

    connect(this, &QLineEdit::textChanged, this, [this]() {
        updateLayout();
    });

    updateLayout();
}

void LDETLineEdit::updateColorScheme()
{
    auto *theme = LDETTheme::instance();
    m_backgroundColor = theme->widgetBackgroundColor();
    m_normalBorderColor = theme->normalBorderColor();
    m_highlightBorderColor = theme->highlightBorderColor();
    m_focusBorderColor = theme->focusBorderColor();
    m_textColor = theme->textColor();

    QPalette pal = palette();
    pal.setColor(QPalette::Text, m_textColor);
    pal.setColor(QPalette::ButtonText, m_textColor);
    pal.setColor(QPalette::Highlight, LDETTheme::AccentColor);
    pal.setColor(QPalette::HighlightedText, Qt::black);
    pal.setColor(QPalette::Base, Qt::transparent);
    pal.setColor(QPalette::Window, Qt::transparent);
    setPalette(pal);

    updateButtonIcon();
    update();
}

void LDETLineEdit::updateButtonIcon()
{
    QString iconPath = (echoMode() == QLineEdit::Password)
                           ? QStringLiteral(":/icons/eye-slash.svg")
                           : QStringLiteral(":/icons/eye.svg");
    m_echoToggleAction->setIcon(QIcon(iconPath));
}

void LDETLineEdit::updateLayout()
{
    bool shouldShowButton = m_echoModeToggleEnabled &&
                            (echoMode() == QLineEdit::Password ||
                             echoMode() == QLineEdit::PasswordEchoOnEdit);

    if (shouldShowButton) {
        m_echoToggleAction->setVisible(true);
        updateButtonIcon();
        setTextMargins(CONTENT_MARGIN_H, 0, BUTTON_SIZE + BUTTON_SPACING, 0);
    } else {
        m_echoToggleAction->setVisible(false);
        setTextMargins(CONTENT_MARGIN_H, 0, CONTENT_MARGIN_H, 0);
    }
}

qreal LDETLineEdit::borderOpacity() const { return m_borderOpacity; }

void LDETLineEdit::setBorderOpacity(qreal opacity)
{
    if (!qFuzzyCompare(m_borderOpacity, opacity)) {
        m_borderOpacity = qBound(0.0, opacity, 1.0);
        update();
    }
}

void LDETLineEdit::startBorderAnimation(qreal targetOpacity)
{
    if (m_borderAnimation->state() == QAbstractAnimation::Running) {
        m_borderAnimation->stop();
    }
    m_borderAnimation->setStartValue(m_borderOpacity);
    m_borderAnimation->setEndValue(targetOpacity);
    m_borderAnimation->start();
}

void LDETLineEdit::setEchoModeToggleEnabled(bool enabled)
{
    if (m_echoModeToggleEnabled != enabled) {
        m_echoModeToggleEnabled = enabled;
        updateLayout();
    }
}

bool LDETLineEdit::isEchoModeToggleEnabled() const { return m_echoModeToggleEnabled; }

QSize LDETLineEdit::sizeHint() const
{
    QFontMetrics fm(font());
    int textHeight = fm.height();
    int height = textHeight + CONTENT_MARGIN_V * 2;
    int width = fm.horizontalAdvance(QStringLiteral("XXXXXXXXXX")) + CONTENT_MARGIN_H * 2;
    return QSize(width, height);
}

QSize LDETLineEdit::minimumSizeHint() const
{
    QFontMetrics fm(font());
    int textHeight = fm.height();
    int height = textHeight + CONTENT_MARGIN_V * 2;
    int width = fm.horizontalAdvance(QStringLiteral("XX")) + CONTENT_MARGIN_H * 2;
    return QSize(width, height);
}

void LDETLineEdit::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect borderRect = rect().adjusted(1, 1, -1, -1);

    QColor borderColor;
    QColor bgColor = m_backgroundColor;

    if (!isEnabled()) {
        borderColor = m_normalBorderColor;
        bgColor = LDETDrawUtils::blendColors(bgColor, QColor(128, 128, 128), 0.1);
    } else if (m_hasFocus) {
        borderColor = m_focusBorderColor;
    } else {
        borderColor = LDETDrawUtils::blendColors(m_normalBorderColor, m_highlightBorderColor, m_borderOpacity);
    }

    LDETDrawUtils::drawRoundedRect(&painter, borderRect, BORDER_RADIUS, bgColor, borderColor, 1);

    LDETDrawUtils::safeEndPainter(&painter);

    QLineEdit::paintEvent(event);
}

void LDETLineEdit::enterEvent(QEnterEvent *event)
{
    if (!isEnabled()) {
        return;
    }
    QLineEdit::enterEvent(event);
    if (!m_hasFocus) {
        startBorderAnimation(1.0);
    }
}

void LDETLineEdit::leaveEvent(QEvent *event)
{
    if (!isEnabled()) {
        return;
    }
    QLineEdit::leaveEvent(event);
    if (!m_hasFocus) {
        startBorderAnimation(0.0);
    }
}

void LDETLineEdit::focusInEvent(QFocusEvent *event)
{
    m_hasFocus = true;
    if (m_borderAnimation->state() == QAbstractAnimation::Running) {
        m_borderAnimation->stop();
    }
    QLineEdit::focusInEvent(event);
}

void LDETLineEdit::focusOutEvent(QFocusEvent *event)
{
    m_hasFocus = false;
    if (!underMouse()) {
        startBorderAnimation(0.0);
    }
    QLineEdit::focusOutEvent(event);
}

void LDETLineEdit::resizeEvent(QResizeEvent *event)
{
    QLineEdit::resizeEvent(event);
    updateLayout();
}

void LDETLineEdit::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        setAttribute(Qt::WA_TransparentForMouseEvents, !isEnabled());
        update();
    }
    QLineEdit::changeEvent(event);
}
