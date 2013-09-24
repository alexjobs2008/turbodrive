#include "ConnectionWidget.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>

#define MIN_CONNECTION_TAB_LABEL_WIDTH 96

ConnectionWidget::ConnectionWidget(QWidget *parent)
    : QFrame(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addLayout(createBandwidthControls());
    mainLayout->addWidget(createProxyControls());
    mainLayout->addStretch(1);

    QMetaObject::connectSlotsByName(this);
    
    // TODO: replace with settings value
    rbDownloadNoLimit->setChecked(true);
    rbUploadNoLimit->setChecked(true);

    rbNoProxy->setChecked(true);
}

QBoxLayout* ConnectionWidget::createBandwidthControls()
{
    // Download rate

    QGroupBox *gbDownload = new QGroupBox(tr("Download rate"), this);
    QVBoxLayout *blDownload = new QVBoxLayout(gbDownload);

    rbDownloadNoLimit = new QRadioButton(tr("Don't limit"), gbDownload);
    rbDownloadNoLimit->setObjectName("rbDownloadNoLimit");    

    QHBoxLayout *blDownloadLimit = new QHBoxLayout();

    QRadioButton *rbDownloadLimit =
        new QRadioButton(tr("Limit to:"), gbDownload);

    leDownloadLimit = new QLineEdit("50", gbDownload);
    leDownloadLimit->setMaximumWidth(48);
    leDownloadLimit->setAlignment(Qt::AlignRight);

    lDownloadLimit = new QLabel(tr("kB/s"), gbDownload);

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
    rbUploadNoLimit->setObjectName("rbUploadNoLimit");    

    QHBoxLayout *blUploadLimit = new QHBoxLayout();

    QRadioButton *rbUploadLimit =
        new QRadioButton(tr("Limit to:"), gbUpload);

    leUploadLimit = new QLineEdit("50", gbUpload);
    leUploadLimit->setMaximumWidth(48);
    leUploadLimit->setAlignment(Qt::AlignRight);

    lUploadLimit = new QLabel(tr("kB/s"), gbUpload);

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
    rbNoProxy->setObjectName("rbNoProxy");

    rbAutoProxy = new QRadioButton(tr("Auto-detect"), gbProxy);
    rbAutoProxy->setObjectName("rbAutoProxy");

    rbManualProxy = new QRadioButton(tr("Manual:"), gbProxy);
    rbManualProxy->setObjectName("rbManualProxy");

    blProxyOptions->addWidget(rbNoProxy);
    blProxyOptions->addWidget(rbAutoProxy);
    blProxyOptions->addWidget(rbManualProxy);
    blProxyOptions->addStretch(1);    

    // Proxy type
    QHBoxLayout *blProxyType = new QHBoxLayout();
    QLabel *lProxyType = new QLabel(tr("Proxy type:"), gbProxy);    
    lProxyType->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH);
    lProxyType->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    cbProxyType = new QComboBox(gbProxy);
    cbProxyType->addItem("HTTP", 3); // QNetworkProxy::HttpProxy
    cbProxyType->addItem("SOCKS", 1); // QNetworkProxy::Socks5Proxy
    blProxyType->addWidget(lProxyType);
    blProxyType->addWidget(cbProxyType);
    blProxyType->addStretch(1);

    // Server and port
    QHBoxLayout *blProxyServer = new QHBoxLayout();
    QLabel *lProxyServer = new QLabel("Server:", gbProxy);
    lProxyServer->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH);
    lProxyServer->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    leProxyServer = new QLineEdit(gbProxy);
    QLabel *lColon = new QLabel(":", gbProxy);
    leProxyPort = new QLineEdit(gbProxy);
    leProxyPort->setMaximumWidth(32);
    leProxyPort->setAlignment(Qt::AlignRight);

    blProxyServer->addWidget(lProxyServer);
    blProxyServer->addWidget(leProxyServer);
    blProxyServer->addWidget(lColon);
    blProxyServer->addWidget(leProxyPort);
    blProxyServer->addStretch(1);

    // Authorization required
    QHBoxLayout *blProxyAuthRequired = new QHBoxLayout();
    QLabel *lAligner = new QLabel(gbProxy);
    lAligner->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH);
    lAligner->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    cProxyAuthRequired =
        new QCheckBox(tr("Proxy server requires a password"), gbProxy);
    cProxyAuthRequired->setObjectName("cProxyAuthRequired");
    blProxyAuthRequired->addWidget(lAligner);
    blProxyAuthRequired->addWidget(cProxyAuthRequired);
    blProxyAuthRequired->addStretch(1);

    // Username
    userNameWidget = new QWidget(gbProxy);
    QHBoxLayout *blProxyUsername = new QHBoxLayout(userNameWidget);
    blProxyUsername->setContentsMargins(0, 0, 0, 0);
    QLabel *lProxyUsername = new QLabel(tr("Username:"), gbProxy);
    lProxyUsername->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH);
    lProxyUsername->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    leProxyUsername = new QLineEdit(gbProxy);
    blProxyUsername->addWidget(lProxyUsername);
    blProxyUsername->addWidget(leProxyUsername);
    blProxyUsername->addStretch(1);

    // Password
    passwordWidget = new QWidget(gbProxy);
    QHBoxLayout *blProxyPassword = new QHBoxLayout(passwordWidget);
    blProxyPassword->setContentsMargins(0, 0, 0, 0);
    QLabel *lProxyPassword = new QLabel(tr("Password:"), gbProxy);
    lProxyPassword->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH);
    lProxyPassword->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    leProxyPassword = new QLineEdit(gbProxy);
    blProxyPassword->addWidget(lProxyPassword);
    blProxyPassword->addWidget(leProxyPassword);
    blProxyPassword->addStretch(1);    

    // Manual proxy layout
    manualProxySettingsWidget = new QWidget(gbProxy);
    QVBoxLayout *blManualProxySettings = new QVBoxLayout(manualProxySettingsWidget);
    blManualProxySettings->setContentsMargins(0, 0, 0, 0);
    blManualProxySettings->addLayout(blProxyType);
    blManualProxySettings->addLayout(blProxyServer);
    blManualProxySettings->addLayout(blProxyAuthRequired);
    blManualProxySettings->addWidget(userNameWidget);
    blManualProxySettings->addWidget(passwordWidget);
    blManualProxySettings->addStretch(1);

    blProxy->addLayout(blProxyOptions);
    blProxy->addWidget(manualProxySettingsWidget);
    blProxy->addStretch(1);
    
    return gbProxy;
}

void ConnectionWidget::on_rbDownloadNoLimit_toggled(bool checked)
{
    leDownloadLimit->setDisabled(checked);
    lDownloadLimit->setDisabled(checked);
}

void ConnectionWidget::on_rbUploadNoLimit_toggled(bool checked)
{
    leUploadLimit->setDisabled(checked);
    lUploadLimit->setDisabled(checked);
}

void ConnectionWidget::on_rbManualProxy_toggled(bool checked)
{
    manualProxySettingsWidget->setEnabled(checked);
}

void ConnectionWidget::on_cProxyAuthRequired_toggled(bool checked)
{
    userNameWidget->setEnabled(checked);
    passwordWidget->setEnabled(checked);
}

