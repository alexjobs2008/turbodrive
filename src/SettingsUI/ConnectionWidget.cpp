#include "ConnectionWidget.h"
#include "Settings/settings.h"
#include "Settings/proxySettings.h"
#include "QsLog/QsLog.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>

#define MIN_CONNECTION_TAB_LABEL_WIDTH 96

namespace Drive
{

ConnectionWidget::ConnectionWidget(QWidget *parent)
	: QFrame(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	mainLayout->addLayout(createBandwidthControls());
	mainLayout->addWidget(createProxyControls());
	mainLayout->addStretch(1);

	setFromSettings();

	QMetaObject::connectSlotsByName(this);
}

QBoxLayout* ConnectionWidget::createBandwidthControls()
{
	// Download rate

	QGroupBox *gbDownload = new QGroupBox(tr("Download rate"), this);
	QVBoxLayout *blDownload = new QVBoxLayout(gbDownload);

	rbDownloadNoLimit = new QRadioButton(tr("Don't limit"), gbDownload);
	rbDownloadNoLimit->setObjectName("downloadNoLimit");

	QHBoxLayout *blDownloadLimit = new QHBoxLayout();
	blDownloadLimit->setContentsMargins(0, 0, 0, 0);

	rbDownloadLimit = new QRadioButton(tr("Limit to:"), gbDownload);

	leDownloadLimit = new QLineEdit(gbDownload);
	leDownloadLimit->setMaximumWidth(48);
	leDownloadLimit->setAlignment(Qt::AlignRight);

	lDownloadLimit = new QLabel(tr("kB/s"), gbDownload);

	leDownloadLimit->setDisabled(true);
	lDownloadLimit->setDisabled(true);

	blDownloadLimit->addWidget(rbDownloadLimit);
	blDownloadLimit->addWidget(leDownloadLimit);
	blDownloadLimit->addWidget(lDownloadLimit);
	blDownloadLimit->addStretch(1);

	blDownload->addWidget(rbDownloadNoLimit);
	blDownload->addLayout(blDownloadLimit);

	// Upload rate

	QGroupBox *gbUpload = new QGroupBox(tr("Upload rate"), this);
	QVBoxLayout *blUpload = new QVBoxLayout(gbUpload);

	rbUploadNoLimit = new QRadioButton(tr("Don't limit"), gbUpload);
	rbUploadNoLimit->setObjectName("uploadNoLimit");

	QHBoxLayout *blUploadLimit = new QHBoxLayout();

	rbUploadLimit = new QRadioButton(tr("Limit to:"), gbUpload);

	leUploadLimit = new QLineEdit(gbUpload);
	leUploadLimit->setMaximumWidth(48);
	leUploadLimit->setAlignment(Qt::AlignRight);

	lUploadLimit = new QLabel(tr("kB/s"), gbUpload);

	leUploadLimit->setDisabled(true);
	lUploadLimit->setDisabled(true);

	blUploadLimit->addWidget(rbUploadLimit);
	blUploadLimit->addWidget(leUploadLimit);
	blUploadLimit->addWidget(lUploadLimit);
	blUploadLimit->addStretch(1);

	blUpload->addWidget(rbUploadNoLimit);
	blUpload->addLayout(blUploadLimit);

	QHBoxLayout *blBandwidth = new QHBoxLayout();
	blBandwidth->addWidget(gbDownload);
	blBandwidth->addSpacing(4);
	blBandwidth->addWidget(gbUpload);

	return blBandwidth;
}

QWidget* ConnectionWidget::createProxyControls()
{
	QGroupBox *gbProxy = new QGroupBox(tr("Proxy"), this);
	QVBoxLayout *blProxy = new QVBoxLayout(gbProxy);

	// Proxy options
	QVBoxLayout *blProxyOptions = new QVBoxLayout();
	rbNoProxy = new QRadioButton(tr("No proxy"), gbProxy);
	rbNoProxy->setObjectName("noProxy");

	rbAutoProxy = new QRadioButton(tr("Auto-detect"), gbProxy);
	rbAutoProxy->setObjectName("autoProxy");

	QHBoxLayout *blManualProxy = new QHBoxLayout();
	blManualProxy->setContentsMargins(0, 0, 0, 0);
	rbManualProxy = new QRadioButton(tr("Manual settings:"), gbProxy);
	rbManualProxy->setObjectName("manualProxy");
	pbProxyConfigure = new QPushButton(tr("Configure..."), gbProxy);
	pbProxyConfigure->setObjectName("proxyConfigure");
	blManualProxy->addWidget(rbManualProxy);
	blManualProxy->addWidget(pbProxyConfigure);
	blManualProxy->addStretch(1);

	blProxyOptions->setSpacing(3);
	blProxyOptions->addWidget(rbNoProxy);
	blProxyOptions->addSpacing(4);
	blProxyOptions->addWidget(rbAutoProxy);
	blProxyOptions->addSpacing(1);
	blProxyOptions->addLayout(blManualProxy);
	blProxyOptions->addStretch(1);

	blProxy->addLayout(blProxyOptions);
	blProxy->addStretch(1);

	return gbProxy;
}

void ConnectionWidget::setFromSettings()
{
	Settings &settings = Settings::instance();

	if (settings.get(Settings::limitDownload).toBool())
		rbDownloadLimit->setChecked(true);
	else
		rbDownloadNoLimit->setChecked(true);

	leDownloadLimit->setDisabled(!rbDownloadLimit->isChecked());
	lDownloadLimit->setDisabled(!rbDownloadLimit->isChecked());

	if (settings.get(Settings::limitUpload).toBool())
		rbUploadLimit->setChecked(true);
	else
		rbUploadNoLimit->setChecked(true);

	leUploadLimit->setDisabled(!rbUploadLimit->isChecked());
	lUploadLimit->setDisabled(!rbUploadLimit->isChecked());

	leDownloadLimit->setText(settings.get(Settings::downloadSpeed).toString());
	leUploadLimit->setText(settings.get(Settings::uploadSpeed).toString());

	ProxyUsage proxy = (ProxyUsage)settings.get(Settings::proxyUsage).toInt();

	switch (proxy)
	{
	case NoProxy:
		rbNoProxy->setChecked(true);
		break;
	case AutodetectProxy:
		rbAutoProxy->setChecked(true);
		break;
	case CustomProxy:
		rbManualProxy->setChecked(true);
		break;
	default:
		QLOG_DEBUG() << "Bad proxy usage setting";
		break;
	}

	pbProxyConfigure->setEnabled(rbManualProxy->isChecked());

}

void ConnectionWidget::on_downloadNoLimit_toggled(bool checked)
{
	Settings::instance().set(Settings::limitDownload, !checked);

	leDownloadLimit->setDisabled(checked);
	lDownloadLimit->setDisabled(checked);
}

void ConnectionWidget::on_uploadNoLimit_toggled(bool checked)
{
	Settings::instance().set(Settings::limitUpload, !checked);

	leUploadLimit->setDisabled(checked);
	lUploadLimit->setDisabled(checked);
}

void ConnectionWidget::on_noProxy_toggled(bool checked)
{
	if (checked)
		Settings::instance().set(Settings::proxyUsage, ProxyUsage::NoProxy);
}

void ConnectionWidget::on_autoProxy_toggled(bool checked)
{
	if (checked)
		Settings::instance()
			.set(Settings::proxyUsage, ProxyUsage::AutodetectProxy);
}

void ConnectionWidget::on_manualProxy_toggled(bool checked)
{
	pbProxyConfigure->setEnabled(checked);

	if (checked)
		Settings::instance()
			.set(Settings::proxyUsage, ProxyUsage::CustomProxy);
}

void ConnectionWidget::on_proxyConfigure_clicked(bool)
{
	static ProxySettingsDialog *d = new ProxySettingsDialog(this);

	if (d->exec() == QDialog::Accepted)
	{
		Settings::instance().set(Settings::proxyCustomSettings,
			QVariant::fromValue(d->proxySettings()));
	}
}

ProxySettingsDialog::ProxySettingsDialog(QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setWindowTitle(tr("Proxy Settings"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowIcon(QIcon(":/appicon.ico"));
	setAttribute(Qt::WA_QuitOnClose, false);


	// Proxy type
	QHBoxLayout *blProxyType = new QHBoxLayout();
	QLabel *lProxyType = new QLabel(tr("Proxy type:"), this);
	lProxyType->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH);
	lProxyType->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	rbProxyHttp = new QRadioButton("HTTP", this);
	rbProxyHttp->setChecked(true);
	rbProxySocks = new QRadioButton("SOCKS", this);

	blProxyType->addWidget(lProxyType);
	blProxyType->addWidget(rbProxyHttp);
	blProxyType->addSpacing(8);
	blProxyType->addWidget(rbProxySocks);
	blProxyType->addStretch(1);

	// Server and port
	QHBoxLayout *blProxyServer = new QHBoxLayout();
	QLabel *lProxyServer = new QLabel("Server:", this);
	lProxyServer->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH);
	lProxyServer->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	leProxyServer = new QLineEdit(this);
	QLabel *lColon = new QLabel(":", this);
	leProxyPort = new QLineEdit(this);
	leProxyPort->setMaximumWidth(48);
	leProxyPort->setAlignment(Qt::AlignRight);

	blProxyServer->addWidget(lProxyServer);
	blProxyServer->addWidget(leProxyServer);
	blProxyServer->addWidget(lColon);
	blProxyServer->addWidget(leProxyPort);
	blProxyServer->addStretch(1);

	// Authorization required
	QHBoxLayout *blProxyAuthRequired = new QHBoxLayout();
	QLabel *lAligner = new QLabel(this);
	lAligner->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH);
	lAligner->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	cProxyAuthRequired =
		new QCheckBox(tr("Proxy server requires a password"), this);
	cProxyAuthRequired->setObjectName("cProxyAuthRequired");
	blProxyAuthRequired->addWidget(lAligner);
	blProxyAuthRequired->addWidget(cProxyAuthRequired);
	blProxyAuthRequired->addStretch(1);

	// Username
	userNameWidget = new QWidget(this);
	userNameWidget->setDisabled(true);
	QHBoxLayout *blProxyUsername = new QHBoxLayout(userNameWidget);
	blProxyUsername->setContentsMargins(0, 0, 0, 0);
	QLabel *lProxyUsername = new QLabel(tr("Username:"), this);
	lProxyUsername->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH);
	lProxyUsername->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	leProxyUsername = new QLineEdit(this);
	blProxyUsername->addWidget(lProxyUsername);
	blProxyUsername->addWidget(leProxyUsername);
	blProxyUsername->addStretch(1);

	// Password
	passwordWidget = new QWidget(this);
	passwordWidget->setDisabled(true);
	QHBoxLayout *blProxyPassword = new QHBoxLayout(passwordWidget);
	blProxyPassword->setContentsMargins(0, 0, 0, 0);
	QLabel *lProxyPassword = new QLabel(tr("Password:"), this);
	lProxyPassword->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH);
	lProxyPassword->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	leProxyPassword = new QLineEdit(this);
	leProxyPassword->setEchoMode(QLineEdit::Password);
	blProxyPassword->addWidget(lProxyPassword);
	blProxyPassword->addWidget(leProxyPassword);
	blProxyPassword->addStretch(1);

	// Layout
	QVBoxLayout *blManualProxySettings = new QVBoxLayout();
	blManualProxySettings->addLayout(blProxyType);
	blManualProxySettings->addLayout(blProxyServer);
	blManualProxySettings->addLayout(blProxyAuthRequired);
	blManualProxySettings->addWidget(userNameWidget);
	blManualProxySettings->addWidget(passwordWidget);
	blManualProxySettings->addStretch(1);

	// bottom button box
	QDialogButtonBox *buttonBox = new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
		Qt::Horizontal, this);

	QFrame *separator = new QFrame(this);
	separator->setStyleSheet("QFrame {"
		"border-image: url(\":/hline.png\") 1 0 1 0;"
		"border-top: 1px transparent;"
		"border-bottom: 1px transparent;"
		"border-left: 0px transparent;"
		"border-right: 0px transparent;"
		"margin: 4px 0px 4px 0px;"
		"min-height: 1px;"
		"max-height: 1px;"
		"};");

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addLayout(blManualProxySettings, 1);
	layout->addWidget(separator);
	layout->addWidget(buttonBox);

	connect(buttonBox, &QDialogButtonBox::accepted,
			this, &ProxySettingsDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected,
			this, &ProxySettingsDialog::reject);

	QMetaObject::connectSlotsByName(this);
}

int ProxySettingsDialog::exec()
{
	ProxySettings settings = Settings::instance().get(Settings::proxyCustomSettings).value<ProxySettings>();

	if (settings.kind == ProxySettings::HttpServer)
		rbProxyHttp->setChecked(true);
	else
		rbProxySocks->setChecked(true);

	leProxyServer->setText(settings.server);
	leProxyPort->setText(settings.port);
	cProxyAuthRequired->setChecked(settings.loginRequired);
	leProxyUsername->setText(settings.username);
	leProxyPassword->setText(settings.password);

	return QDialog::exec();
}

ProxySettings ProxySettingsDialog::proxySettings() const
{
	ProxySettings settings;

	if (rbProxyHttp->isChecked())
		settings.kind = ProxySettings::HttpServer;
	else
		settings.kind = ProxySettings::SocksServer;

	settings.server = leProxyServer->text();
	settings.port = leProxyPort->text();
	settings.loginRequired = cProxyAuthRequired->isChecked();
	settings.username = leProxyUsername->text();
	settings.password = leProxyPassword->text();
	return settings;
}

void ProxySettingsDialog::on_cProxyAuthRequired_toggled(bool checked)
{
	userNameWidget->setEnabled(checked);
	passwordWidget->setEnabled(checked);
}

}
