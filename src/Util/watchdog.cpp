#include "watchdog.h"

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
	if (m_timerId != 0)
	{
		stop();
	}
	m_timerId = startTimer(m_intervalMSec);
	Q_ASSERT(m_timerId != 0);
	QLOG_DEBUG() << "Watchdog started.";
}

void WatchDog::stop()
{
	//Q_ASSERT(m_timerId != 0);
	if (m_timerId != 0)
	{
		killTimer(m_timerId);
		m_timerId = 0;
		QLOG_DEBUG() << "Watchdog stopped.";
	}
}

void WatchDog::timerEvent(QTimerEvent*)
{
	m_callback();
	QLOG_DEBUG() << "Watchdog event, callback called.";
	stop();
}

}
