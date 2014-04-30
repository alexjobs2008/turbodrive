#ifndef LOCAL_CACHE_H
#define LOCAL_CACHE_H

#include <QtCore/QObject>
#include <QtCore/QMutex>

#include "APIClient/ApiTypes.h"

namespace Drive
{

class LocalCache : public QObject
{
	Q_OBJECT

public:
	static LocalCache& instance();

	RemoteFileDesc file(const QString& remotePath, bool forParent = false) const;
	RemoteFileDesc file(int id, bool forParent = false) const;

	void clear();

	void addRoot(const RemoteFileDesc&);
	void addFile(const RemoteFileDesc&);

	QString fullPath(const RemoteFileDesc&) const;

private:
	RemoteFileDesc fileById(int id) const;

	bool removeById(int id);

	QString toString() const;

private:
	mutable QMutex m_mutex;
	std::map<QString, RemoteFileDesc> m_files;
};

}

#endif // LOCAL_CACHE_H
