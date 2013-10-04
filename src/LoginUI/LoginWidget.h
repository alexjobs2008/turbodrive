#ifndef LOGIN_WIDGET_H
#define LOGIN_WIDGET_H

#include <QtWidgets/QFrame>
#include "QsLog/QsLog.h"

class QLineEdit;

class LoginWidget : public QFrame
{
    Q_OBJECT
public:
    LoginWidget(QWidget *parent = 0);
    ~LoginWidget() { QLOG_DEBUG() << "abra"; };

signals:
    void loginRequest();

private slots:
    void on_login_clicked(bool checked);

private:
    QLineEdit *leUsername;
    QLineEdit *lePassword;
};

#endif 