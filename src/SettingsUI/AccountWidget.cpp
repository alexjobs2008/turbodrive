#include "AccountWidget.h"

#include "APIClient/ApiTypes.h"
#include "Util/AppStrings.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

namespace Drive
{

AccountWidget::AccountWidget(QWidget *parent)
	: QFrame(parent)
{
	QLabel *loggedAsLabel = new QLabel(tr("Logged in as:"), this);
	nameLabel = new QLabel(this);
	loginLabel = new QLabel(this);
	nameLabel->setStyleSheet("QLabel {font-weight: bold};");
	loginLabel->setStyleSheet(nameLabel->styleSheet());

	avatarLabel = new QLabel(this);
	avatarLabel->setPixmap(QPixmap(":/user.png"));
	avatarLabel->setStyleSheet("QLabel {border: 1px solid black; min-height: 128px; max-height: 128px; min-width: 128px; max-width: 128px;}");

	QPushButton *pbResetPassword = new QPushButton(tr("Reset Password"), this);
	QPushButton *pbLogout = new QPushButton(tr("Logout"), this);
	pbLogout->setObjectName("pbLogout");
	QVBoxLayout *blButtons = new QVBoxLayout();
	blButtons->addWidget(pbLogout);
	blButtons->addWidget(pbResetPassword);
	blButtons->addStretch(1);

	QLabel *folderLabel = new QLabel(this);
	folderLabel->setObjectName("folderLabel");
#ifdef Q_OS_WIN
	folderLabel->setText("<a href=\"gotothefolder\"><img src=\":/folder-win-128.png\" /></a>");
#endif

#ifdef Q_OS_MAC
	folderLabel->setText("<a href=\"gotothefolder\"><img src=\":/folder-mac-256.png\" /></a>");
#endif

#ifdef Q_OS_LINUX
	folderLabel->setText("<a href=\"gotothefolder\"><img src=\":/folder-win-128.png\" /></a>");
#endif

	folderLabel->setToolTip(QString(tr("Open %1 folder", "%1 - product name"))
		.arg(Strings::getAppString(Strings::AppFullName)));

	//QLabel *spaceLabel = new QLabel("17 KB (8 files) out of 100 GB", this);

	QLabel *statusLabel = new QLabel("Sync status: Up to date", this);

	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	QVBoxLayout *leftLayout = new QVBoxLayout();
	QVBoxLayout *rightLayout = new QVBoxLayout();

	leftLayout->addSpacing(4);
	leftLayout->addWidget(folderLabel);
	leftLayout->addSpacing(8);
	leftLayout->addWidget(statusLabel);
	leftLayout->addStretch(1);

	rightLayout->addWidget(loggedAsLabel);
	rightLayout->addWidget(nameLabel);
	rightLayout->addWidget(loginLabel);
	rightLayout->addWidget(avatarLabel);
	rightLayout->addSpacing(8);
	rightLayout->addLayout(blButtons);
	rightLayout->addStretch(1);

	mainLayout->addSpacing(16);
	mainLayout->addLayout(leftLayout, 1);
	mainLayout->addLayout(rightLayout, 0);

	QMetaObject::connectSlotsByName(this);
}

void AccountWidget::setProfileData(const ProfileData& profileData)
{
	nameLabel->setText(QString("%1 %2")
		.arg(profileData.firstName)
		.arg(profileData.lastName));

	loginLabel->setText(profileData.email);

	if (!profileData.avatar.isNull())
		avatarLabel->setPixmap(profileData.avatar);
}

void AccountWidget::on_folderLabel_linkActivated(const QString &link)
{
	Q_UNUSED(link);
	emit openFolder();
}

void AccountWidget::on_pbLogout_clicked(bool checked)
{
	emit logout();
}

}