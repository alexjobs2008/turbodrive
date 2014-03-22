#include "AboutWidget.h"
#include "Util/AppStrings.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtCore/QCoreApplication>

namespace Drive
{

AboutWidget::AboutWidget(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    
    QLabel *pic = new QLabel(this);
    pic->setPixmap(QPixmap(":/clouds.png"));

    QWidget *info = new QWidget(this);
    QVBoxLayout *infoLayout = new QVBoxLayout(info);

    QLabel *labelCompanyName = new QLabel(info);
    labelCompanyName->setObjectName("labelCompanyName");
    labelCompanyName->setText(Strings::getAppString(Strings::CompanyName));

    QLabel *labelAppName = new QLabel(info);
    labelAppName->setObjectName("labelAppName");
    labelAppName->setText(Strings::getAppString(Strings::AppName));

    labelAppName->setStyleSheet("QLabel {font-size: 12pt;};");

    QLabel *labelAppVersion = new QLabel(info);
    labelAppVersion->setObjectName("labelAppVersion");
    labelAppVersion->setText(QString("v%1").arg(Strings::appVersion));

    QLabel *labelHomepage = new QLabel(tr("Homepage:"), this);
    labelHomepage->setObjectName("labelHomepage");
    
    QLabel *labelHomepageLink = new QLabel(
        QString("<a href=\"%1\">%2</a>")
        .arg(Strings::websiteLink)
        .arg(Strings::getAppString(Strings::WebSiteText))
        , info);
    labelHomepageLink->setObjectName("labelHomepageLink");
    labelHomepageLink->setOpenExternalLinks(true);

    QLabel *labelCopyright =
        new QLabel(Strings::getAppString(Strings::Copyright));
    labelCopyright->setObjectName("labelCopyright");
    labelCopyright->setAlignment(Qt::AlignRight);

    infoLayout->setSpacing(0);
    infoLayout->addWidget(labelCompanyName, 0, Qt::AlignRight);
    infoLayout->addWidget(labelAppName, 0, Qt::AlignRight);
    infoLayout->addWidget(labelAppVersion, 0, Qt::AlignRight);    
    infoLayout->addWidget(labelHomepage, 0, Qt::AlignRight);    
    infoLayout->addWidget(labelHomepageLink, 0, Qt::AlignRight);
    infoLayout->addStretch(1);
    infoLayout->addWidget(labelCopyright, 0, Qt::AlignRight);
    
    mainLayout->addWidget(pic);
    mainLayout->addWidget(info, 1);
}

}