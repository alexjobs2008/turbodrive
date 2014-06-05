#ifndef ABOUT_WIDGET_H
#define ABOUT_WIDGET_H

#include <QtWidgets/QFrame>

namespace Drive
{

class AboutWidget : public QFrame
{
	Q_OBJECT
public:
	AboutWidget(QWidget *parent = 0);

};

}

#endif ABOUT_WIDGET_H