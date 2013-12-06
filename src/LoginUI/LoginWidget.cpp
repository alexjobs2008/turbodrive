#include "LoginWidget.h"
#include "Settings/settings.h"
#include "QsLog/QsLog.h"
#include "Util/AppStrings.h"
#include "UtilUI/StylesheetReader.h"
#include "UtilUI/AuxWidgets.h"
#include "UtilUI/ExtraLabels.h"
#include "UtilUI/SpinnerWidget.h"
#include "Application/AppController.h"

#include "rtCSS/RealtimeStyleSheetLoader.h"

#include <QtCore/QTimerEvent>
#include <QtCore/QList>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QDir>

#include <QtGui/QIcon>
#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopServices>
#include <QtGui/QValidator>

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkCookie>
#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QSslError>






namespace Drive
{

LoginWidget::LoginWidget(QWidget *parent)
    : QFrame(parent)
    , registerLink(QString())
{
    man = new QNetworkAccessManager(this);

    Q_INIT_RESOURCE(loginUI);
    
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_QuitOnClose, false);
    
    setWindowTitle(QString(tr("%1 %2", "%1 - app name, %2 - Sign In"))
        .arg(Strings::appName)
        .arg(tr("Sign In")));

    QIcon icon;
    icon.addPixmap(QPixmap(":/appicon/16.png"));
    icon.addPixmap(QPixmap(":/appicon/24.png"));
    icon.addPixmap(QPixmap(":/appicon/32.png"));
    icon.addPixmap(QPixmap(":/appicon/48.png"));
    icon.addPixmap(QPixmap(":/appicon/256.png"));

    QList<QSize> list = icon.availableSizes();
    QListIterator<QSize> i(list);

    QLOG_INFO() << "icon images count: " << list.size();

    while (i.hasNext())
    {
        QSize s = i.next();
        QLOG_INFO() << s.height() << "x" << s.width();
    }

    setWindowIcon(icon);

    setWindowFlags(Qt::CustomizeWindowHint
        | Qt::WindowTitleHint
        | Qt::WindowCloseButtonHint);

    
    // spinner
    
    spinner = new CommonUI::SpinnerWidget(tr("Please wait..."),
        ":/spinner/24-", 80, this);

    // controls

    username = new CommonUI::LabeledEdit(
        tr("&Email:")
        , CommonUI::LabeledEdit::Text
        , QString()
        , 0
        , "\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}\\b"
        , 100
        , this);

    username->setName("username");

    password = new CommonUI::LabeledEdit(
        tr("&Password:")
        , CommonUI::LabeledEdit::Text
        , QString()
        , 0
        , QString()
        , 0
        , this);

    password->setName("password");
    password->lineEdit()->setEchoMode(QLineEdit::Password);

    forgot = new CommonUI::LinkLabel(tr("Forgot?"), "forgot", this);
    forgot->setObjectName("forgot");

    password->layout()->addWidget(forgot, 1);
    password->layout()->addStretch(1);

    cRememberPassword = new QCheckBox(
        QString(tr("&Sign me in when Drive starts"))
        .arg(Strings::appName)
        , this);

    cRememberPassword->setObjectName("rememberPassword");	

    // Sign In or Sign Up
    QLabel *lAligner = new QLabel("", this);
    lAligner->setObjectName("bottomAligner");

    pbSignIn = new QPushButton(tr("Sign in"), this);
    pbSignIn->setObjectName("signIn");

    pbSignIn->setAutoDefault(true);
    pbSignIn->setMouseTracking(true);

    QLabel *or = new QLabel(tr("or"), this);
    or->setObjectName("or");

    signUp = new CommonUI::LinkLabel(tr("Create account"), "signUp", this);
    signUp->setObjectName("signUp");
    signUp->setEnabled(false);

    actionsFrame = new QFrame(this);
    actionsFrame->setObjectName("actionsFrame");
    actionsFrame->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *blActions = new QHBoxLayout(actionsFrame);
    blActions->setContentsMargins(0, 0, 0, 0);
    blActions->setSpacing(0);

    blActions->addWidget(lAligner, 0);
    blActions->addWidget(pbSignIn, 0, Qt::AlignLeft | Qt::AlignVCenter);
    blActions->addWidget(or, 0, Qt::AlignLeft | Qt::AlignVCenter);
    blActions->addWidget(signUp, 0, Qt::AlignLeft | Qt::AlignVCenter);
    blActions->addStretch(1);

    // main layout
        
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    layout->addWidget(spinner);
    layout->addWidget(username);
    layout->addWidget(password);
    layout->addSpacing(4);
    layout->addWidget(cRememberPassword);
    layout->addSpacing(4);
    layout->addWidget(actionsFrame);
    layout->addStretch(1);
    
    username->setText(Settings::instance().get(Settings::email).toString());
    password->setText(Settings::instance().get(Settings::password).toString());
    cRememberPassword->
        setChecked(Settings::instance().get(Settings::autoLogin).toBool());

    QMetaObject::connectSlotsByName(this);

    CommonUI::StyleSheetReader::setStyleSheetFor(this);

    username->installEventFilter(this);
    password->installEventFilter(this);
    cRememberPassword->installEventFilter(this);

    RealtimeStyleSheetLoader *rsl = new RealtimeStyleSheetLoader(this);
    rsl->addWidget(this);
}

LoginWidget::~LoginWidget()
{
}

bool LoginWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == username
        || watched == password
        || watched == cRememberPassword)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Return
                || keyEvent->key() == Qt::Key_Enter)
            {
                pbSignIn->click();
                return true;
            }
        }
    }

    return false;
}

void LoginWidget::closeEvent(QCloseEvent *event)
{
    this->showMinimized();
    event->ignore();
}

void LoginWidget::on_signIn_clicked(bool checked)
{
    Q_UNUSED(checked)

    QString usernameCandidate = username->lineEdit()->text().trimmed();
    int pos = usernameCandidate.size();

    if (username->lineEdit()->validator()->validate(usernameCandidate, pos)
        != QValidator::Acceptable)
    {
        setError(tr("Please provide a valid email"));
        focusOnEmail();
        return;
    }

    if (password->lineEdit()->text().trimmed().isEmpty())
    {
        setError(tr("Please provide a non-empty password"));
        password->lineEdit()->setText(QString());
        password->lineEdit()->setFocus(Qt::MouseFocusReason);
        return;
    }

    AppController::instance().setState(Authorizing);

    Settings::instance().set(Settings::email,
        username->text().trimmed(), Settings::RealSetting);

    Settings::instance().set(Settings::password,
        password->text().trimmed(), Settings::RealSetting);

    Settings::instance().set(Settings::autoLogin,
        cRememberPassword->isChecked(), Settings::RealSetting);

    QLOG_TRACE() << "Login from UI requested";
    
    enableControls(false);
    
    emit loginRequest();
}

void LoginWidget::timerEvent(QTimerEvent *event)
{
}

void LoginWidget::enableControls(bool enable)
{
    spinner->setOn(!enable);
    username->setEnabled(enable);
    password->setEnabled(enable);
    cRememberPassword->setEnabled(enable);
    actionsFrame->setEnabled(enable);

    if (registerLink.isEmpty())
    {
        signUp->setEnabled(false);
    }
}

void LoginWidget::focusOnEmail()
{
    username->lineEdit()->selectAll();
    username->lineEdit()->setFocus(Qt::MouseFocusReason);
}

void LoginWidget::setError(const QString& text)
{
	spinner->setSubstText(text);
}

void LoginWidget::setRegisterLink(const QString& link)
{
    registerLink = link;
    signUp->setEnabled(actionsFrame->isEnabled());
}

void LoginWidget::on_signUp_linkActivated(const QString &link)
{
    Q_UNUSED(link)
    QDesktopServices::openUrl(QUrl(registerLink));
}

void LoginWidget::on_forgot_linkActivated(const QString &link)
{
    emit passwordResetRequest(username->text().trimmed());
}

void LoginWidget::sslErrors(QNetworkReply * reply, const QList<QSslError> & errors)
{
    for (int i = 0; i < errors.size(); i++)
    {
        QSslError error = errors.at(i);
        QLOG_INFO() << "SSL Error: " << error.errorString();
    }

    reply->ignoreSslErrors();
}

void LoginWidget::onFinished(QNetworkReply *reply)
{   
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    
    QLOG_INFO() << "URL: " << reply->url();
    QLOG_INFO() << "STATUS: " << status;
    QLOG_INFO() << "ERROR: " << reply->error() << ":" << reply->errorString();
    QLOG_INFO() << data;
    QLOG_INFO() << reply->rawHeaderPairs();
    QLOG_INFO() << "SIZE: " << reply->bytesAvailable();
    
    QString s = QString(reply->readAll());
    
    QLOG_INFO() << "BODY: " << s;

    if (status == 200)
    {
        QJsonObject jo = QJsonDocument::fromJson(s.toLocal8Bit()).object();

        QString token;

        if (jo.contains("data"))
        {
            token = jo.value("data").toString();
        }

        QLOG_INFO() << "Token: " << token;

        QNetworkRequest *req = new QNetworkRequest(QUrl("https://api.assistent.by/api/v1/user"));
        
//        QNetworkRequest *req = new QNetworkRequest(QUrl("http://files.new.assistent.by/api/v1/files/getChildren/727272"));

        req->setRawHeader("X-Sso-Token", token.toLocal8Bit());
//        req->setRawHeader("X-Current-Workspace-Id", "43");

        

        man->get(*req);    
    }

    if(status == 200)
    {
        AppController::instance().setState(Syncing);
        setFolder();
        close();
    }
    else
    {
        if (AppController::instance().state() == Authorizing)
        {
            AppController::instance().setState(NotAuthorized);
        }
    }
}

void LoginWidget::setFolder()
{
    QString folderPath =
        Settings::instance().get(Settings::folderPath).toString();

    QDir dir;
    dir.mkpath(folderPath);


}

}