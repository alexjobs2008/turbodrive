#include "ConnectionWidget.h"

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
    blDownloadLimit->setContentsMargins(0, 0, 0, 0);

    QRadioButton *rbDownloadLimit =
        new QRadioButton(tr("Limit to:"), gbDownload);

    leDownloadLimit = new QLineEdit("50", gbDownload);
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
    rbUploadNoLimit->setObjectName("rbUploadNoLimit");    

    QHBoxLayout *blUploadLimit = new QHBoxLayout();

    QRadioButton *rbUploadLimit =
        new QRadioButton(tr("Limit to:"), gbUpload);

    leUploadLimit = new QLineEdit("50", gbUpload);
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
    rbNoProxy->setObjectName("rbNoProxy");

    rbAutoProxy = new QRadioButton(tr("Auto-detect"), gbProxy);
    rbAutoProxy->setObjectName("rbAutoProxy");

    QHBoxLayout *blManualProxy = new QHBoxLayout();
    blManualProxy->setContentsMargins(0, 0, 0, 0);
    rbManualProxy = new QRadioButton(tr("Manual settings:"), gbProxy);
    rbManualProxy->setObjectName("rbManualProxy");
    pbProxyConfigure = new QPushButton(tr("Configure..."), gbProxy);
    pbProxyConfigure->setObjectName("pbProxyConfigure");
    pbProxyConfigure->setEnabled(false);
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

    //manualProxySettingsWidget = new ProxyWidget(gbProxy);

    blProxy->addLayout(blProxyOptions);
    //blProxy->addWidget(manualProxySettingsWidget);
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
    //manualProxySettingsWidget->setEnabled(checked);
    pbProxyConfigure->setEnabled(checked);
}

void ConnectionWidget::on_pbProxyConfigure_clicked(bool checked)
{
    ProxySettingsDialog *d = new ProxySettingsDialog();
    if (d->exec() == QDialog::Accepted)
    {
        // apply settings
    }
    delete d;
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

    //     cbProxyType = new QComboBox(gbProxy);
    //     cbProxyType->addItem("HTTP", 3); // QNetworkProxy::HttpProxy
    //     cbProxyType->addItem("SOCKS", 1); // QNetworkProxy::Socks5Proxy

    rbProxyHttp = new QRadioButton("HTTP", this);
    rbProxyHttp->setChecked(true);
    rbProxySocks = new QRadioButton("SOCKS", this);

    blProxyType->addWidget(lProxyType);
    blProxyType->addWidget(rbProxyHttp);
    blProxyType->addSpacing(8);
    blProxyType->addWidget(rbProxySocks);
    //    blProxyType->addWidget(cbProxyType);
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
    separator->setFrameShape(QFrame::HLine);
    //separator->setStyleSheet("QFrame {margin-left: 8px; margin-right: 8px};");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(blManualProxySettings, 1);
    layout->addSpacing(8);
    layout->addWidget(separator);
    layout->addSpacing(8);
    layout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}

void ProxySettingsDialog::on_cProxyAuthRequired_toggled(bool checked)
{
    userNameWidget->setEnabled(checked);
    passwordWidget->setEnabled(checked);
}