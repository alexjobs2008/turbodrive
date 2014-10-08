#include "AuxWidgets.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QBoxLayout>
#include <QtGui/QRegExpValidator>

namespace CommonUI
{

LabeledEdit::LabeledEdit(QString leftCaption,
						Type type,
						QString rightCaption,
						uint leftLabelMinWidth, QString regexp,
						uint editMaxWidth, QWidget *parent)
	: QFrame(parent)
	, type(type)
	, _rightLabel(0)
{
    setContentsMargins(0, 1, 0, 1);
	_layout = new QHBoxLayout(this);
    _layout->setContentsMargins(0, 1, 0, 1);
	_layout->setSpacing(0);

	_leftLabel = new QLabel(leftCaption, this);
	_leftLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	_leftLabel->setMinimumWidth(leftLabelMinWidth);
	_layout->addWidget(_leftLabel);

	_lineEdit = new QLineEdit(this);
	if (!regexp.isEmpty())
	{
		QRegExp re(regexp);
		QRegExpValidator *validator = new QRegExpValidator(re, this);
		_lineEdit->setValidator(validator);
	}

	_lineEdit->setAlignment(type == Type::Numeric
		? Qt::AlignRight
		: Qt::AlignLeft);

	_leftLabel->setBuddy(_lineEdit);

	if (editMaxWidth)
	{
		_lineEdit->setMaximumWidth(editMaxWidth);
		_layout->addWidget(_lineEdit);
	}
	else
	{
		_layout->addWidget(_lineEdit, 1);
	}

	if (!rightCaption.isEmpty())
	{
		_rightLabel = new QLabel(rightCaption, this);
		_layout->addWidget(_rightLabel, 1);
	}

	if (editMaxWidth && rightCaption.isEmpty())
		_layout->addStretch(1);
}

bool LabeledEdit::getStateString() const
{
	return isEnabled();
}

void LabeledEdit::setName(const QString& name)
{
	setObjectName(name);
	_leftLabel->setObjectName(QString("%1Label").arg(name));

	if (_rightLabel)
		_rightLabel->setObjectName(QString("%1SecondLabel").arg(name));

	_lineEdit->setObjectName(QString("%1Edit").arg(name));
}

QString LabeledEdit::text() const
{
	return _lineEdit->text();
}

void LabeledEdit::setText(QString text)
{
	_lineEdit->setText(text);
}

int LabeledEdit::number()
{
	return type == Type::Numeric
		? _lineEdit->text().replace("K", "000").replace("M", "000000").toInt()
		: 0;
}

QHBoxLayout* LabeledEdit::layout() const
{
	return _layout;
}

QLabel* LabeledEdit::leftLabel() const
{
	return _leftLabel;
}

QLineEdit* LabeledEdit::lineEdit() const
{
	return _lineEdit;
}

QLabel* LabeledEdit::rightLabel() const
{
	return _rightLabel;
}

LabeledLabel::LabeledLabel(QString leftCaption, QString rightCaption,
						uint leftLabelMinWidth, QWidget *parent)
	: QFrame(parent)
{
	setContentsMargins(0, 0, 0, 0);
	QHBoxLayout *bl = new QHBoxLayout(this);
	bl->setContentsMargins(0, 0, 0, 0);

	QLabel *leftLabel = new QLabel(leftCaption, this);
	leftLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	leftLabel->setMinimumWidth(leftLabelMinWidth);

	QLabel *rightLabel = new QLabel(rightCaption, this);
	rightLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	bl->addWidget(leftLabel);
	bl->addWidget(rightLabel);
	bl->addStretch(1);
}

MinMaxEdit::MinMaxEdit(QString defaultMin, QString defaultMax,
					QString leftCaption, QString middleCaption,
					QString rightCaption, uint leftLabelMinWidth,
					uint middleLabelMinWidth, uint editMaxWidth,
					QString regexpMin, QString regexpMax, QWidget *parent)

	: QFrame(parent)
{
	setContentsMargins(0, 0, 0, 0);
	QHBoxLayout *bl = new QHBoxLayout(this);
	bl->setContentsMargins(0, 0, 0, 0);

	QLabel *lLeft = new QLabel(leftCaption, this);
	QLabel *lMiddle = new QLabel(middleCaption, this);
	QLabel *lRight = new QLabel(rightCaption, this);

	lLeft->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	lLeft->setMinimumWidth(leftLabelMinWidth);
	lMiddle->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	lMiddle->setMinimumWidth(middleLabelMinWidth);

	leMin = new QLineEdit(this);
	leMax = new QLineEdit(this);
	leMin->setAlignment(Qt::AlignRight);
	leMax->setAlignment(Qt::AlignRight);
	leMin->setMaximumWidth(editMaxWidth);
	leMax->setMaximumWidth(editMaxWidth);

	bl->addWidget(lLeft);
	bl->addWidget(leMin);
	bl->addWidget(lMiddle);
	bl->addWidget(leMax);
	bl->addWidget(lRight);
	bl->addStretch(1);

	if (!regexpMin.isEmpty())
	{
		QRegExp re(regexpMin);
		QRegExpValidator *validator = new QRegExpValidator(re, this);
		leMin->setValidator(validator);
	}

	if (!regexpMax.isEmpty())
	{
		QRegExp re(regexpMax);
		QRegExpValidator *validator = new QRegExpValidator(re, this);
		leMax->setValidator(validator);
	}

	leMin->setText(defaultMin);
	leMax->setText(defaultMax);
}

int MinMaxEdit::minInt() const
{
	return prepare(leMin->text()).toInt();
}

int MinMaxEdit::maxInt() const
{
	return prepare(leMax->text()).toInt();
}

QString MinMaxEdit::minString() const
{
	return leMin->text();
}

QString MinMaxEdit::maxString() const
{
	return leMax->text();
}

QString MinMaxEdit::prepare(QString value) const
{
    return value.replace("K", "000").replace("M", "000000");
}

MessageWindow::MessageWindow(QWidget *parent) :
    QWidget(parent, Qt::ToolTip /*| Qt::CustomizeWindowHint | Qt::FramelessWindowHint*/)
{
    setStyleSheet("color: red; background-color: white");

    textLabel = new QLabel("", this);
    textLabel->adjustSize();
    textLabel->setStyleSheet("color: white; background-color: red; font-weight: bold");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(textLabel);
    resize(textLabel->size());
    setStyleSheet("color: white; background-color: red; font-weight: bold");
}

void MessageWindow::showText(QString text)
{
    textLabel->setText(text);
    textLabel->adjustSize();
    resize(textLabel->size());
    show();
}

}
