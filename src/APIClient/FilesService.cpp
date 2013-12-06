#include "FilesService.h"

#include "Settings/settings.h"
#include "QsLog/QsLog.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QUrlQuery>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUrlQuery>
#include <QtCore/QStringList>

#define FILES_SERVICE_NAME "FilesService"

namespace Drive
{

OnlineRestResourceRef OnlineRestResource::create()
{
    OnlineRestResourceRef resource = RestResource::create<OnlineRestResource>();
    return resource;
}

void OnlineRestResource::ping()
{
    QLOG_INFO() << "Going online...";

    doOperation(QNetworkAccessManager::GetOperation, QByteArray(), HeaderList());
}

QString OnlineRestResource::path() const
{
    return "_online";
}

QString OnlineRestResource::service() const
{
    return FILES_SERVICE_NAME;
}

bool OnlineRestResource::restricted() const
{
    return true;
}

bool OnlineRestResource::processGetResponse(int status,
                                          const QByteArray& data,
                                          const HeaderList& headers)
{
    QLOG_INFO() << "Ping status " << status;

	if (status == 200)
	{
		emit pingOk();
	}
	else
	{
		emit pingError();
	}		

    return true;    
}

}