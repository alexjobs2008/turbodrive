#ifndef LOGIN_CONTROLLER_H
#define LOGIN_CONTROLLER_H

#include <QtCore/QSharedPointer>
#include <QtCore/QVariantMap>

namespace Drive
{

class LoginWidget;

class LoginController : public QObject
{
    Q_OBJECT
public:    
    static LoginController& instance();

    ~LoginController();

public slots:
    void showLoginForm();
    void login();
    void passwordReset(const QString& email);
    void closeAll();
    void requestUserData();

signals:
    void loginFinished();

private slots:
    void onLoginSucceeded(const QString& token);
	void onLoginFailed(const QString& error);
    void onPasswordResetSucceeded();
    void onPasswordResetFailed(const QString& error);
    void onProfileDataReceived(const QJsonObject& data);
    void onProfileDataError();
    void onAvatarDownloaded(const QPixmap& pixmap);
    void onLoginFinished();

private:
    Q_DISABLE_COPY(LoginController)
    explicit LoginController(QObject *parent = 0);

    static LoginWidget *loginWidget;
};

}

#endif 