//    ---------------------------------------------------------------------------
//
//    Copyright (c) 2013. All rights reserved.
//
//    ---------------------------------------------------------------------------
//
//    File:   LoginWidget.h
//
//    Author: Mikalai Arapau <infoctopus@gmail.com>
//
//    Desc:   Login widget class. Let's user log in with email/password.
//            And also to reset password or to sing up.
//
//    ---------------------------------------------------------------------------

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

namespace CommonUI
{
    class LabeledEdit;
    class LinkLabel;
    class SpinnerWidget;
};

namespace Drive
{

/**
* @class LoginWidget
* @date 2013/11/01
* @brief Login widget. Contains email, password fields and extra links
* @details The widget is styled with auto-loaded CSS, separate styles
           for Win and Mac, see CommonUI::StyleSheetReader
*/
class LoginWidget : public QFrame
{
    Q_OBJECT
public:
    /**
    * @brief Creates instance of LoginWidget, sets up its controls
    * @details CommonUI lib widgets and real-time CSS loading utilized
    * @param[in] parent Parent widget
    * @return LoginWidget object.
    */

    explicit LoginWidget(QWidget *parent = 0);
    virtual ~LoginWidget();

public slots:

    /**
    * @brief Enables/disables controls while login process is active/complete
    * @param[in] enable If controls should be enabled
    */
	void enableControls(bool enable = true);

    /**
    * @brief Focuses text cursor on email field
    * @details Should be used if email isn't recognized
    */
    void focusOnEmail();

    /**
    * @brief Displays error text above email text edit control
    * @details The error text is visible until next login attempt
    */	
    void setError(const QString& text);

    /**
    * @brief Sets the link that's activated upon clicking on the
             corresponding link label
    * @details The link should work for non-authorized users
    */	
    void setRegisterLink(const QString& link);

signals:
    /**
    * @brief Emitted when Sign In button gets clicked or Enter pressed
    */	    
    void loginRequest();

    /**
    * @brief Emitted when user requested password reset
    */	    
    void passwordResetRequest(const QString& email);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void on_signIn_clicked(bool checked);
    void on_signUp_linkActivated(const QString &link);
    void on_forgot_linkActivated(const QString &link);

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

    QString registerLink;
};

}

#endif