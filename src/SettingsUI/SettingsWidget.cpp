#include "SettingsWidget.h"

#include "APIClient/ApiTypes.h"
#include "QsLog/QsLog.h"
#include "Settings/settings.h"
#include "UtilUI/ExtraLabels.h"
#include "UtilUI/StyleSheetReader.h"

#include "AccountWidget.h"
#include "ConnectionWidget.h"
#include "AdvancedWidget.h"
#include "AboutWidget.h"

#include <QtCore/qglobal.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QStringListModel>
#include <QtCore/QLocale>
#include <QtGui/QPainter>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>

#define SETTINGS_PAGE_GENERAL 0
#define SETTINGS_PAGE_ACCOUNT 1
#define SETTINGS_PAGE_CONNECTION 2
#define SETTINGS_PAGE_ADVANCED 3
#define SETTINGS_PAGE_ABOUT 4

inline void initResources() { Q_INIT_RESOURCE(settingsUI); }

namespace Drive
{

const QString SettingsWidget::linkHelp =
	QLatin1String("http://disk.mts.by/");

SettingsWidget& SettingsWidget::instance()
{
	static SettingsWidget myself;
	return myself;
}

void SettingsWidget::onProfileDataUpdated(const ProfileData& profileData)
{
	accountWidget->setProfileData(profileData);
}

SettingsWidget::SettingsWidget(QWidget *parent)
	: QFrame(parent)
{
	QLOG_TRACE() << "creating Settings window";

	initResources();

	// avoid app close on window close
	setAttribute(Qt::WA_DeleteOnClose, false);
	setAttribute(Qt::WA_QuitOnClose, false);

	setWindowFlags(Qt::CustomizeWindowHint
		| Qt::WindowTitleHint
		| Qt::WindowCloseButtonHint);

#ifdef Q_OS_MACX
	setMinimumWidth(414);
#endif

#ifdef Q_OS_WIN
	setMinimumWidth(404);
#endif

	setupListView();

	QVBoxLayout *layoutMain = new QVBoxLayout(this);
#ifdef Q_OS_MACX
	layoutMain->setContentsMargins(0, 6, 0, 0);
#endif

#ifdef Q_OS_WIN
	layoutMain->setContentsMargins(0, 0, 0, 0);
#endif

	stackedWidget = new QStackedWidget(this);

	accountWidget = new AccountWidget(this);
	connect(accountWidget, &AccountWidget::openFolder, this, &SettingsWidget::openFolder);
	connect(accountWidget, &AccountWidget::logout, this, &SettingsWidget::logout);
	connect(accountWidget, &AccountWidget::resetPassword, this, &SettingsWidget::resetPassword);


	ConnectionWidget *connectionWidget = new ConnectionWidget(this);
	AdvancedWidget *advancedWidget = new AdvancedWidget(this);
	AboutWidget *aboutWidget = new AboutWidget(stackedWidget);

	stackedWidget->addWidget(accountWidget);
	stackedWidget->addWidget(connectionWidget);
	stackedWidget->addWidget(advancedWidget);
	stackedWidget->addWidget(aboutWidget);

	QFrame *separator = new QFrame(this);
	separator->setObjectName("separator");

	QFrame *fButtonBox = new QFrame(this);
	fButtonBox->setObjectName("buttonBox");
	QHBoxLayout *blButtonBox = new QHBoxLayout(fButtonBox);
	blButtonBox->setContentsMargins(8, 8, 8, 8);
	blButtonBox->setSpacing(8);

	CommonUI::LinkLabel *llHelp =
		new CommonUI::LinkLabel(tr("Help"), linkHelp, this);
	llHelp->setObjectName("help");

	QPushButton *pbOK = new QPushButton(tr("OK"), this);
	pbOK->setObjectName("OK");
	QPushButton *pbCancel = new QPushButton(tr("Cancel"), this);
	pbCancel->setObjectName("cancel");
	pbApply = new QPushButton(tr("Apply"), this);
	pbApply->setObjectName("apply");
	pbApply->setEnabled(false);

	blButtonBox->addWidget(llHelp);
	blButtonBox->addStretch(1);
	blButtonBox->addWidget(pbOK);
	blButtonBox->addWidget(pbCancel);
	blButtonBox->addWidget(pbApply);

	layoutMain->setSpacing(0);
	layoutMain->addWidget(tabs);
	layoutMain->addSpacing(4);
	layoutMain->addWidget(stackedWidget);
	layoutMain->addSpacing(2);
	layoutMain->addWidget(separator);
	layoutMain->addWidget(fButtonBox);

	connect(&Settings::instance(), &Settings::dirtyStateChanged,
			this, &SettingsWidget::onSettingsDirtyChanged);

	QMetaObject::connectSlotsByName(this);

	//CommonUI::StyleSheetReader::setStyleSheetFor(this);

	//QFont f("Segoe UI", 10);
	QFont f("MS Shell Dlg", 10);
	setFont(f);
}

SettingsWidget::~SettingsWidget()
{
}

void SettingsWidget::resizeEvent(QResizeEvent *event)
{
	QLOG_INFO() << event->size().width() << ", " << event->size().height();
	event->ignore();
}

void SettingsWidget::setupListView()
{
	tabs = new QListWidget(this);
	tabs->setObjectName("tabs");
	tabs->setFlow(QListView::LeftToRight);
	tabs->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tabs->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tabs->setMaximumHeight(64);
	tabs->setIconSize(QSize(32, 32));

	tabs->setItemDelegate(new Delegate(this));

	tabs->setCurrentItem(new QListWidgetItem(QIcon(":/account.png"),
		tr("Account"), tabs, SETTINGS_PAGE_ACCOUNT));
//	new QListWidgetItem(QIcon(":/connection.png"),
//		tr("Connection"), tabs, SETTINGS_PAGE_CONNECTION);
	new QListWidgetItem(QIcon(":/advanced.png"),
		tr("Advanced"), tabs, SETTINGS_PAGE_ADVANCED);
	new QListWidgetItem(QIcon(":/about.png"),
		tr("About"), tabs, SETTINGS_PAGE_ABOUT);


}

void SettingsWidget::on_tabs_currentItemChanged(QListWidgetItem *current,
												QListWidgetItem *previous)
{
	stackedWidget->setCurrentIndex(current->type() - 1);
}

void SettingsWidget::onSettingsDirtyChanged(bool isDirty)
{
	pbApply->setEnabled(isDirty);
}

void SettingsWidget::on_help_linkActivated(const QString& link)
{
	Q_UNUSED(link)
	QDesktopServices::openUrl(QString(link)
		.arg(QCoreApplication::applicationVersion())
		.arg(QLocale::languageToString((QLocale::Language)
			Settings::instance().get(Settings::language).toInt()))
		.arg(stackedWidget->currentIndex()));
}

void SettingsWidget::on_OK_clicked(bool checked)
{
	Q_UNUSED(checked)
	Settings::instance().apply();
	close();
}

void SettingsWidget::on_cancel_clicked(bool checked)
{
	Q_UNUSED(checked)
	Settings::instance().cancel();
	close();
}

void SettingsWidget::on_apply_clicked(bool checked)
{
	Q_UNUSED(checked)
	Settings::instance().apply();
}

Delegate::Delegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	QStringListModel *helperModel = new QStringListModel(this);
	QStringList helperData;
	helperData << QString();
	helperModel->setStringList(helperData);

	helperIndex = helperModel->index(0, 0);

	label = new QLabel("", 0);
}

Delegate::~Delegate()
{
	delete label;
}

QSize Delegate::sizeHint(const QStyleOptionViewItem & option,
						const QModelIndex & index) const
{
	return QSize(128, 48);
}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
					const QModelIndex & index) const
{
//	if (index.column() == 0)
//		printStyleOptionStates(option, index.column());

	QStyleOptionViewItem optionCopy = option;

	// no need to draw focus rect and don't draw grayed out
	optionCopy.state = optionCopy.state & ~QStyle::State_HasFocus | QStyle::State_Active;

#ifdef Q_OS_MACX
	QStyledItemDelegate::paint(painter, optionCopy, helperIndex);
#endif

	QIcon icon =
		index.model()->data(index, Qt::DecorationRole).value<QIcon>();

	QRect iconRect(option.rect);
	iconRect.adjust(0, 8, 0, 0);

	icon.paint(painter, iconRect, Qt::AlignTop | Qt::AlignHCenter);

	painter->drawText(option.rect
		, index.model()->data(index, Qt::DisplayRole).toString()
		, Qt::AlignBottom | Qt::AlignHCenter);


#ifdef Q_OS_WIN
	QStyledItemDelegate::paint(painter, optionCopy, helperIndex);
#endif
}

void Delegate::printStyleOptionStates(const QStyleOptionViewItem & option, int i)
{
	QLOG_INFO() << "=== " << i << " ========================================";

	if (option.state & QStyle::State_None)
		QLOG_INFO() << "QStyle::State_None";

	if (option.state & QStyle::State_Active)
		QLOG_INFO() << "QStyle::State_Active";

	if (option.state & QStyle::State_AutoRaise)
		QLOG_INFO() << "QStyle::State_AutoRaise";

	if (option.state & QStyle::State_Children)
		QLOG_INFO() << "QStyle::State_Children";

	if (option.state & QStyle::State_DownArrow)
		QLOG_INFO() << "QStyle::State_DownArrow";

	if (option.state & QStyle::State_Editing)
		QLOG_INFO() << "QStyle::State_Editing";

	if (option.state & QStyle::State_Enabled)
		QLOG_INFO() << "QStyle::State_Enabled";

	if (option.state & QStyle::State_MouseOver)
		QLOG_INFO() << "QStyle::State_MouseOver";

	if (option.state & QStyle::State_HasFocus)
		QLOG_INFO() << "QStyle::State_HasFocus";

	if (option.state & QStyle::State_Horizontal)
		QLOG_INFO() << "QStyle::State_Horizontal";

	if (option.state & QStyle::State_KeyboardFocusChange)
		QLOG_INFO() << "QStyle::State_KeyboardFocusChange";

	if (option.state & QStyle::State_NoChange)
		QLOG_INFO() << "QStyle::State_NoChange";

	if (option.state & QStyle::State_Off)
		QLOG_INFO() << "QStyle::State_Off";

	if (option.state & QStyle::State_On)
		QLOG_INFO() << "QStyle::State_On";

	if (option.state & QStyle::State_Raised)
		QLOG_INFO() << "QStyle::State_Raised";

	if (option.state & QStyle::State_ReadOnly)
		QLOG_INFO() << "QStyle::State_ReadOnly";

	if (option.state & QStyle::State_Selected)
		QLOG_INFO() << "QStyle::State_Selected";

	if (option.state & QStyle::State_Item)
		QLOG_INFO() << "QStyle::State_Item";

	if (option.state & QStyle::State_Open)
		QLOG_INFO() << "QStyle::State_Open";

	if (option.state & QStyle::State_Sibling)
		QLOG_INFO() << "QStyle::State_Sibling";

	if (option.state & QStyle::State_Sunken)
		QLOG_INFO() << "QStyle::State_Sunken";

	if (option.state & QStyle::State_UpArrow)
		QLOG_INFO() << "QStyle::State_UpArrow";

	if (option.state & QStyle::State_Mini)
		QLOG_INFO() << "QStyle::State_Mini";

	if (option.state & QStyle::State_Small)
		QLOG_INFO() << "QStyle::State_Small";

	QLOG_INFO() << "--------------------------------------------------------";


}

}
