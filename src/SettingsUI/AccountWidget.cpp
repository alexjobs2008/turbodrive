#include "AccountWidget.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>


AccountWidget::AccountWidget(QWidget *parent)
    : QFrame(parent)
{
    QLabel *loggedAsLabel = new QLabel(tr("Logged in as:"), this);
    QLabel *loginLabel = new QLabel("mobile@turboheads.by", this);
    loginLabel->setStyleSheet("QLabel {font-weight: bold};");
    
    QPushButton *pbResetPassword = new QPushButton(tr("Reset Password"), this);
    pbResetPassword->setMaximumWidth(104);
    pbResetPassword->setMinimumWidth(104);
    QPushButton *pbLogout = new QPushButton(tr("Logout..."), this);
    pbLogout->setMaximumWidth(104);
    QHBoxLayout *blButtons = new QHBoxLayout();
    blButtons->addWidget(pbResetPassword);
    blButtons->addWidget(pbLogout);
    blButtons->addStretch(1);

    QLabel *folderLabel = new QLabel(this);
    folderLabel->setObjectName("folderLabel");
    folderLabel->setText("<a href=\"gotothefolder\"><img src=\":/folder.png\" /></a>");
    folderLabel->setToolTip(tr("Open Drive folder"));

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
    rightLayout->addWidget(loginLabel);
    rightLayout->addSpacing(8);
    rightLayout->addLayout(blButtons);
    rightLayout->addStretch(1);

    mainLayout->addSpacing(16);
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 0);

    QMetaObject::connectSlotsByName(this);
}

void AccountWidget::on_folderLabel_linkActivated(const QString &link)
{
    Q_UNUSED(link);

    emit openFolder();

}