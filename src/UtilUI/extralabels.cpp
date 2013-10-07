#include "extralabels.h"
#include <QtWidgets/QBoxLayout>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>

namespace AuxWidgets
{

LinkLabel::LinkLabel(QString text, QString link, QWidget *parent)
    : QLabel(parent)
    , m_hovered(false)
    , m_link(link)
{
    setAttribute(Qt::WA_Hover);
    setText(text);
}

bool LinkLabel::event(QEvent *e)
{
    if (isEnabled())
    {
        if (e->type() == QEvent::HoverEnter)
        {
            if (text().contains("<a>"))
                setCursor(Qt::PointingHandCursor);
            m_hovered = true;
            setStyleSheet(styleSheet());
        }
        if (e->type() == QEvent::HoverLeave)
        {
            setCursor(Qt::ArrowCursor);
            m_hovered = false;
            setStyleSheet(styleSheet());
        }
    }

    return QLabel::event(e);
}

void LinkLabel::mousePressEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton))
    {
        emit linkActivated(m_link);
    }
}


ButtonLabel::ButtonLabel(QWidget *parent)
    : QLabel(parent)
    , m_state(sNormal)
{
}

void ButtonLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    if ((m_state != sDisabled)
        && (event->button() == Qt::LeftButton))
    {
        emit doubleClicked();
    }
    QLabel::mouseDoubleClickEvent(event);
}

void ButtonLabel::mousePressEvent(QMouseEvent *event)
{
    if ((m_state != sDisabled)
        && (event->button() == Qt::LeftButton))
    {
        m_state = sPressed;
        setStyleSheet(styleSheet());
    }
    QLabel::mousePressEvent(event);
}

void ButtonLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if ((m_state == sPressed)
        && (event->button() == Qt::LeftButton)
        && rect().contains(event->pos()))
    {
        m_state = sHovered;
        setStyleSheet(styleSheet());
        emit clicked();
    }
    else if (m_state != sDisabled)
    {
        m_state = sNormal;
        setStyleSheet(styleSheet());
    }
    QLabel::mouseReleaseEvent(event);
}

void ButtonLabel::enterEvent(QEvent *event)
{
    if (m_state != sDisabled)
    {
        m_state = sHovered;
        setStyleSheet(styleSheet());
    }
    QLabel::enterEvent(event);
}

void ButtonLabel::leaveEvent(QEvent *event)
{
    if (m_state == sHovered)
    {
        m_state = sNormal;
        setStyleSheet(styleSheet());
    }
    QLabel::leaveEvent(event);
}

void ButtonLabel::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange)
    {
        m_state = isEnabled() ? sNormal : sDisabled;
        setStyleSheet(styleSheet());
    }
    QLabel::changeEvent(event);
}

QString ButtonLabel::getStateString()
{
    switch (m_state)
    {
        case sHovered: return "hovered";
        case sPressed: return "pressed";
        case sDisabled: return "disabled";
        default: return "normal";
    }
}


ElidedLabel::ElidedLabel(QWidget *parent)
    : QLabel(parent)
    , m_originalText(QString())
    , m_fullSize(QSize(-1, -1))
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

void ElidedLabel::setText(const QString &text)
{
    m_originalText = text;
    updateFullSize();
    QLabel::setText(text);
}

QSize ElidedLabel::sizeHint() const
{
    return m_fullSize;
}

void ElidedLabel::resizeEvent(QResizeEvent *event)
{
    int leftMargin, topMargin, rightMargin, bottomMargin;
    getContentsMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin);

    QFontMetrics fm(font());
    QString newText = fm.elidedText(m_originalText, Qt::ElideRight, event->size().width() - leftMargin - rightMargin);

    if (newText != m_originalText)
    {
        setToolTip(m_originalText);
    }
    else
    {
        setToolTip(QString());
    }

    if (text() != newText)
    {
        QLabel::setText(newText);
        update();
    }

    QLabel::resizeEvent(event);
}

void ElidedLabel::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::FontChange)
    {
        updateFullSize();
    }
    QLabel::changeEvent(event);
}

void ElidedLabel::updateFullSize()
{
    QFontMetrics fm(font());
    m_fullSize = fm.size(Qt::TextSingleLine, m_originalText);
}

}
