#ifndef PIXMAP_ANIMATION_H
#define PIXMAP_ANIMATION_H

#include <QtCore/QObject>
#include <QtCore/QVector>

class QPixmap;

namespace CommonUI
{

class PixmapAnimation : public QObject
{
	Q_OBJECT
public:
	explicit PixmapAnimation(const QString &pathPrefix, QObject *parent = 0);
	~PixmapAnimation();

	bool start(int delay = 40);
	void stop();

signals:
	void nextPixmap(const QPixmap& pixmap);

protected:
	virtual void timerEvent(QTimerEvent * event);

private:
	QVector<QPixmap> frames;
	int currentFrame;
	int timerId;
};

}

#endif
