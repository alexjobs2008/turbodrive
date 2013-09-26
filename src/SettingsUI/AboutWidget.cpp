#include "AboutWidget.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtCore/QCoreApplication>

AboutWidget::AboutWidget(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    
    QLabel *pic = new QLabel(this);
    pic->setPixmap(QPixmap(":/clouds.png"));

    QWidget *info = new QWidget(this);
    QVBoxLayout *infoLayout = new QVBoxLayout(info);

    QLabel *appName = new QLabel(info);
    appName->setText(QString("%1 %2")
        .arg(QCoreApplication::organizationName())
        .arg(QCoreApplication::applicationName()));

    appName->setStyleSheet("QLabel {font-size: 12pt;};");

    QLabel *appVersion = new QLabel(info);
    appVersion->
        setText(QString("v%1").arg(QCoreApplication::applicationVersion()));

    QLabel *www =
        new QLabel("<a href=\"http://assistent.by\">www.assistent.by</a>", info);
    www->setOpenExternalLinks(true);

    QLabel *copyright = new QLabel("Copyright (c) 2013, Assistent");

    //infoLayout->addSpacing(16);
    infoLayout->addWidget(appName, 0, Qt::AlignRight);
    infoLayout->addWidget(appVersion, 0, Qt::AlignRight);    
    infoLayout->addWidget(www, 0, Qt::AlignRight);
    infoLayout->addStretch(1);
    infoLayout->addWidget(copyright, 0, Qt::AlignRight);
    
    mainLayout->addWidget(pic);
    mainLayout->addWidget(info, 1);
}