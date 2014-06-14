#ifndef CONNECTION_WIDGET_H
#define CONNECTION_WIDGET_H

#include <QtWidgets/QFrame>
#include <QtWidgets/QDialog>

class QRadioButton;
class QLineEdit;
class QLabel;
class QBoxLayout;
class QComboBox;
class QCheckBox;
class QPushButton;

namespace Drive
{

struct ProxySettings;

class ConnectionWidget : public QFrame
{
	Q_OBJECT
public:
	ConnectionWidget(QWidget *parent = 0);

private slots:
	void on_downloadNoLimit_toggled(bool checked);
	void on_uploadNoLimit_toggled(bool checked);

	void on_noProxy_toggled(bool checked);
	void on_autoProxy_toggled(bool checked);
	void on_manualProxy_toggled(bool checked);

	void on_proxyConfigure_clicked(bool checked);

private:
	QBoxLayout* createBandwidthControls();
	QWidget* createProxyControls();
	void setFromSettings();

	QRadioButton *rbDownloadNoLimit;
	QRadioButton *rbDownloadLimit;
	QLineEdit *leDownloadLimit;
	QLabel *lDownloadLimit;

	QRadioButton *rbUploadNoLimit;
	QRadioButton *rbUploadLimit;
	QLineEdit *leUploadLimit;
	QLabel *lUploadLimit;

	QRadioButton *rbNoProxy;
	QRadioButton *rbAutoProxy;
	QRadioButton *rbManualProxy;

	QWidget *manualProxySettingsWidget;
	QPushButton *pbProxyConfigure;
};

class ProxySettingsDialog : public QDialog
{
	Q_OBJECT
public:
	ProxySettingsDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
	ProxySettings proxySettings() const;

public slots:
	int exec();

private slots:
	void on_cProxyAuthRequired_toggled(bool checked);

private:
	QWidget *userNameWidget;
	QWidget *passwordWidget;
	QComboBox *cbProxyType;

	QRadioButton *rbProxyHttp;
	QRadioButton *rbProxySocks;

	QLineEdit *leProxyServer;
	QLineEdit *leProxyPort;
	QCheckBox *cProxyAuthRequired;
	QLineEdit *leProxyUsername;
	QLineEdit *leProxyPassword;
};

}

#endif // CONNECTION_WIDGET_H

