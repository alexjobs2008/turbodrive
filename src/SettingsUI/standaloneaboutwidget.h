#ifndef STANDALONE_ABOUT_WIDGET_H
#define STANDALONE_ABOUT_WIDGET_H

#include <QtWidgets/QFrame>
#include <QtWidgets/QStyledItemDelegate>
#include <QtGui/QResizeEvent>

namespace Drive
{

class StandaloneAboutWidget : public QFrame
{
	Q_OBJECT

public:
	static StandaloneAboutWidget& instance();

private:
    StandaloneAboutWidget(const StandaloneAboutWidget&) = delete;
    StandaloneAboutWidget& operator=(const StandaloneAboutWidget&) = delete;

	StandaloneAboutWidget(QWidget *parent = 0);

	void createHeader(QLayout* layout);
	void createBody(QLayout* layout);
};

}

#endif // STANDALONE_ABOUT_WIDGET_H
