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

	QWidget *info = new QWidget(this);
	QVBoxLayout *infoLayout = new QVBoxLayout(info);

	QLabel *labelNameAndVersion = new QLabel(info);
	labelNameAndVersion->setText(QString("%1 %2")
								 .arg(Strings::getAppString(Strings::AppFullName))
								 .arg(QCoreApplication::applicationVersion()));

	QLabel *labelHomepage = new QLabel(info);
	labelHomepage->setText(QString("%1 <a href=\"%2\">%3</a>")
						   .arg(tr("Homepage:"))
						   .arg(QCoreApplication::organizationDomain())
						   .arg(Strings::getAppString(Strings::WebSiteText)));
	labelHomepage->setOpenExternalLinks(true);

	QLabel *labelCopyright =
		new QLabel(Strings::getAppString(Strings::Copyright));

	infoLayout->setSpacing(0);
	infoLayout->addWidget(labelNameAndVersion);
	infoLayout->addWidget(labelHomepage);
	infoLayout->addWidget(labelCopyright);
	infoLayout->addStretch(1);

	mainLayout->addWidget(info);
}

}
