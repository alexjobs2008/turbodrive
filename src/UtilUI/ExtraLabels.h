#ifndef EXTRA_LABELS_H
#define EXTRA_LABELS_H

#include <QtWidgets/QLabel>

namespace CommonUI
{

class LinkLabel : public QLabel
{
	Q_OBJECT
	Q_PROPERTY(QString state READ getStateString())

public:

	enum State
	{
		Normal,
		Hover
	};

	LinkLabel(QString text, QString link, QWidget *parent);

	QString getStateString();

protected:
	bool event(QEvent *e);
	void mousePressEvent(QMouseEvent *event);

private:
	State state;
	QString link;
};


class ButtonLabel : public QLabel
{
	Q_OBJECT
	Q_PROPERTY(QString state READ getStateString())

public:

	enum State
	{
		Normal,
		Hover,
		Pressed,
		Disabled
	};

	ButtonLabel(QWidget *parent = 0);
	QString getStateString();

protected:
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
	void changeEvent(QEvent *event);

	State state;

signals:
	void doubleClicked();
	void clicked();
};

class ElidedLabel : public QLabel
{
	Q_OBJECT

public:
	ElidedLabel(QWidget *parent = 0);

	void setText(const QString &text);
	QSize sizeHint() const;

protected:
	void resizeEvent(QResizeEvent *event);
	void changeEvent(QEvent *event);

private:
	void updateFullSize();

	QString m_originalText;
	QSize m_fullSize;
};

}
#endif // EXTRA_LABELS_H
