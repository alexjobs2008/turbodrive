#ifndef LOGIN_CONTROLLER_H
#define LOGIN_CONTROLLER_H

//#include <QtCore/QObject>
#include <QtCore/QSharedPointer>

class LoginWidget;

class LoginController : public QObject
{
    Q_OBJECT
public:    
    static LoginController& instance();

public slots:
    void showLoginForm();
    void closeAll();

private:
    explicit LoginController(QObject *parent = 0);

    LoginWidget *loginWidget;
};


#endif 