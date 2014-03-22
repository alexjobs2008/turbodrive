#include "ExtraLabels.h"
#include <QtWidgets/QBoxLayout>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>

namespace CommonUI
{

LinkLabel::LinkLabel(QString text, QString link, QWidget *parent)
	: QLabel(parent)
	, state(Normal)
	, link(link)
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
//			if (text().contains("<a "))
			setCursor(Qt::PointingHandCursor);
			state = Hover;
			setStyleSheet(styleSheet());
		}
		if (e->type() == QEvent::HoverLeave)
		{
			setCursor(Qt::ArrowCursor);
			state = Normal;
			setStyleSheet(styleSheet());
		}
	}

	return QLabel::event(e);
}

QString LinkLabel::getStateString()
{
	switch (state)
	{
	case Hover: return "hover";
	case Normal:
	default: return "normal";
	}
}

void LinkLabel::mousePressEvent(QMouseEvent *event)
{
	if ((event->button() == Qt::LeftButton))
	{
		emit linkActivated(link);
	}
}


ButtonLabel::ButtonLabel(QWidget *parent)
	: QLabel(parent)
	, state(State::Normal)
{
}

void ButtonLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
	if ((state != Disabled)
		&& (event->button() == Qt::LeftButton))
	{
		emit doubleClicked();
	}
	QLabel::mouseDoubleClickEvent(event);
}

void ButtonLabel::mousePressEvent(QMouseEvent *event)
{
	if ((state != Disabled)
		&& (event->button() == Qt::LeftButton))
	{
		state = Pressed;
		setStyleSheet(styleSheet());
	}
	QLabel::mousePressEvent(event);
}

void ButtonLabel::mouseReleaseEvent(QMouseEvent *event)
{
	if ((state == Pressed)
		&& (event->button() == Qt::LeftButton)
		&& rect().contains(event->pos()))
	{
		state = Hover;
		setStyleSheet(styleSheet());
		emit clicked();
	}
	else if (state != Disabled)
	{
		state = Normal;
		setStyleSheet(styleSheet());
	}
	QLabel::mouseReleaseEvent(event);
}

void ButtonLabel::enterEvent(QEvent *event)
{
	if (state != Disabled)
	{
		state = Hover;
		setStyleSheet(styleSheet());
	}
	QLabel::enterEvent(event);
}

void ButtonLabel::leaveEvent(QEvent *event)
{
	if (state == Hover)
	{
		state = Normal;
		setStyleSheet(styleSheet());
	}
	QLabel::leaveEvent(event);
}

void ButtonLabel::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::EnabledChange)
	{
		state = isEnabled() ? Normal : Disabled;
		setStyleSheet(styleSheet());
	}
	QLabel::changeEvent(event);
}

QString ButtonLabel::getStateString()
{
	switch (state)
	{
		case Hover: return "hover";
		case Pressed: return "pressed";
		case Disabled: return "disabled";
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
