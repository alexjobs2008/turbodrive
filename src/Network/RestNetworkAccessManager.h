#ifndef REST_NETWORK_ACCESS_MANAGER_H
#define REST_NETWORK_ACCESS_MANAGER_H

#include <QtNetwork/QNetworkAccessManager>

class RestNetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT
public:
	explicit RestNetworkAccessManager(QObject *parent = 0);
	virtual ~RestNetworkAccessManager();

protected:
	virtual QNetworkReply *createRequest(Operation op,
		const QNetworkRequest &request, QIODevice *outgoingData = 0);

public:
	void abortAllRequests();

private slots:
	void onFinished(QNetworkReply*);

private:
	struct Data;
	Data* data;
};

#endif // REST_NETWORK_ACCESS_MANAGER_H