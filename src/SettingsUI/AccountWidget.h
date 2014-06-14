#ifndef ACCOUNT_WIDGET_H
#define ACCOUNT_WIDGET_H

#include <QtWidgets/QFrame>

class QLabel;
class QPushButton;

namespace Drive
{

struct ProfileData;

class AccountWidget : public QFrame
{
	Q_OBJECT
public:
	AccountWidget(QWidget *parent = 0);

public slots:
	void setProfileData(const ProfileData& profileData);

signals:
	void openFolder();
	void logout();
	void resetPassword(const QString& username);

private slots:
	void on_folderLabel_linkActivated(const QString &);
	void on_pbLogout_clicked(bool checked);
	void on_m_resetPasswordPushButton_clicked(bool);

private:
	QLabel *nameLabel;
	QLabel *loginLabel;
	QLabel *avatarLabel;
	QPushButton* m_resetPasswordPushButton;
};

class UserDataWidget : public QFrame
{
	Q_OBJECT
public:
	UserDataWidget(QWidget *parent = 0);



};

}

#endif // ACCOUNT_WIDGET_H
