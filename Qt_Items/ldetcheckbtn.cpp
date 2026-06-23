#include "ldetcheckbtn.h"
#include "ldettheme.h"
#include "ldetdrawutils.h"

#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QStyleHints>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QFontMetrics>
#include <QKeyEvent>

LDETCheckBtn::LDETCheckBtn(QWidget *parent)
    : QCheckBox(parent)
    , m_tipFontSize(9)
    , m_sliderPosition(0.0)
    , m_borderOpacity(0.0)
    , m_pressedOnSwitch(false)
    , m_borderless(false)
    , m_isProgrammaticChange(false)
{
    init();
}

LDETCheckBtn::LDETCheckBtn(const QString &text, QWidget *parent)
    : QCheckBox(text, parent)
    , m_tipFontSize(9)
    , m_sliderPosition(0.0)
    , m_borderOpacity(0.0)
    , m_pressedOnSwitch(false)
    , m_borderless(false)
    , m_isProgrammaticChange(false)
{
    init();
}

LDETCheckBtn::~LDETCheckBtn()
{
    if (m_animation) {
        m_animation->stop();
    }
    if (m_borderAnimation) {
        m_borderAnimation->stop();
    }
}

void LDETCheckBtn::init()
{
    m_animation = new QPropertyAnimation(this, "sliderPosition", this);
    m_animation->setDuration(ANIMATION_DURATION);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    m_borderAnimation = new QPropertyAnimation(this, "borderOpacity", this);
    m_borderAnimation->setDuration(BORDER_ANIMATION_DURATION);
    m_borderAnimation->setEasingCurve(QEasingCurve::OutCubic);

    updateColorScheme();

    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this]() {
        updateColorScheme();
        update();
    });

    m_sliderPosition = isChecked() ? 1.0 : 0.0;
}

void LDETCheckBtn::updateColorScheme()
{
    auto *theme = LDETTheme::instance();
    m_activeColor = theme->switchActiveColor();
    m_inactiveColor = theme->switchInactiveColor();
    m_sliderColor = theme->switchSliderColor();
    m_tipHighlightColor = theme->highlightBorderColor();
    update();
}

QString LDETCheckBtn::tipText() const { return m_tipText; }
void LDETCheckBtn::setTipText(const QString &text)
{
    if (m_tipText != text) {
        m_tipText = text;
        updateGeometry();
        update();
    }
}

QString LDETCheckBtn::briefTip() const { return m_briefTip; }
void LDETCheckBtn::setBriefTip(const QString &text)
{
    if (m_briefTip != text) {
        m_briefTip = text;
        updateGeometry();
        update();
    }
}

int LDETCheckBtn::tipFontSize() const { return m_tipFontSize; }
void LDETCheckBtn::setTipFontSize(int size)
{
    if (size > 0 && m_tipFontSize != size) {
        m_tipFontSize = size;
        updateGeometry();
        update();
    }
}

qreal LDETCheckBtn::sliderPosition() const { return m_sliderPosition; }
void LDETCheckBtn::setSliderPosition(qreal pos)
{
    if (!qFuzzyCompare(m_sliderPosition, pos)) {
        m_sliderPosition = qBound(0.0, pos, 1.0);
        update();
    }
}

qreal LDETCheckBtn::borderOpacity() const { return m_borderOpacity; }
void LDETCheckBtn::setBorderOpacity(qreal opacity)
{
    if (!qFuzzyCompare(m_borderOpacity, opacity)) {
        m_borderOpacity = qBound(0.0, opacity, 1.0);
        update();
    }
}

bool LDETCheckBtn::isBorderless() const { return m_borderless; }
void LDETCheckBtn::setBorderless(bool borderless)
{
    if (m_borderless != borderless) {
        m_borderless = borderless;
        updateGeometry();
        update();
    }
}

void LDETCheckBtn::setChecked(bool checked)
{
    m_isProgrammaticChange = true;
    QCheckBox::setChecked(checked);
    m_isProgrammaticChange = false;
    startSliderAnimation(checked ? 1.0 : 0.0);
}

void LDETCheckBtn::setCheckedSlot(bool checked)
{
    setChecked(checked);
}

void LDETCheckBtn::nextCheckState()
{
    QCheckBox::nextCheckState();
    startSliderAnimation(isChecked() ? 1.0 : 0.0);
}

void LDETCheckBtn::startSliderAnimation(qreal targetPos)
{
    if (m_animation->state() == QAbstractAnimation::Running) {
        m_animation->stop();
    }
    m_animation->setStartValue(m_sliderPosition);
    m_animation->setEndValue(targetPos);
    m_animation->start();
}

void LDETCheckBtn::startBorderAnimation(qreal targetOpacity)
{
    if (m_borderAnimation->state() == QAbstractAnimation::Running) {
        m_borderAnimation->stop();
    }
    m_borderAnimation->setStartValue(m_borderOpacity);
    m_borderAnimation->setEndValue(targetOpacity);
    m_borderAnimation->start();
}

QSize LDETCheckBtn::sizeHint() const
{
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(text());
    int textHeight = fm.height();

    int margin = m_borderless ? 3 : 10;
    int width = textWidth + TEXT_SWITCH_SPACING + SWITCH_WIDTH + margin * 2;

    int height;
    if (!m_briefTip.isEmpty()) {
        QFont tipFont = font();
        tipFont.setPointSize(m_tipFontSize);
        QFontMetrics tipFm(tipFont);
        height = margin + qMax(textHeight, SWITCH_HEIGHT) + TIP_TEXT_SPACING + tipFm.height() + margin;
    } else {
        height = margin * 2 + qMax(textHeight, SWITCH_HEIGHT);
    }

    return QSize(width, height);
}

QSize LDETCheckBtn::minimumSizeHint() const
{
    return QSize(SWITCH_WIDTH + 20, SWITCH_HEIGHT + 10);
}

QRect LDETCheckBtn::calculateSwitchRect() const
{
    int margin = m_borderless ? 3 : 10;
    int switchX = width() - SWITCH_WIDTH - margin;
    int switchY;

    if (!m_briefTip.isEmpty()) {
        int contentHeight = qMax(fontMetrics().height(), SWITCH_HEIGHT);
        switchY = margin + (contentHeight - SWITCH_HEIGHT) / 2;
    } else {
        switchY = (height() - SWITCH_HEIGHT) / 2;
    }

    return QRect(switchX, switchY, SWITCH_WIDTH, SWITCH_HEIGHT);
}

QRectF LDETCheckBtn::calculateSliderRect() const
{
    QRect switchRect = calculateSwitchRect();
    qreal sliderSize = switchRect.height() - 2 * SLIDER_MARGIN;
    qreal sliderX = switchRect.x() + SLIDER_MARGIN +
                    m_sliderPosition * (switchRect.width() - 2 * SLIDER_MARGIN - sliderSize);
    qreal sliderY = switchRect.y() + SLIDER_MARGIN;

    return QRectF(sliderX, sliderY, sliderSize, sliderSize);
}

void LDETCheckBtn::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int contentMargin = m_borderless ? 3 : 10;

    if (!m_borderless) {
        QRect borderRect = rect().adjusted(1, 1, -1, -1);
        auto *theme = LDETTheme::instance();
        QColor bgColor = theme->widgetBackgroundColor();
        QColor normalBorder = theme->normalBorderColor();
        QColor highlightBorder = palette().color(QPalette::Highlight);

        QColor borderColor = LDETDrawUtils::blendColors(normalBorder, highlightBorder, m_borderOpacity);

        LDETDrawUtils::drawRoundedRect(&painter, borderRect, 5, bgColor, borderColor, 1);
    }

    QRect switchRect = calculateSwitchRect();
    QRectF sliderRect = calculateSliderRect();

    QFontMetrics fm(font());
    int textHeight = fm.height();
    int textY;

    if (!m_briefTip.isEmpty()) {
        int contentHeight = qMax(textHeight, SWITCH_HEIGHT);
        textY = contentMargin + (contentHeight - textHeight) / 2 + fm.ascent();
    } else {
        textY = (height() - textHeight) / 2 + fm.ascent();
    }

    painter.setFont(font());
    painter.setPen(palette().color(QPalette::Text));
    painter.drawText(contentMargin, textY, text());

    QPainterPath switchPath;
    switchPath.addRoundedRect(switchRect, BORDER_RADIUS, BORDER_RADIUS);

    QColor bgColor = (m_sliderPosition > 0.5) ? m_activeColor : m_inactiveColor;
    painter.fillPath(switchPath, bgColor);

    QPainterPath sliderPath;
    sliderPath.addEllipse(sliderRect);
    painter.fillPath(sliderPath, m_sliderColor);

    if (!m_briefTip.isEmpty()) {
        QFont tipFont = font();
        tipFont.setPointSize(m_tipFontSize);
        QFontMetrics tipFm(tipFont);

        painter.setFont(tipFont);
        painter.setPen(m_tipHighlightColor);

        int contentHeight = qMax(fm.height(), SWITCH_HEIGHT);
        int tipY = contentMargin + contentHeight + TIP_TEXT_SPACING + tipFm.ascent();
        painter.drawText(contentMargin, tipY, m_briefTip);
    }
}

void LDETCheckBtn::resizeEvent(QResizeEvent *event)
{
    QCheckBox::resizeEvent(event);
    update();
}

void LDETCheckBtn::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QRect switchRect = calculateSwitchRect();
        m_pressedOnSwitch = switchRect.contains(event->pos());
        if (m_pressedOnSwitch) {
            event->accept();
            return;
        }
    }
    QCheckBox::mousePressEvent(event);
}

void LDETCheckBtn::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_pressedOnSwitch) {
        QRect switchRect = calculateSwitchRect();
        if (switchRect.contains(event->pos())) {
            nextCheckState();
            event->accept();
        }
        m_pressedOnSwitch = false;
        return;
    }
    QCheckBox::mouseReleaseEvent(event);
}

void LDETCheckBtn::enterEvent(QEnterEvent *event)
{
    QCheckBox::enterEvent(event);
    startBorderAnimation(1.0);
}

void LDETCheckBtn::leaveEvent(QEvent *event)
{
    QCheckBox::leaveEvent(event);
    startBorderAnimation(0.0);
}

void LDETCheckBtn::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        nextCheckState();
        event->accept();
        return;
    }
    QCheckBox::keyPressEvent(event);
}
