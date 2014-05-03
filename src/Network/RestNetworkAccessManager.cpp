#include "RestNetworkAccessManager.h"
#include <QtNetwork/QNetworkReply>
#include <QtCore/QList>
#include <QtCore/QPointer>

struct RestNetworkAccessManager::Data
{
	QList<QPointer<QNetworkReply> > replies;
};

RestNetworkAccessManager::RestNetworkAccessManager(QObject *parent)
	: QNetworkAccessManager(parent)
	, data(new Data())
{
	connect(this, &RestNetworkAccessManager::finished,
			this, &RestNetworkAccessManager::onFinished);
}

RestNetworkAccessManager::~RestNetworkAccessManager()
{
	delete data;
}

QNetworkReply* RestNetworkAccessManager::createRequest(Operation op,
	const QNetworkRequest &request, QIODevice *outgoingData)
{
	QNetworkReply* reply =
		QNetworkAccessManager::createRequest(op, request, outgoingData);

	data->replies.append(reply);

	return reply;
}

void RestNetworkAccessManager::onFinished(QNetworkReply* reply)
{
	data->replies.removeOne(reply);
}

void RestNetworkAccessManager::abortAllRequests()
{
	while(data->replies.size())
	{
		QPointer<QNetworkReply> reply = data->replies.takeFirst();
		if(!(reply.isNull() || reply->isFinished()))
			reply->abort();
	}
}
