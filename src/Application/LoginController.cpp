#include "LoginController.h"

#include "AppController.h"
#include "LoginUI/LoginWidget.h"
#include "SettingsUI/SettingsWidget.h"
#include "QsLog/QsLog.h"
#include "Settings/settings.h"

#include "APIClient/ApiTypes.h"
#include "APIClient/AuthenticationService.h"
#include "APIClient/DashboardService.h"
#include "APIClient/ProfileService.h"
#include "Network/SimpleDownloader.h"

#include <QtCore/QCoreApplication>
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
			Drive::Settings::instance().get(Drive::Settings::email);

	const auto result = username != previousUsername && !dirContent.isEmpty();

	QLOG_DEBUG()
			<< "needSyncDirClear [" << dirPath
			<< ", " << previousUsername.toString()
			<< ", " << username.toString()
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

LoginController& LoginController::instance()
{
	static LoginController myself;
	return myself;
}

LoginController::LoginController(QObject *parent)
	: QObject(parent)
{
	connect(this, SIGNAL(loginFinished()), this, SLOT(onLoginFinished()));
}

LoginController::~LoginController()
{
	if (loginWidget)
		delete loginWidget;
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
		Settings::instance().set(Settings::forceRelogin, false, Settings::RealSetting);
		showLoginForm();
	}
}

void LoginController::showLoginForm()
{
	if (!loginWidget)
	{
		loginWidget = new LoginWidget();

		connect(loginWidget, &LoginWidget::loginRequest,
			this, &LoginController::login);

		connect(loginWidget, &LoginWidget::passwordResetRequest,
			this, &LoginController::passwordReset);


		RegisterLinkResourceRef regLink = RegisterLinkResource::create();
		connect(regLink.data(), SIGNAL(linkReceived(QString)),
			loginWidget, SLOT(setRegisterLink(QString)));

		regLink->requestRegisterLink();
	}

	loginWidget->show();
}

void LoginController::login(const QString& username, const QString& password)
{
	QLOG_INFO() << "LoginController::login(" << login << ")";

	AppController::instance().setState(Authorizing);

	if (loginWidget)
	{
		loginWidget->enableControls(false);
		QCoreApplication::processEvents();
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
}

void LoginController::passwordReset(const QString& email)
{
	QLOG_INFO() << "LoginController::passwordReset()";

	if (loginWidget)
	{
		loginWidget->enableControls(false);
		QCoreApplication::processEvents();
	}

	PasswordResetResourceRef passwordResetResource =
		PasswordResetResource::create();

	connect(passwordResetResource.data(), SIGNAL(resetSuccessfully()),
		this,  SLOT(onPasswordResetSucceeded()));

	connect(passwordResetResource.data(), SIGNAL(resetFailed(QString)),
		this,  SLOT(onPasswordResetFailed(QString)));

	passwordResetResource->resetPassword(email);
}

void LoginController::closeAll()
{
	if (loginWidget)
	{
		loginWidget->close();
		delete loginWidget;
		loginWidget = 0;
	}
}

void LoginController::requestUserData()
{
	QLOG_INFO() << "LoginController::requestUserData()";

	ProfileRestResourceRef userResource = ProfileRestResource::create();

	connect(userResource.data(), SIGNAL(profileDataReceived(QJsonObject)),
		this,  SLOT(onProfileDataReceived(QJsonObject)));

	connect(userResource.data(), SIGNAL(profileDataError()),
		this,  SLOT(onProfileDataError()));

	userResource->requestProfileData();
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

	AppController::instance().setAuthToken(token);
	requestUserData();
}

void LoginController::onLoginFailed(const QString& error)
{
	showLoginForm();

	loginWidget->enableControls();
	loginWidget->focusOnEmail();
	loginWidget->setError(error);

	AppController::instance().setState(NotAuthorized);
}

void LoginController::onPasswordResetSucceeded()
{
	if (loginWidget)
	{
		loginWidget->enableControls(true);
	}

	QWidget *w = loginWidget ? loginWidget : 0;

	QMessageBox::information(w, tr("Please check your email"),
		tr("We've sent you an email with further instructions about password reset."));
}

void LoginController::onPasswordResetFailed(const QString& error)
{
	if (loginWidget)
	{
		loginWidget->enableControls(true);
	}

	QWidget *w = loginWidget ? loginWidget : 0;
	QMessageBox::warning(w, tr("Password reset failed"), error);
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

		connect(d, SIGNAL(finished(QPixmap)),
			this, SLOT(onAvatarDownloaded(QPixmap)));
	}
	else
	{
		QLOG_INFO() << "No avatar set. URL is: " << profileData.avatarUrl;
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
	QCoreApplication::processEvents();
}

}
