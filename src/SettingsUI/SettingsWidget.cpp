#include "SettingsWidget.h"
#include "QsLog/QsLog.h"
#include "GeneralWidget.h"
#include "AccountWidget.h"
#include "ConnectionWidget.h"
#include "AdvancedWidget.h"
#include "AboutWidget.h"

#include <QtCore/qglobal.h>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QStackedWidget>
#include <QtCore/QStringListModel>
#include <QtGui/QPainter>
#include <QtWidgets/QLabel>

#define SETTINGS_PAGE_GENERAL 0
#define SETTINGS_PAGE_ACCOUNT 1
#define SETTINGS_PAGE_CONNECTION 2
#define SETTINGS_PAGE_ADVANCED 3
#define SETTINGS_PAGE_ABOUT 4

SettingsWidget::SettingsWidget(QWidget *parent)
    : QFrame(parent)
{
    Q_INIT_RESOURCE(settingsUI);

    // avoid app close on window close
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

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    buttonBox->setContentsMargins(8, 8, 8, 8);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    setupListView();
    
    QVBoxLayout *layoutMain = new QVBoxLayout(this);
#ifdef Q_OS_MACX
    layoutMain->setContentsMargins(0, 6, 0, 0);
#endif
    
#ifdef Q_OS_WIN
    layoutMain->setContentsMargins(0, 0, 0, 0);
#endif

    stackedWidget = new QStackedWidget(this);

    GeneralWidget *generalWidget = new GeneralWidget(this);
    AccountWidget *accountWidget = new AccountWidget(this);
    ConnectionWidget *connectionWidget = new ConnectionWidget(this);
    AdvancedWidget *advancedWidget = new AdvancedWidget(this);
    AboutWidget *aboutWidget = new AboutWidget(stackedWidget);
    
    stackedWidget->addWidget(generalWidget);
    stackedWidget->addWidget(accountWidget);
    stackedWidget->addWidget(connectionWidget);
    stackedWidget->addWidget(advancedWidget);
    stackedWidget->addWidget(aboutWidget);

    connect(tabs, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
        this, SLOT(onTabChanged(QListWidgetItem*, QListWidgetItem*)));

    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    
    layoutMain->addWidget(tabs);
    layoutMain->addWidget(stackedWidget);
    layoutMain->addWidget(separator);
    layoutMain->addWidget(buttonBox);
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
    tabs->setFlow(QListView::LeftToRight);
    tabs->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabs->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabs->setMaximumHeight(64);
    tabs->setIconSize(QSize(32, 32));

    tabs->setItemDelegate(new Delegate(this));

    new QListWidgetItem(QIcon(":/general.png"),
        tr("General"), tabs, SETTINGS_PAGE_GENERAL);
    new QListWidgetItem(QIcon(":/account.png"),
        tr("Account"), tabs, SETTINGS_PAGE_ACCOUNT);
    new QListWidgetItem(QIcon(":/connection.png"),
        tr("Connection"), tabs, SETTINGS_PAGE_CONNECTION);
    new QListWidgetItem(QIcon(":/advanced.png"),
        tr("Advanced"), tabs, SETTINGS_PAGE_ADVANCED);
    new QListWidgetItem(QIcon(":/about.png"),
        tr("About"), tabs, SETTINGS_PAGE_ABOUT);
}

void SettingsWidget::onTabChanged(QListWidgetItem *current,
                                  QListWidgetItem *previous)
{
    stackedWidget->setCurrentIndex(current->type());
}


void SettingsWidget::accept()
{

}

void SettingsWidget::reject()
{
    close();
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
    return QSize(80, 48);
}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem & option,
                     const QModelIndex & index) const
{
    QIcon icon =
        index.model()->data(index, Qt::DecorationRole).value<QIcon>();

    QRect iconRect(option.rect);
    iconRect.adjust(0, 8, 0, 0);

    icon.paint(painter, iconRect, Qt::AlignTop | Qt::AlignHCenter);

    painter->drawText(option.rect
        , index.model()->data(index, Qt::DisplayRole).toString()
        , Qt::AlignBottom | Qt::AlignHCenter);

    // draw selection and hover overlay
    QStyledItemDelegate::paint(painter, option, QModelIndex());
}