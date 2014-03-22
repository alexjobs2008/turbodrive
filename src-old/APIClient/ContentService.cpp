#include "ContentService.h"

#include "Settings/settings.h"
#include "QsLog/QsLog.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QUrlQuery>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUrlQuery>

#include <QtNetwork/QHttpMultiPart>

namespace Drive
{

ContentRestResourceRef ContentRestResource::create()
{
    ContentRestResourceRef resource = RestResource::create<ContentRestResource>();
    return resource;
}

void ContentRestResource::upload(const QString& filePath)
{
    QHttpMultiPart *multiPart = 
        new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

    QHttpPart *part1 = 
}

QString ContentRestResource::path() const
{
    return "/api/v1/content/";
}

QString ContentRestResource::service() const
{
    return "ContentService";
}

bool ContentRestResource::restricted() const
{
    return true;
}

bool ContentRestResource::processPostResponse(int status,
                                              const QByteArray& data,
                                              const HeaderList& headers)
{

    return false;
}

bool ContentRestResource::processGetResponse(int status,
                                             const QByteArray& data,
                                             const HeaderList& headers)
{

    return false;
}

}