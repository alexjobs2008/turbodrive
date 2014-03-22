#include "LoginController.h"
#include "LoginUI/LoginWidget.h"

LoginController& LoginController::instance()
{
    static LoginController myself;
    return myself;
}

LoginController::LoginController(QObject *parent)
    : QObject(parent)
    , loginWidget(0)
{
}

void LoginController::showLoginForm()
{
    loginWidget = new LoginWidget();
    loginWidget->show();
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