#ifndef CONNECTION_WIDGET_H
#define CONNECTION_WIDGET_H

#include <QtWidgets/QFrame>

class QRadioButton;
class QLineEdit;
class QLabel;
class QBoxLayout;
class QComboBox;
class QCheckBox;

class ConnectionWidget : public QFrame
{
    Q_OBJECT
public:
    ConnectionWidget(QWidget *parent = 0);

private slots:
    void on_rbDownloadNoLimit_toggled(bool checked);
    void on_rbUploadNoLimit_toggled(bool checked);
    void on_rbManualProxy_toggled(bool checked);
    void on_cProxyAuthRequired_toggled(bool checked);    

private:
    QBoxLayout* createBandwidthControls();
    QWidget* createProxyControls();


    QRadioButton *rbDownloadNoLimit;
    QLineEdit *leDownloadLimit;
    QLabel *lDownloadLimit;

    QRadioButton *rbUploadNoLimit;
    QLineEdit *leUploadLimit;
    QLabel *lUploadLimit;

    QRadioButton *rbNoProxy;
    QRadioButton *rbAutoProxy;
    QRadioButton *rbManualProxy;

    QWidget *manualProxySettingsWidget;
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

#endif CONNECTION_WIDGET_H

