#include "SettingsWidget.h"
#include "QsLog/QsLog.h"

#include <QtCore/qglobal.h>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QStackedWidget>
#include <QtCore/QStringListModel>
#include <QtGui/QPainter>
#include <QtWidgets/QLabel>

SettingsWidget::SettingsWidget(QWidget *parent)
    : QFrame(parent)
{
    Q_INIT_RESOURCE(settingsUI);

    // avoid app close on window close
    setAttribute(Qt::WA_QuitOnClose, false);

    setWindowFlags(Qt::CustomizeWindowHint
        | Qt::WindowTitleHint
        | Qt::WindowCloseButtonHint);

    setMinimumWidth(404); // windows. TODO: mac

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    buttonBox->setContentsMargins(8, 8, 8, 8);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    setupListView();
    
    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setContentsMargins(0, 0, 0, 0);

    QStackedWidget *stackedWidget = new QStackedWidget(this);
    
    layoutMain->addWidget(tabs);
    layoutMain->addWidget(stackedWidget);
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
    QListWidget *list = new QListWidget(this);
    list->setFlow(QListView::LeftToRight);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setMaximumHeight(64);
    list->setIconSize(QSize(32, 32));

    list->setItemDelegate(new Delegate(this));

    new QListWidgetItem(QIcon(":/general.png"), tr("General"), list);
    new QListWidgetItem(QIcon(":/account.png"), tr("Account"), list);
    new QListWidgetItem(QIcon(":/connection.png"), tr("Connection"), list);
    new QListWidgetItem(QIcon(":/advanced.png"), tr("Advanced"), list);
    new QListWidgetItem(QIcon(":/about.png"), tr("About"), list);

    tabs = list;
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