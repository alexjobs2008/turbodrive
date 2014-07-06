#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <QtCore/QObject>

#include <functional>

namespace Drive {

class WatchDog: public QObject
{
	Q_OBJECT

public:
	explicit WatchDog(std::function<void ()> callback, int intervalMSec = 10000);

	void restart();
	void stop();

protected:
	virtual void timerEvent(QTimerEvent*) override;

private:
	WatchDog(const WatchDog&) = delete;
	WatchDog& operator=(const WatchDog&) = delete;

private:
	std::function<void ()> m_callback;
	const int m_intervalMSec;
	int m_timerId;
};

}

#endif // WATCHDOG_H
