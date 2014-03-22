#ifndef LOGIN_WIDGET_H
#define LOGIN_WIDGET_H

#include <QtWidgets/QFrame>
#include "QsLog/QsLog.h"

class QFrame;
class PixmapAnimation;
class QLineEdit;
class QCheckBox;
class QPushButton;
class QLabel;

class QCloseEvent;
class QSslError;

namespace CommonUI
{
    class LabeledEdit;
    class LinkLabel;
    class SpinnerWidget;
};

class QNetworkAccessManager;
class QNetworkReply;

namespace Drive
{

class LoginWidget : public QFrame
{
    Q_OBJECT
public:
    LoginWidget(QWidget *parent = 0);
    ~LoginWidget();

    bool eventFilter(QObject *watched, QEvent *event);

public slots:
	void enableControls(bool enable = true);
    void focusOnEmail();
	void setError(const QString& text);
    void setRegisterLink(const QString& link);

signals:
    void loginRequest();
    void passwordResetRequest(const QString& email);

protected:
    void timerEvent(QTimerEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void on_signIn_clicked(bool checked);
    void on_signUp_linkActivated(const QString &link);
    void on_forgot_linkActivated(const QString &link);

    void onFinished(QNetworkReply *reply);
    void sslErrors(QNetworkReply * reply, const QList<QSslError> & errors);

private:    
    void setFolder();

    CommonUI::SpinnerWidget *spinner;

    CommonUI::LabeledEdit *username;
    CommonUI::LabeledEdit *password;
    CommonUI::LinkLabel *forgot;
    QCheckBox *cRememberPassword;
    QFrame *actionsFrame;
    QPushButton *pbSignIn;
    CommonUI::LinkLabel *signUp;

    QNetworkAccessManager* man;
    QByteArray data;

    QString registerLink;
};

}

#endif