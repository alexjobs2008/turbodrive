#include "LoginWidget.h"

#include <QtGui/QIcon>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

#define MIN_CONNECTION_TAB_LABEL_WIDTH 150

LoginWidget::LoginWidget(QWidget *parent)
    : QFrame(parent)
{
    Q_INIT_RESOURCE(loginUI);
    
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_QuitOnClose, false);
    
    setWindowTitle(tr("turboDrive Sign In"));
    setWindowIcon(QIcon(":/appicon.ico"));

    // avoid app close on window close
    setAttribute(Qt::WA_QuitOnClose, false);

    setWindowFlags(Qt::CustomizeWindowHint
        | Qt::WindowTitleHint
        | Qt::WindowCloseButtonHint);

    setStyleSheet("LoginWidget {"
        "background: url(\":back.png\") no-repeat top center; };"
        //"magrin: 16px 16px 16px 16px;"
        "};");

    QWidget *userNameWidget = new QWidget(this);    
    QHBoxLayout *blUsername = new QHBoxLayout(userNameWidget);
    blUsername->setContentsMargins(0, 0, 0, 0);
    QLabel *lUsername = new QLabel(tr("Email:"), this);
    lUsername->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH);
    lUsername->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QLineEdit *leUsername = new QLineEdit(this);
    blUsername->addWidget(lUsername);
    blUsername->addWidget(leUsername);
    blUsername->addStretch(1);

    // Password
    QWidget *passwordWidget = new QWidget(this);    
    QHBoxLayout *blPassword = new QHBoxLayout(passwordWidget);
    blPassword->setContentsMargins(0, 0, 0, 0);
    QLabel *lPassword = new QLabel(tr("Password:"), this);
    lPassword->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH);
    lPassword->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QLineEdit *lePassword = new QLineEdit(this);
    lePassword->setEchoMode(QLineEdit::Password);
    blPassword->addWidget(lPassword);
    blPassword->addWidget(lePassword);
    blPassword->addStretch(1);

    QHBoxLayout *blForgot = new QHBoxLayout();
    QLabel *lHelper = new QLabel(this);
    lHelper->setMinimumWidth(MIN_CONNECTION_TAB_LABEL_WIDTH + 48);
    
    
    QLabel *www =
        new QLabel("<a href=\"http://assistent.by\">Forgot password?</a>", this);
    www->setOpenExternalLinks(true);

    blForgot->addWidget(lHelper);
    blForgot->addWidget(www);
    blForgot->addStretch(1);

    QHBoxLayout *blButtons = new QHBoxLayout();
    QPushButton *pbCreateAccount = new QPushButton(tr(" Create Account "), this);
    QPushButton *pbLogin = new QPushButton(tr("Login"), this);
    pbLogin->setDefault(true);

    blButtons->addWidget(pbCreateAccount);
    blButtons->addStretch(1);
    blButtons->addWidget(pbLogin);

    resize(450, 540);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addSpacing(350);
    layout->addWidget(userNameWidget);
    layout->addSpacing(4);
    layout->addWidget(passwordWidget);
    layout->addSpacing(4);
    layout->addLayout(blForgot);
    layout->addStretch(1);
    layout->addLayout(blButtons);
    layout->addSpacing(4);
}
