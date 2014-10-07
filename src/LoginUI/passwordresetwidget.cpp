#include "passwordresetwidget.h"

#include <QtGui/QIcon>

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>

#include "UtilUI/AuxWidgets.h"
#include "DashboardService.h"
#include "LoginController.h"
#include "UtilUI/StylesheetReader.h"
#include "rtCSS/RealtimeStyleSheetLoader.h"

namespace Drive
{


PasswordResetWidget::PasswordResetWidget(
    const QString& username, QWidget *parent) :
    QFrame(parent),
    username(username)
{
    setBaseSize(QSize(456, 412));

    QIcon icon;
    icon.addPixmap(QPixmap(":/appicon/16.png"));
    icon.addPixmap(QPixmap(":/appicon/24.png"));
    icon.addPixmap(QPixmap(":/appicon/32.png"));
    icon.addPixmap(QPixmap(":/appicon/48.png"));
    icon.addPixmap(QPixmap(":/appicon/256.png"));

    setWindowIcon(icon);

    connect(this, &PasswordResetWidget::finished,
            &LoginController::instance(), &LoginController::onPasswordResetSucceeded);

    initControls();
}

void PasswordResetWidget::on_resetButton_clicked(bool checked)
{
    (void)checked;

    username = phoneInput->text();

    PasswordResetResourceRef passwordResetResource =
        PasswordResetResource::create();

    connect(passwordResetResource.data(), &PasswordResetResource::resetSuccessfully,
            &LoginController::instance(), &LoginController::onPasswordResetSucceeded);

    connect(passwordResetResource.data(), &PasswordResetResource::resetFailed,
            &LoginController::instance(), &LoginController::onPasswordResetFailed);

    passwordResetResource->resetPassword(username);

}

void PasswordResetWidget::initControls()
{
    QLabel* headerLabel = new QLabel(this);
    headerLabel->setObjectName("header");
    headerLabel->setPixmap(QPixmap(":/PasswordResetWidget/Header.png"));
    headerLabel->setScaledContents(true);

    QLabel* textLabel = new QLabel(this);
    textLabel->setObjectName("prompt");
    textLabel->setText(tr("Не проблема! Введите свой телефонный\nномер и мы вышлем новый пароль."));

    phoneInput = new CommonUI::LabeledEdit(
        QString::null
        , CommonUI::LabeledEdit::Text
        , QString()
        , 0
        , "\\+?375\\d+"
        , 100
        , this);
    phoneInput->setName("phone");
    phoneInput->setText(username);
    phoneInput->lineEdit()->setPlaceholderText(tr("Телефон")); // +375 ХХ ХХХХХХХ

    resetButton = new QPushButton(tr(""), this);
    resetButton->setObjectName("resetButton");
    resetButton->setAutoDefault(true);
    resetButton->setMouseTracking(true);
    // resetButton->setStyleSheet("border-image: url(':/PasswordResetWidget/ResetPasswordButton.png') 5;");

    recalled = new CommonUI::LinkLabel(tr("Вспомнил"), "recalled", this);
    recalled->setObjectName("recalled");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addSpacing(20);
    layout->addWidget(headerLabel, 0, Qt::AlignCenter);
    layout->addSpacing(12);
    layout->addWidget(textLabel, 0, Qt::AlignCenter);
    layout->addSpacing(12);
    layout->addWidget(phoneInput, 0, Qt::AlignCenter);
    layout->addSpacing(6);
    layout->addWidget(resetButton, 0, Qt::AlignCenter);
    layout->addStretch(1);
    layout->addWidget(recalled, 0, Qt::AlignCenter);

    QMetaObject::connectSlotsByName(this);
    CommonUI::StyleSheetReader::setStyleSheetFor(this);

    RealtimeStyleSheetLoader *rsl = new RealtimeStyleSheetLoader(this);
    rsl->addWidget(this);
}


void PasswordResetWidget::on_recalled_linkActivated(const QString&)
{
    emit finished();
}

void PasswordResetWidget::closeEvent(QCloseEvent *)
{
    emit finished();
}

}
