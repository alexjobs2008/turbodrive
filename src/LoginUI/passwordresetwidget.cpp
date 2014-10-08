#include "passwordresetwidget.h"

#include <QtGui/QIcon>

#include <QtGui/QValidator>
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
    username(username),
    errorWindow(new CommonUI::MessageWindow(this))
{
    setBaseSize(QSize(456, 412));
    setWindowTitle(trUtf8("Восстановление пароля"));
    setFocusPolicy(Qt::StrongFocus);

    connect(this, &PasswordResetWidget::finished,
            &LoginController::instance(), &LoginController::onPasswordResetFinished);

    initControls();
}

void PasswordResetWidget::on_resetButton_clicked(bool checked)
{
    (void)checked;

    enableControls(false);
    errorWindow->hide();
    username = phoneInput->text();
    QString testUserName(username);
    int pos = username.size();

    // Check phone number correctness
    QValidator::State validationState =
            phoneInput->lineEdit()->validator()->validate(testUserName, pos);
    bool userNameAcceptable = validationState == QValidator::Acceptable;

    if (!userNameAcceptable)
    {
        moveErrorWindow();
        errorWindow->showText(trUtf8("Неправильный формат номера телефона"));
        enableControls(true);
        return;
    }

    // Issue password reset request
    PasswordResetResourceRef passwordResetResource =
        PasswordResetResource::create();

    connect(passwordResetResource.data(), &PasswordResetResource::resetSuccessfully,
            &LoginController::instance(), &LoginController::onPasswordResetSucceeded);

    connect(passwordResetResource.data(), &PasswordResetResource::resetFailed,
            this, &PasswordResetWidget::onPasswordResetFailed);

    enableControls(false);
    passwordResetResource->resetPassword(username);

}

void PasswordResetWidget::initControls()
{
    QLabel* logoLabel = new QLabel(this);
    logoLabel->setObjectName("logo");
    logoLabel->setPixmap(QPixmap(":/logo.png"));
    logoLabel->setScaledContents(true);

    QLabel* headerLabel = new QLabel(this);
    headerLabel->setObjectName("header");
    headerLabel->setText("Забыли пароль?");
    headerLabel->setAlignment(Qt::AlignLeft);
    // headerLabel->setPixmap(QPixmap(":/PasswordResetWidget/Header.png"));
    // headerLabel->setScaledContents(true);

    QLabel* textLabel = new QLabel(this);
    textLabel->setObjectName("prompt");
    textLabel->setText(trUtf8("Введите свой телефонный номер\nи мы вышлем вам новый пароль."));

    phoneInput = new CommonUI::LabeledEdit(
        QString::null
        , CommonUI::LabeledEdit::Text
        , QString()
        , 0
        , "\\+?375\\d+" // "\\+375\\s\\d{0,2}[\\s\\d{0,7}]"
        , 100
        , this);
    phoneInput->setName("phone");
    phoneInput->setText(username);
    phoneInput->lineEdit()->setPlaceholderText(tr("Телефон")); // +375 ХХ ХХХХХХХ
    // phoneInput->lineEdit()->setInputMask("+375 99 9999999");

    resetButton = new QPushButton(tr("Сбросить пароль"), this);
    resetButton->setObjectName("resetButton");
    resetButton->setAutoDefault(true);
    resetButton->setMouseTracking(true);

    m_spinner = new CommonUI::SpinnerWidget(tr(" Пожалуйста подождите..."),
        ":/spinner/24-", 80, this);

    recalled = new CommonUI::LinkLabel(tr("Вспомнил"), "recalled", this);
    recalled->setObjectName("recalled");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addSpacing(24);
    layout->addWidget(logoLabel, 0, Qt::AlignCenter);
    layout->addSpacing(12);
    layout->addWidget(headerLabel, 0, Qt::AlignCenter);
    layout->addSpacing(12);
    layout->addWidget(textLabel, 0, Qt::AlignCenter);
    layout->addSpacing(12);
    layout->addWidget(phoneInput, 0, Qt::AlignCenter);
    layout->addSpacing(6);
    layout->addWidget(resetButton, 0, Qt::AlignCenter);
    layout->addWidget(m_spinner, 0, Qt::AlignCenter);
    layout->addStretch(4);
    layout->addWidget(recalled, 0, Qt::AlignCenter);

    QMetaObject::connectSlotsByName(this);
    CommonUI::StyleSheetReader::setStyleSheetFor(this);

    RealtimeStyleSheetLoader *rsl = new RealtimeStyleSheetLoader(this);
    rsl->addWidget(this);

    enableControls(true);
}

void PasswordResetWidget::moveEvent(QMoveEvent *)
{
    moveErrorWindow();
}

void PasswordResetWidget::focusInEvent(QFocusEvent *)
{

}

void PasswordResetWidget::focusOutEvent(QFocusEvent *)
{
    errorWindow->hide();
}

void PasswordResetWidget::closeEvent(QCloseEvent *)
{
    errorWindow->hide();
    enableControls(true);
    emit finished();
}


void PasswordResetWidget::on_recalled_linkActivated(const QString&)
{
    errorWindow->hide();
    enableControls(true);
    emit finished();
}

void PasswordResetWidget::onPasswordResetFailed(const QString &message)
{
    enableControls(true);
    moveErrorWindow();
    errorWindow->showText(message);
}

void PasswordResetWidget::enableControls(bool enable)
{
    m_spinner->setOn(!enable);
    phoneInput->setEnabled(enable);
    resetButton->setEnabled(enable);
    recalled->setEnabled(enable);
}

void PasswordResetWidget::moveErrorWindow()
{
    errorWindow->move(
        pos().x() + phoneInput->pos().x() + phoneInput->width(),
                pos().y() + phoneInput->pos().y());

}

}
