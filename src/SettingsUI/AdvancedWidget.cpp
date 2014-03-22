#include "AdvancedWidget.h"
#include "Util/AppStrings.h"
#include "Settings/settings.h"
#include "SelectiveSyncDialog.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QCheckBox>

#define MIN_ADVANCED_TAB_LABEL_WIDTH 96
#define ADVANCED_TAB_CONTROL_FIXED_WIDTH 236

namespace Drive
{

AdvancedWidget::AdvancedWidget(QWidget *parent)
	: QFrame(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	// Show desktop notifications
	QHBoxLayout *blNotifications = new QHBoxLayout();
	QLabel *lAligner = new QLabel(this);
	lAligner->setMinimumWidth(MIN_ADVANCED_TAB_LABEL_WIDTH);
	lAligner->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	cNotifications = new QCheckBox(tr("Show desktop notifications"), this);
	cNotifications->setObjectName("desktopNotifications");
	blNotifications->addWidget(lAligner);
	blNotifications->addWidget(cNotifications);
	blNotifications->addStretch(1);

	// Selective Sync
	QLabel *lSync = new QLabel(tr("Selective Sync:"), this);
	lSync->setMinimumWidth(MIN_ADVANCED_TAB_LABEL_WIDTH);
	lSync->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	QPushButton *pbSync = new QPushButton(
		tr("Select which folders and files to sync..."), this);
	pbSync->setObjectName("selectiveSync");


	pbSync->setMinimumWidth(ADVANCED_TAB_CONTROL_FIXED_WIDTH);
	pbSync->setMaximumWidth(ADVANCED_TAB_CONTROL_FIXED_WIDTH);

	QHBoxLayout *blSync = new QHBoxLayout();

	blSync->addWidget(lSync);
	blSync->addWidget(pbSync, 0, Qt::AlignLeft | Qt::AlignVCenter);
	blSync->addStretch(1);

	// Drive folder
	QLabel *lFolderPath = new QLabel(this);
	lFolderPath->setText(QString(tr("%1 folder"))
		.arg(Strings::getAppString(Strings::AppName)));
	lFolderPath->setMinimumWidth(MIN_ADVANCED_TAB_LABEL_WIDTH);
	lFolderPath->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	leFolderPath = new QLineEdit(this);
	leFolderPath->setObjectName("folderPath");
	leFolderPath->setEnabled(false);
	QPushButton *pbMoveFolder = new QPushButton(tr("Move..."), this);
	pbMoveFolder->setObjectName("moveFolder");

	QHBoxLayout *blFolderPath = new QHBoxLayout();
	blFolderPath->addWidget(lFolderPath);
	blFolderPath->addWidget(leFolderPath, 1, Qt::AlignVCenter);
	blFolderPath->addWidget(pbMoveFolder, 0, Qt::AlignLeft | Qt::AlignVCenter);

	// Autostart
	QHBoxLayout *blAutostart = new QHBoxLayout();
	QLabel *lAligner2 = new QLabel(this);
	lAligner2->setMinimumWidth(MIN_ADVANCED_TAB_LABEL_WIDTH);
	lAligner2->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	cAutostart = new QCheckBox(
		QString(tr("Start % on system startup"))
			.arg(Strings::getAppString(Strings::AppFullName))
		, this);
	cAutostart->setObjectName("autostart");
	blAutostart->addWidget(lAligner2);
	blAutostart->addWidget(cAutostart);
	blAutostart->addStretch(1);

	// Language
	QLabel *lLanguage = new QLabel(tr("Language:"), this);
	lLanguage->setMinimumWidth(MIN_ADVANCED_TAB_LABEL_WIDTH);
	lLanguage->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	cbLanguage = new QComboBox(this);
	cbLanguage->setObjectName("language");
	cbLanguage->setMinimumWidth(ADVANCED_TAB_CONTROL_FIXED_WIDTH);
	QHBoxLayout *blLanguage = new QHBoxLayout();
	blLanguage->addWidget(lLanguage);
	blLanguage->addWidget(cbLanguage, 0, Qt::AlignLeft | Qt::AlignVCenter);
	blLanguage->addStretch(1);


	mainLayout->addLayout(blNotifications);
	mainLayout->addSpacing(8);
	mainLayout->addLayout(blSync);
	mainLayout->addSpacing(8);
	mainLayout->addLayout(blFolderPath);
	mainLayout->addSpacing(8);
	mainLayout->addLayout(blAutostart);
	mainLayout->addSpacing(8);
	mainLayout->addLayout(blLanguage);
	mainLayout->addStretch(1);

	setFromSettings();

	QMetaObject::connectSlotsByName(this);
}

void AdvancedWidget::setFromSettings()
{
	cNotifications->setChecked(
		Settings::instance().get(Settings::desktopNotifications).toBool());

	leFolderPath->setText(QDir::toNativeSeparators(
		Settings::instance().get(Settings::folderPath).toString()));

	cAutostart->setChecked(
		Settings::instance().get(Settings::autostart).toBool());


	// Language combo box content and current value
	QList<int> langs = Settings::supportedLanguages();
	for (int i = 0; i < langs.size(); i++)
	{
		cbLanguage->addItem(
			QLocale::languageToString((QLocale::Language)langs.at(i))
			, langs.at(i));
	}

	cbLanguage->setCurrentIndex(cbLanguage->
		findData(Settings::instance().get(Settings::language).toInt()));
}

void AdvancedWidget::on_desktopNotifications_toggled(bool checked)
{
	Settings::instance().set(Settings::desktopNotifications, checked);
}

void AdvancedWidget::on_moveFolder_clicked(bool checked)
{
	QString path = QFileDialog::getExistingDirectory(this
		, QString(tr("Choose Folder to Move %1 Into"))
		.arg(Strings::getAppString(Strings::AppName))
		, leFolderPath->text()
		, QFileDialog::ShowDirsOnly);

	if (path != QString())
	{
		leFolderPath->setText(QDir::toNativeSeparators(path));
	}
}

void AdvancedWidget::on_folderPath_textChanged(const QString &text)
{
	Settings::instance().set(Settings::folderPath, text);
}

void AdvancedWidget::on_autostart_toggled(bool checked)
{
	Settings::instance().set(Settings::autostart, checked);
}

void AdvancedWidget::on_language_currentIndexChanged(int index)
{
	Settings::instance().
		set(Settings::language, cbLanguage->itemData(index).toInt());
}

void AdvancedWidget::on_selectiveSync_clicked(bool checked)
{
	SelectiveSyncDialog d;
	d.exec();
}

}
