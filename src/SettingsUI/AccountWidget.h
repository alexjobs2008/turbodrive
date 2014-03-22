#ifndef ACCOUNT_WIDGET_H
#define ACCOUNT_WIDGET_H

#include <QtWidgets/QFrame>

class QLabel;

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

private slots:
	void on_folderLabel_linkActivated(const QString &);
	void on_pbLogout_clicked(bool checked);

private:
	QLabel *nameLabel;
	QLabel *loginLabel;
	QLabel *avatarLabel;

};

class UserDataWidget : public QFrame
{
	Q_OBJECT
public:
	UserDataWidget(QWidget *parent = 0);



};

}

#endif ACCOUNT_WIDGET_H