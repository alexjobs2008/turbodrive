#include "standaloneaboutwidget.h"

#include <QtGui>
#include <QtWidgets>

#include "QsLog.h"

#include "AppStrings.h"

inline void initResources() { Q_INIT_RESOURCE(standaloneaboutwidgetUI); }

namespace Drive
{

StandaloneAboutWidget& StandaloneAboutWidget::instance()
{
	static StandaloneAboutWidget myself;
	return myself;
}

StandaloneAboutWidget::StandaloneAboutWidget(QWidget *parent)
	: QFrame(parent)
{
	initResources();

    setWindowTitle(QString(trUtf8("МТС Диск")));

    QIcon icon;
    icon.addPixmap(QPixmap(":/appicon/16.png"));
    icon.addPixmap(QPixmap(":/appicon/24.png"));
    icon.addPixmap(QPixmap(":/appicon/32.png"));
    icon.addPixmap(QPixmap(":/appicon/48.png"));
    icon.addPixmap(QPixmap(":/appicon/256.png"));

    setWindowIcon(icon);

    // avoid app close on window close
	setAttribute(Qt::WA_DeleteOnClose, false);
	setAttribute(Qt::WA_QuitOnClose, false);

	setWindowFlags(Qt::CustomizeWindowHint
		| Qt::WindowTitleHint
		| Qt::WindowCloseButtonHint);

	setMinimumWidth(400);

	QVBoxLayout *layoutMain = new QVBoxLayout(this);
	layoutMain->setSpacing(0);
	layoutMain->setContentsMargins(0, 0, 0, 0);

	createHeader(layoutMain);
	createBody(layoutMain);

	QMetaObject::connectSlotsByName(this);
}

void StandaloneAboutWidget::createHeader(QLayout* layout)
{
	auto header = new QFrame(this);

	layout->addWidget(header);
	header->setStyleSheet("QFrame { background-color: white };");

	auto hLayout = new QHBoxLayout(header);
	hLayout->setSpacing(0);

	{
		auto vLayout = new QVBoxLayout(header);
		hLayout->addLayout(vLayout);

		vLayout->setSpacing(0);

		auto appNameLabel = new QLabel(header);
		appNameLabel->setText(Strings::getAppString(Strings::AppFullName));
		appNameLabel->setStyleSheet("QLabel { font-weight: bold };");
		vLayout->addWidget(appNameLabel);

		auto appVersionLabel = new QLabel(header);
		appVersionLabel->setText(trUtf8("Version %1").arg(QCoreApplication::applicationVersion()));
		vLayout->addWidget(appVersionLabel);

		vLayout->addStretch();
	}

	hLayout->addStretch();

	auto appIconLabel = new QLabel(header);
	appIconLabel->setPixmap(QPixmap(":/app.png"));
	hLayout->addWidget(appIconLabel);
}

void StandaloneAboutWidget::createBody(QLayout* layout)
{
	auto body = new QFrame(this);
	layout->addWidget(body);

	auto vLayout = new QVBoxLayout(body);
	vLayout->setSpacing(0);

	vLayout->addStretch();

	auto copyrightLabel = new QLabel(body);
	copyrightLabel->setText(Strings::getAppString(Strings::Copyright));
	vLayout->addWidget(copyrightLabel);

	vLayout->addSpacing(10);

	auto homepageLabel = new QLabel(body);
	homepageLabel->setText(QString("<a href=\"%1\">%2</a>")
						   .arg(QCoreApplication::organizationDomain())
						   .arg(Strings::getAppString(Strings::WebSiteText)));
	homepageLabel->setOpenExternalLinks(true);
	vLayout->addWidget(homepageLabel);
}


}
