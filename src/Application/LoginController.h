#ifndef LOGIN_CONTROLLER_H
#define LOGIN_CONTROLLER_H

#include <QtCore/QSharedPointer>
#include <QtCore/QVariantMap>
#include <QtWidgets/QMessageBox>
#include "AuxWidgets.h"
#include "LoginWidget.h"
#include "passwordresetwidget.h"

namespace Drive
{

class LoginWidget;

class LoginController : public QObject
{
	Q_OBJECT
public:
	static LoginController& instance();

	~LoginController();

    void setLoggedIn(bool isLoggedIn);
    bool isLoggedIn();

public slots:
	void showLoginFormOrLogin();
	void showLoginForm();
	void login(const QString& username, const QString& password);
    void passwordReset(const QString &username);
	void closeAll();
	void requestUserData();
    void showPasswordResetWidget(const QString& username);
    void closePasswordResetWidget();

signals:
	void loginFinished();

public slots:
    void onPasswordResetFinished();
    void onPasswordResetSucceeded();
    void onPasswordResetFailed(const QString& message);

private slots:
	void onLoginSucceeded(
			const QString& username, const QString& password, const QString& token);

	void onLoginFailed(const QString& error);

	void onProfileDataReceived(const QJsonObject& data);
	void onProfileDataError();
	void onAvatarDownloaded(const QPixmap& pixmap);
	void onLoginFinished();

private:
	Q_DISABLE_COPY(LoginController)
	explicit LoginController(QObject *parent = 0);

    bool loggedIn;

	static LoginWidget *loginWidget;
    static PasswordResetWidget *passwordResetWidget;
    QMessageBox *messageBox;
    QAbstractButton *messageBoxButton;
};

}

#endif
