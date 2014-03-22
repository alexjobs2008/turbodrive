#ifndef PROFILE_SERVICE_H
#define PROFILE_SERVICE_H

#include "Network/RestResource.h"

#include <QtCore/QVariantMap>

class QJsonObject;

namespace Drive
{

class ProfileRestResource;
typedef QSharedPointer<ProfileRestResource> ProfileRestResourceRef;

class ProfileRestResource : public RestResource
{
	Q_OBJECT
public:

	static ProfileRestResourceRef create();

	void requestProfileData();

	virtual QString path() const;
	virtual QString service() const;
	virtual bool restricted() const;

signals:
	void profileDataReceived(const QJsonObject& data);
	void profileDataError();

private:
	virtual bool processGetResponse(int status, const QByteArray& data,
		const HeaderList& headers);


};

}

#endif // PROFILE_SERVICE_H