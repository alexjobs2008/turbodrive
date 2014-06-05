#ifndef AUTHENTICATION_SERVICE_H
#define AUTHENTICATION_SERVICE_H

#include "Network/RestResource.h"

namespace Drive
{

class AuthRestResource;
typedef QSharedPointer<AuthRestResource> AuthRestResourceRef;

class AuthRestResource : public RestResource
{
	Q_OBJECT
public:

	struct Input
	{
		QString username;
		QString password;
	};

	struct Output
	{
		QString token;
	};

	void login(const Input& data);

	static AuthRestResourceRef create();

	virtual QString path() const;
	virtual QString service() const;
	virtual bool restricted() const;

signals:
	void loginSucceeded(const QString& username, const QString& password, const QString& token);
	void loginFailed(const QString& error);

private:
	virtual bool processPostResponse(int status, const QByteArray& data,
		const HeaderList& headers);

	Input m_data;
};

}

#endif // AUTHENTICATION_SERVICE_H
