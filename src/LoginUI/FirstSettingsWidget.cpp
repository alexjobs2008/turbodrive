#include "FirstSettingsWidget.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QPushButton>

FirstSettingsWidget::FirstSettingsWidget(QWidget *parent)
	: QFrame(parent)
{
	rbTypicalSettings = new QRadioButton(tr(
		"Typical settings (recommended)\n"
		"Set up Drive with normal settings.")
		, this);

	rbTypicalSettings->setObjectName("typicalSettings");

	rbAdvancedSettings = new QRadioButton(tr(
		"Advanced settings\n"
		"Choose Drive folder location and which folders and files to sync.")
		, this);

	rbAdvancedSettings->setObjectName("advancedSettings");

	QMetaObject::connectSlotsByName(this);
}

void FirstSettingsWidget::on_typicalSettings_toggled(bool)
{
}

void FirstSettingsWidget::on_advancedSettings_toggled(bool)
{
}
