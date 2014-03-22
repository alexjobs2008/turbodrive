#include "RestService.h"

RestService::RestService(const QString& name, const QString& address,
						QObject* parent)
	: QObject(parent)
	, serviceName(name)
	, serviceAddress(address)
	, currentRequest(0)
{
}

RestService::~RestService()
{
}

const QString& RestService::name() const
{
	return serviceName;
}

const QString& RestService::address() const
{
	return serviceAddress;
}