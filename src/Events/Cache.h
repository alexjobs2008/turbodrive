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

	int id(const QString& remotePath, bool forParent = false) const;
	RemoteFileDesc fileDescriptor(const QString& remotePath, bool forParent = false) const;

	void clear();

	void addRoot(const RemoteFileDesc&);
	void addFile(const RemoteFileDesc&);

private:
	QString toString() const;

	void insertIntoPathMap(const RemoteFileDesc&);

	bool isRootId(const int id) const;

private:
	mutable QMutex m_mutex;

	std::vector<int> m_rootIds;

	QMap<QString, RemoteFileDesc> m_pathMap;
	QMap<int, RemoteFileDesc> m_idMap;

};

}

#endif // LOCAL_CACHE_H
