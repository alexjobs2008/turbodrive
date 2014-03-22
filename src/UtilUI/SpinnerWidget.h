#ifndef SPINNER_WIDGET_H
#define SPINNER_WIDGET_H

#include <QtWidgets/QFrame>

class QLabel;

namespace CommonUI
{

class PixmapAnimation;

class SpinnerWidget : public QFrame
{
	Q_OBJECT
public:

	// text = "Please wait..."
	// imgPath = ":/spinner/24-" for 24-0001.png, 24-0002.png, ...
	SpinnerWidget(const QString& text, const QString& imgPath,
		int animationDelay = 40, QWidget *parent = 0);

	void setOn(bool on = true);
	void setSubstText(const QString& text);

private:
	QLabel *spinner;
	QLabel *wait;
	QLabel *substText;
	PixmapAnimation *spinnerAnimation;
	int animationDelay;
};


}


#endif // SPINNER_WIDGET_H