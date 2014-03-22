#include "QueueProcessor.h"

#include <QtCore/QThread>

namespace Drive
{

QueueProcessor::QueueProcessor(QObject *parent)
	: QObject(parent)
	, state(Finished)
{
}

// void QueueProcessor::addEvent(FileEvent* fileEvent)
// {
// 	q.enqueue(fileEvent);
// 	if (state == Finished)
// 	{
// 		next();
// 	}
// }

void QueueProcessor::start()
{
	next();
}

void QueueProcessor::next()
{
// 	if (q.isEmpty())
// 	{
// 		state = Finished;
// 		return;
// 	}
// 	
// 	FileEventHandler* fileEvent = q.dequeue();
// 
// 	QThread *thread = new QThread(this);
// 	fileEvent->moveToThre
}

void QueueProcessor::stop()
{

}

void QueueProcessor::cancelAll()
{
	stop();
//	q.clear();
}

void QueueProcessor::onFinishProcessingEvent()
{

}

}