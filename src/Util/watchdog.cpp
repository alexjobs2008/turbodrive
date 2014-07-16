#include "watchdog.h"

#include <QtNetwork/QTcpSocket>
#include <QsLog/QsLog.h>

using namespace std;

namespace Drive
{

WatchDog::WatchDog(std::function<void ()> callback, int intervalMSec)
	: m_callback(callback)
	, m_intervalMSec(intervalMSec)
	, m_timerId(0)
{
}

void WatchDog::restart()
{
	stop();
	m_timerId = startTimer(m_intervalMSec);
	Q_ASSERT(m_timerId != 0);
}

void WatchDog::stop()
{
	//Q_ASSERT(m_timerId != 0);
	if (m_timerId != 0)
	{
		killTimer(m_timerId);
		m_timerId = 0;
	}
}

void WatchDog::timerEvent(QTimerEvent*)
{
	QTcpSocket messenger;
	messenger.connectToHost("www.google.com", 80);
	if(!messenger.waitForConnected(3000))
	{
		stop();
		m_callback();
	}
}

}
