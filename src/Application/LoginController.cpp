#include "LoginController.h"

#include "AppController.h"
#include "LoginUI/LoginWidget.h"
#include "SettingsUI/SettingsWidget.h"
#include "QsLog/QsLog.h"
#include "Settings/settings.h"
#include "Util/FileUtils.h"

#include "APIClient/ApiTypes.h"
#include "APIClient/AuthenticationService.h"
#include "APIClient/DashboardService.h"
#include "APIClient/ProfileService.h"
#include "Network/SimpleDownloader.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QJsonObject>
#include <QtWidgets/QMessageBox>

#include <QDir>

namespace
{

bool needSyncDirClear(const QString& username)
{
	const auto dirPath = Drive::Settings::instance().get(Drive::Settings::folderPath).toString();
	const auto syncDir = QDir(dirPath);
	const auto dirContent = syncDir.entryList(QDir::AllEntries
										| QDir::System
										| QDir::NoDotAndDotDot);

	const auto previousUsername =
			Drive::Settings::instance().get(Drive::Settings::email).toString();

	const auto result = username != previousUsername && !dirContent.isEmpty();

	QLOG_DEBUG()
			<< "needSyncDirClear [" << dirPath
			<< ", " << previousUsername
			<< ", " << username
			<< "]: " << result;
	return result;
}

bool syncDirClearingConfirmed()
{
	const auto dirPath = Drive::Settings::instance().get(Drive::Settings::folderPath).toString();
	const auto result = true;

	QLOG_DEBUG() << "syncDirClearingConfirmed [" << dirPath << "]: " << result;
	return result;
}

bool clearSyncDir()
{
	const auto dirPath = Drive::Settings::instance().get(Drive::Settings::folderPath).toString();
	auto syncDir = QDir(dirPath);

	const auto result = syncDir.removeRecursively();
	Drive::AppController::instance().createFolder();

	if (result)
	{
		QLOG_DEBUG() << "clearSyncDir [" << dirPath << "]: " << result;
	}
	else
	{
		QLOG_ERROR() << "clearSyncDir [" << dirPath << "] FAILED";
	}
	return true;
}

}

namespace Drive
{

LoginWidget* LoginController::loginWidget = 0;
PasswordResetWidget* LoginController::passwordResetWidget(0);

LoginController& LoginController::instance()
{
	static LoginController myself;
	return myself;
}

LoginController::LoginController(QObject *parent)
    : QObject(parent),
      loggedIn(false),
      messageBox(new QMessageBox(loginWidget))
{
    messageBox->setMinimumWidth(350);
    messageBox->setWindowTitle(trUtf8("Восстановление пароля"));
    messageBoxButton = messageBox->addButton(trUtf8("Готово"), QMessageBox::YesRole);

    connect(this, &LoginController::loginFinished,
			this, &LoginController::onLoginFinished);
}

LoginController::~LoginController()
{
    closeAll();
}

void LoginController::setLoggedIn(bool isLoggedIn)
{
    loggedIn = isLoggedIn;
}

bool LoginController::isLoggedIn()
{
    return loggedIn;
}

void LoginController::showLoginFormOrLogin()
{
	const auto username = Settings::instance().get(Settings::email).toString();
	const auto password = Settings::instance().get(Settings::password).toString();
	const auto autoLogin = Settings::instance().get(Settings::autoLogin).toBool();
	const auto forceRelogin = Settings::instance().get(Settings::forceRelogin).toBool();
	if (!forceRelogin && autoLogin && !username.isEmpty() && !password.isEmpty())
	{
		login(username, password);
	}
	else
	{
		showLoginForm();
	}
}

void LoginController::showLoginForm()
{
	if (!loginWidget)
	{
		loginWidget = new LoginWidget();
        messageBox->setWindowIcon(loginWidget->windowIcon());

		connect(loginWidget, &LoginWidget::loginRequest,
			this, &LoginController::login);

		connect(loginWidget, &LoginWidget::passwordResetRequest,
			this, &LoginController::passwordReset);


		RegisterLinkResourceRef regLink = RegisterLinkResource::create();
		connect(regLink.data(), &RegisterLinkResource::linkReceived,
			loginWidget, &LoginWidget::setRegisterLink);

		regLink->requestRegisterLink();
	}

	loginWidget->show();
    showDockIcon(true);
}

void LoginController::login(const QString& username, const QString& password)
{
	QLOG_INFO() << "LoginController::login(" << username << ")";

	AppController::instance().setState(Authorizing);

	if (loginWidget)
	{
		loginWidget->enableControls(false);
        // QCoreApplication::processEvents();
	}

	AuthRestResourceRef authResource = AuthRestResource::create();
	connect(authResource.data(), &AuthRestResource::loginSucceeded,
		this,  &LoginController::onLoginSucceeded);
	connect(authResource.data(), &AuthRestResource::loginFailed,
		this,  &LoginController::onLoginFailed);

	AuthRestResource::Input inputData;
	inputData.username = username;
	inputData.password = password;

	authResource->login(inputData);
    // QCoreApplication::processEvents();
}

void LoginController::passwordReset(const QString& username)
{
	if (loginWidget)
	{
		loginWidget->enableControls(false);
        // QCoreApplication::processEvents();
	}

    this->showPasswordResetWidget(username);

    /* PasswordResetResourceRef passwordResetResource =
		PasswordResetResource::create();

	connect(passwordResetResource.data(), &PasswordResetResource::resetSuccessfully,
			this, &LoginController::onPasswordResetSucceeded);

	connect(passwordResetResource.data(), &PasswordResetResource::resetFailed,
			this, &LoginController::onPasswordResetFailed);

    passwordResetResource->resetPassword(username); */
}

void LoginController::closeAll()
{
	if (loginWidget)
	{
        loginWidget->doClose();
        showDockIcon(false);
        delete loginWidget;
		loginWidget = 0;
	}
}

void LoginController::requestUserData()
{
	QLOG_INFO() << "LoginController::requestUserData()";

	ProfileRestResourceRef userResource = ProfileRestResource::create();

	connect(userResource.data(), &ProfileRestResource::profileDataReceived,
			this, &LoginController::onProfileDataReceived);

	connect(userResource.data(), &ProfileRestResource::profileDataError,
			this, &LoginController::onProfileDataError);

    userResource->requestProfileData();
}

void LoginController::showPasswordResetWidget(const QString& username)
{
    if (passwordResetWidget != 0)
    {
        closePasswordResetWidget();
    }

    // loginWidget->hide();
    passwordResetWidget = new PasswordResetWidget(username);
    passwordResetWidget->setWindowIcon(loginWidget->windowIcon());
    passwordResetWidget->show();
}

void LoginController::closePasswordResetWidget()
{
    if (passwordResetWidget != 0)
    {
        passwordResetWidget->close();
        passwordResetWidget = 0;
    }
}

void LoginController::onLoginSucceeded(
		const QString& username, const QString& password, const QString& token)
{
	if (needSyncDirClear(username))
	{
		if (!syncDirClearingConfirmed())
		{
			onLoginFailed(tr("Login cancelled by user."));
			return;
		}
		else if (!clearSyncDir())
		{
			onLoginFailed(tr("Directory clearing failed."));
			return;
		}
	}

	Settings::instance().set(Settings::email, username, Settings::RealSetting);
	Settings::instance().set(Settings::password, password, Settings::RealSetting);

    loggedIn = true;
	AppController::instance().setAuthToken(token);
	requestUserData();
}

void LoginController::onLoginFailed(const QString& error)
{
    loggedIn = false;
	showLoginForm();

	loginWidget->enableControls();
	loginWidget->focusOnUsername();
	loginWidget->setError(error);

	AppController::instance().setState(NotAuthorized);
}

void LoginController::onPasswordResetFinished()
{
    closePasswordResetWidget();

    if (loginWidget)
	{
        // Return to login widget
        loginWidget->show();
		loginWidget->enableControls(true);
	}
}

void LoginController::onPasswordResetSucceeded()
{
    closePasswordResetWidget();

    if (loginWidget)
    {
        messageBox->setText(trUtf8("Новый пароль был выслан на ваш номер телефона"));
        messageBoxButton->setText(trUtf8("Готово"));
        messageBox->exec();

        // Return to login widget
        loginWidget->show();
        loginWidget->enableControls(true);
    }
}

void LoginController::onPasswordResetFailed(const QString& message)
{
    closePasswordResetWidget();

	if (loginWidget)
	{
        messageBox->setText(message);
        messageBoxButton->setText(trUtf8("Закрыть"));
        messageBox->exec();

        // Return to reset password window
        loginWidget->on_forgot_linkActivated("");

        // loginWidget->show();
        // loginWidget->enableControls(true);
	}
}

void LoginController::onProfileDataReceived(const QJsonObject& data)
{
	ProfileData profileData = ProfileData::fromJson(data);

	//userData.log();

	if (!profileData.isValid())
	{
		onProfileDataError();
		return;
	}

//	QString userName = QString("%1 %2")
//		.arg(map.value("first_name", QVariant()).toString())
//		.arg(map.value("last_name", QVariant()).toString())
//		.trimmed();

	AppController::instance().setProfileData(profileData);
	emit loginFinished();

	QUrl url(profileData.avatarUrl);
	if (url.isValid())
	{
		QString newUrl = url.toString();
		int dotPos = newUrl.lastIndexOf(".");
		if (dotPos == -1)
		{
			QLOG_ERROR() << "Bad avatar image URL: " << profileData.avatarUrl;
			return;
		}

		newUrl = QString("%1%2%3")
			.arg(newUrl.left(dotPos))
			.arg("@avatar_desktop")
			.arg(newUrl.right(newUrl.size() - dotPos));

		SimpleDownloader *d = new SimpleDownloader(QUrl(newUrl),
			SimpleDownloader::Pixmap, this);

		QLOG_TRACE() << "Downloading avatar from: " << newUrl;

		connect(d, &SimpleDownloader::pixmapDownloaded,
				this, &LoginController::onAvatarDownloaded);
	}
}

void LoginController::onProfileDataError()
{
	onLoginFailed(tr("Login failed: can't obtain user details"));
}

void LoginController::onAvatarDownloaded(const QPixmap& pixmap)
{
	ProfileData profileData = AppController::instance().profileData();
	profileData.avatar = pixmap;
	AppController::instance().setProfileData(profileData);
	// TODO: delete downloader
}

void LoginController::onLoginFinished()
{
	QLOG_TRACE() << "Login finished ok, closing the window";
	closeAll();
    // QCoreApplication::processEvents();
}

}
