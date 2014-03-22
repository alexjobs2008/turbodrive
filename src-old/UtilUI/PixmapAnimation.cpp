#include "pixmapAnimation.h"

#include <QtGui/QPixmap>

namespace CommonUI
{

PixmapAnimation::PixmapAnimation(const QString &pathPrefix, QObject *parent)
    : QObject(parent)
    , timerId(0)
{
    int frameNum = 1;
    QString fileName;

    QPixmap pixmap;
    
    fileName.sprintf("%04d.png", frameNum);
    while (pixmap.load(pathPrefix + fileName))
    {
        frames << pixmap;
        fileName.sprintf("%04d.png", ++frameNum);
    }  
}

PixmapAnimation::~PixmapAnimation()
{
    if (timerId)    
        killTimer(timerId);

    frames.clear();
}

bool PixmapAnimation::start(int delay)
{
    if (!frames.size())
        return false;

    stop();

    emit nextPixmap(frames.at(currentFrame));

    if (frames.size() > 1)    
        timerId = this->startTimer(delay);

    return true;
}

void PixmapAnimation::stop()
{
    if (timerId)
    {
        killTimer(timerId);
        timerId = 0;
    }

    currentFrame = 0;
}

void PixmapAnimation::timerEvent(QTimerEvent *event)
{
    if (++currentFrame > frames.size() - 1)
        currentFrame = 0;

    emit nextPixmap(frames.at(currentFrame));
}

}