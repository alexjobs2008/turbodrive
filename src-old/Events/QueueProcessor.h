#ifndef QUEUE_PROCESSOR_H
#define QUEUE_PROCESSOR_H

//#include "EventTypes.h"

#include <QtCore/QObject>
#include <QtCore/QQueue>

namespace Drive
{

class QueueProcessor : public QObject
{
	Q_OBJECT
public:

	enum State
	{
		Finished = 0,
		Stopped,
		Processing
	};

	QueueProcessor(QObject *parent = 0);

//	void addEvent(FileEvent* fileEvent);
	void start();
	void stop();
	void cancelAll();

private slots:
	void onFinishProcessingEvent();

private:
	void next();

	State state;
//	QQueue<FileEvent*> q;
};

}

#endif // QUQUE_PROCESSOR_H