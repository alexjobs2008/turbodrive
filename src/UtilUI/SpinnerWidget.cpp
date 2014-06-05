#include "SpinnerWidget.h"
#include "PixmapAnimation.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>

namespace CommonUI
{

SpinnerWidget::SpinnerWidget(const QString& text, const QString& imgPath,
							int animationDelay, QWidget *parent)
	: QFrame(parent)
	, animationDelay(animationDelay)
{
	setContentsMargins(0, 0, 0, 0);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	spinner = new QLabel(this);
	spinner->setObjectName("spinner");

	wait = new QLabel(text, this);
	wait->setObjectName("wait");

	substText = new QLabel(this);
	substText->setObjectName("substText");
	substText->setVisible(false);

	spinnerAnimation = new PixmapAnimation(imgPath, this);

	connect(spinnerAnimation, &PixmapAnimation::nextPixmap,
			spinner, &QLabel::setPixmap);

	layout->addStretch(1);
	layout->addWidget(spinner);
	layout->addWidget(wait);
	layout->addWidget(substText);
	layout->addStretch(1);

	setOn(false);
}

void SpinnerWidget::setOn(bool on)
{
	substText->setVisible(false);
	spinner->setVisible(on);
	wait->setVisible(on);

	if (on)
	{
		spinnerAnimation->start(animationDelay);
	}
	else
	{
		spinnerAnimation->stop();
	}
}

void SpinnerWidget::setSubstText(const QString& text)
{
	substText->setText(text);
	substText->setVisible(true);
}

}
