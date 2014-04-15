#include "LoginWidget.h"
#include "Settings/settings.h"
#include "QsLog/QsLog.h"
#include "Util/AppStrings.h"
#include "UtilUI/StylesheetReader.h"
#include "UtilUI/AuxWidgets.h"
#include "UtilUI/ExtraLabels.h"
#include "UtilUI/SpinnerWidget.h"
#include "Application/AppController.h"
#include "rtCSS/RealtimeStyleSheetLoader.h"

#include <QtCore/QTimerEvent>
#include <QtCore/QList>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QDir>

#include <QtGui/QIcon>
#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopServices>
#include <QtGui/QValidator>

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>

inline void initResources() { Q_INIT_RESOURCE(loginUI); }

namespace Drive
{

LoginWidget::LoginWidget(QWidget *parent)
	: QFrame(parent)
	, registerLink(QString())
{
	initResources();

	setAttribute(Qt::WA_DeleteOnClose, false);
	setAttribute(Qt::WA_QuitOnClose, false);

	setWindowTitle(QString(tr("%1 Sign In"))
		.arg(Strings::getAppString(Strings::AppName)));

	QIcon icon;
	icon.addPixmap(QPixmap(":/appicon/16.png"));
	icon.addPixmap(QPixmap(":/appicon/24.png"));
	icon.addPixmap(QPixmap(":/appicon/32.png"));
	icon.addPixmap(QPixmap(":/appicon/48.png"));
	icon.addPixmap(QPixmap(":/appicon/256.png"));

	QList<QSize> list = icon.availableSizes();
	QListIterator<QSize> i(list);

	setWindowIcon(icon);

	setWindowFlags(Qt::CustomizeWindowHint
		| Qt::WindowTitleHint
		| Qt::WindowCloseButtonHint);

	initControls();
}

LoginWidget::~LoginWidget()
{
}

bool LoginWidget::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == m_username
		|| watched == m_password
		|| watched == m_autoLogin)
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
			if (keyEvent->key() == Qt::Key_Return
				|| keyEvent->key() == Qt::Key_Enter)
			{
				m_login->click();
				return true;
			}
		}
	}

	return false;
}

void LoginWidget::closeEvent(QCloseEvent *event)
{
	this->showMinimized();
	event->ignore();
}

void LoginWidget::on_signIn_clicked(bool checked)
{
	Q_UNUSED(checked)

	QString usernameCandidate = m_username->lineEdit()->text().trimmed();
	int pos = usernameCandidate.size();

	if (m_username->lineEdit()->validator()->validate(usernameCandidate, pos)
		!= QValidator::Acceptable)
	{
		setError(tr("Please provide a valid phone number"));
		focusOnEmail();
		return;
	}

	if (m_password->lineEdit()->text().trimmed().isEmpty())
	{
		setError(tr("Please provide a non-empty password"));
		m_password->lineEdit()->setText(QString());
		m_password->lineEdit()->setFocus(Qt::MouseFocusReason);
		return;
	}

	Settings::instance().set(Settings::autoLogin,
		m_autoLogin->isChecked(), Settings::RealSetting);

	QLOG_TRACE() << "Login from UI requested";

	enableControls(false);

	emit loginRequest(m_username->text().trimmed(), m_password->text().trimmed());
}

void LoginWidget::enableControls(bool enable)
{
	m_spinner->setOn(!enable);
	m_username->setEnabled(enable);
	m_password->setEnabled(enable);
	m_autoLogin->setEnabled(enable);
	m_register->setEnabled(!registerLink.isEmpty() && enable);
}

void LoginWidget::focusOnEmail()
{
	m_username->lineEdit()->selectAll();
	m_username->lineEdit()->setFocus(Qt::MouseFocusReason);
}

void LoginWidget::setError(const QString& text)
{
	m_spinner->setSubstText(text);
}

void LoginWidget::setRegisterLink(const QString& link)
{
	registerLink = link;
	m_register->setEnabled(!registerLink.isEmpty() && m_username->isEnabled());
}

void LoginWidget::on_signUp_linkActivated(const QString &link)
{
	Q_UNUSED(link)
	QDesktopServices::openUrl(QUrl(registerLink));
}

void LoginWidget::on_forgot_linkActivated(const QString &link)
{
	emit passwordResetRequest(m_username->text().trimmed());
}

void LoginWidget::initControls()
{
	QLabel* logoLabel = new QLabel(this);
	logoLabel->setObjectName("logo");
	logoLabel->setPixmap(QPixmap(":/logo.png"));
	logoLabel->setScaledContents(true);

	m_spinner = new CommonUI::SpinnerWidget(tr("Please wait..."),
		":/spinner/24-", 80, this);

	QLabel* loginLabel = new QLabel(this);
	loginLabel->setObjectName("login");
	loginLabel->setText(tr("Login"));

	m_register = new CommonUI::LinkLabel(tr("Create account"), "signUp", this);
	m_register->setObjectName("signUp");
	m_register->setEnabled(false);

	QHBoxLayout* labelsLayout = new QHBoxLayout();
	labelsLayout->setContentsMargins(0, 0, 0, 0);
	labelsLayout->addWidget(loginLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
	labelsLayout->addWidget(m_register, 0, Qt::AlignRight | Qt::AlignVCenter);

	QFrame* labels = new QFrame(this);
	labels->setObjectName("labels");
	labels->setLayout(labelsLayout);

	m_username = new CommonUI::LabeledEdit(
		QString::null
		, CommonUI::LabeledEdit::Text
		, QString()
		, 0
		, "\\+375\\d+"
		, 100
		, this);
	m_username->setName("username");
	m_username->installEventFilter(this);
	m_username->setText(Settings::instance().get(Settings::email).toString());
	m_username->lineEdit()->setPlaceholderText(tr("Phone"));

	m_password = new CommonUI::LabeledEdit(
		QString::null
		, CommonUI::LabeledEdit::Text
		, QString()
		, 0
		, QString()
		, 0
		, this);
	m_password->setName("password");
	m_password->lineEdit()->setEchoMode(QLineEdit::Password);
	m_password->installEventFilter(this);
	m_password->setText(Settings::instance().get(Settings::password).toString());
	m_password->lineEdit()->setPlaceholderText(tr("Password"));

	m_autoLogin = new QCheckBox(
		QString(tr("&Sign me in when %1 starts"))
		.arg(Strings::getAppString(Strings::AppName))
		, this);
	m_autoLogin->setObjectName("rememberPassword");
	m_autoLogin->installEventFilter(this);
	m_autoLogin->setChecked(Settings::instance().get(Settings::autoLogin).toBool());

	m_login = new QPushButton(tr("Sign in"), this);
	m_login->setObjectName("signIn");
	m_login->setAutoDefault(true);
	m_login->setMouseTracking(true);

	m_resetPassword = new CommonUI::LinkLabel(tr("Forgot?"), "forgot", this);
	m_resetPassword->setObjectName("forgot");

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(logoLabel, 0, Qt::AlignCenter);
	layout->addWidget(m_spinner);
	layout->addWidget(labels, 0, Qt::AlignCenter);
	layout->addWidget(m_username, 0, Qt::AlignCenter);
	layout->addWidget(m_password, 0, Qt::AlignCenter);
	layout->addWidget(m_autoLogin, 0, Qt::AlignCenter);
	layout->addWidget(m_login, 0, Qt::AlignCenter);
	layout->addStretch(1);
	layout->addWidget(m_resetPassword, 0, Qt::AlignCenter);

	QMetaObject::connectSlotsByName(this);

	CommonUI::StyleSheetReader::setStyleSheetFor(this);

	RealtimeStyleSheetLoader *rsl = new RealtimeStyleSheetLoader(this);
	rsl->addWidget(this);
}

void LoginWidget::setFolder()
{
	QString folderPath =
		Settings::instance().get(Settings::folderPath).toString();

	QDir dir;
	dir.mkpath(folderPath);
}

}
