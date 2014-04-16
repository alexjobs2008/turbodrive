#ifndef REST_SERVICE_H
#define REST_SERVICE_H

#include "RestResource.h"

#include <QtCore/QObject>
#include <QtCore/QQueue>

class GeneralRestDispatcher;

class RestService : public QObject
{
	Q_OBJECT
public:
	explicit RestService(const QString& name, const QString& address,
		QObject* parent = 0);

	virtual ~RestService();

	bool queuesAreEmpty() const;

	const QString& name() const;
	const QString& address() const;

	QString toString() const;

private:
	const QString m_name;
	const QString m_address;
	RestResource::RequestRef m_currentRequest;
	QQueue<RestResource::RequestRef> m_authenticatedRequests;
	QQueue<RestResource::RequestRef> m_unauthenticatedRequests;

	friend class GeneralRestDispatcher;
};

#endif // REST_SERVICE_H
