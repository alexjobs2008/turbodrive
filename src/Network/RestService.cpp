#include "RestService.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

RestService::RestService(const QString& name, const QString& address,
						QObject* parent)
	: QObject(parent)
	, m_name(name)
	, m_address(address)
	, m_currentRequest(0)
{
}

RestService::~RestService()
{
}

bool RestService::queuesAreEmpty() const
{
	return m_authenticatedRequests.isEmpty()
		&& m_unauthenticatedRequests.isEmpty();
}

const QString& RestService::name() const
{
	return m_name;
}

const QString& RestService::address() const
{
	return m_address;
}

QString RestService::toString() const
{
	QVariantMap map;

#define MAKE_PAIR(value) QLatin1String(#value), QVariant(value)
	map.insert(MAKE_PAIR(m_name));
	map.insert(MAKE_PAIR(m_address));
#undef MAKE_PAIR
	map.insert(QLatin1String("m_currentRequest"),
				m_currentRequest.isNull()
				? QLatin1String("nullptr")
				: m_currentRequest->toString());

	return QJsonDocument(QJsonObject::fromVariantMap(map)).toJson();
}
