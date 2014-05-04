#ifndef LOCAL_DISPATCHER
#define LOCAL_DISPATCHER

#include <iostream>
#include <memory>
#include <QtCore/QObject>
#include "3rdparty/efsw/include/efsw/efsw.hpp"

namespace Drive
{

struct LocalFileEvent;
class LocalListener;


class LocalFileEventNotifier: public QObject
{
	Q_OBJECT

public:
	static LocalFileEventNotifier& instance();

	Q_SLOT void resetFolder();
	Q_SLOT void stop();

	Q_SIGNAL void newLocalFileEvent(const LocalFileEvent& event);

private:
	LocalFileEventNotifier(QObject* parent = nullptr);
	Q_DISABLE_COPY(LocalFileEventNotifier)

private:
	efsw::FileWatcher m_fileWatcher;
	std::auto_ptr<LocalListener> m_listener;
	efsw::WatchID m_watchID;
};

class LocalListener : public QObject, public efsw::FileWatchListener
{
	Q_OBJECT

public:
	LocalListener(QObject* parent = nullptr);

	virtual void handleFileAction(efsw::WatchID watchid,
			const std::string& dir, const std::string& filename,
			efsw::Action action, std::string oldFilename = "") override;

	Q_SIGNAL void newLocalFileEvent(const LocalFileEvent& localFileEvent);
};

}

#endif
