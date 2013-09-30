#include "AdvancedWidget.h"
#include "Util/AppStrings.h"
#include "Settings/settings.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QCheckBox>

#define MIN_ADVANCED_TAB_LABEL_WIDTH 96
#define ADVANCED_TAB_CONTROL_FIXED_WIDTH 204

AdvancedWidget::AdvancedWidget(QWidget *parent)
    : QFrame(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QGroupBox *gbLocation =
        new QGroupBox(QString("%1 %2 %3:")
            .arg(Strings::companyName)
            .arg(Strings::appName)
            .arg(tr("folder location"))
            , this);

    QLineEdit *leLocation = new QLineEdit(gbLocation);
    leLocation->setText(Settings::instance().folder());
    QPushButton *pbLocation = new QPushButton(tr("Move..."), gbLocation);
    pbLocation->setObjectName("pbMove");
    QHBoxLayout *blLocation = new QHBoxLayout(gbLocation);
    
    blLocation->addWidget(leLocation);
    blLocation->addWidget(pbLocation);

    QGroupBox *gbSync = new QGroupBox(tr("Synchronization"), this);        
    
    QLabel *lSync =
        new QLabel(tr("Select which folders and files to synchronize: "), gbSync);
    
    QPushButton *pbSync = new QPushButton(tr("Selective Sync..."), gbSync);
    
    QHBoxLayout *blSync = new QHBoxLayout(gbSync);
    
    blSync->addWidget(lSync);
    blSync->addWidget(pbSync);
    //blSync->addStretch(1);

    QGroupBox *gbLanguage = new QGroupBox(tr("Language"), this);
    QComboBox *cbLanguage = new QComboBox(gbLanguage);
    cbLanguage->addItem(tr("English"), 0);
    cbLanguage->addItem(trUtf8("Russian"), 1);    

    QHBoxLayout *blLanguage = new QHBoxLayout(gbLanguage);
    blLanguage->addWidget(cbLanguage, 1);

    mainLayout->addWidget(gbLocation);
    mainLayout->addWidget(gbSync);
    mainLayout->addWidget(gbLanguage);
    mainLayout->addStretch(1);

    QMetaObject::connectSlotsByName(this);
}

void AdvancedWidget::on_pbMove_clicked(bool checked)
{
    QString path = QFileDialog::getExistingDirectory(0, "", "", QFileDialog::ShowDirsOnly);
}

AdvancedWidgetV2::AdvancedWidgetV2(QWidget *parent)
    : QFrame(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *lLocation = new QLabel(tr("Drive folder:"), this);
    lLocation->setMinimumWidth(MIN_ADVANCED_TAB_LABEL_WIDTH);
    lLocation->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    leLocation = new QLineEdit(this);
    leLocation->setEnabled(false);
    leLocation->setText(
        QDir::toNativeSeparators(Settings::instance().folder()));
    QPushButton *pbLocation = new QPushButton(tr("Move..."), this);
    pbLocation->setObjectName("pbMove");

    QHBoxLayout *blLocation = new QHBoxLayout();

    blLocation->addWidget(lLocation);
    blLocation->addWidget(leLocation, 1, Qt::AlignVCenter);
    blLocation->addWidget(pbLocation, 0, Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *lSync = new QLabel(tr("Selective Sync:"), this);
    lSync->setMinimumWidth(MIN_ADVANCED_TAB_LABEL_WIDTH);
    lSync->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QPushButton *pbSync = new QPushButton(
        tr("Select which folders and files to sync..."), this);

    pbSync->setMinimumWidth(ADVANCED_TAB_CONTROL_FIXED_WIDTH);
    pbSync->setMaximumWidth(ADVANCED_TAB_CONTROL_FIXED_WIDTH);

    QHBoxLayout *blSync = new QHBoxLayout();

    blSync->addWidget(lSync);
    blSync->addWidget(pbSync, 0, Qt::AlignLeft | Qt::AlignVCenter);
    blSync->addStretch(1);

    QLabel *lLanguage = new QLabel(tr("Language:"), this);
    lLanguage->setMinimumWidth(MIN_ADVANCED_TAB_LABEL_WIDTH);
    lLanguage->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QComboBox *cbLanguage = new QComboBox(this);
    cbLanguage->setMinimumWidth(ADVANCED_TAB_CONTROL_FIXED_WIDTH);
    cbLanguage->addItem(tr("English"), 0);
    cbLanguage->addItem(trUtf8("Russian"), 1);    
    QHBoxLayout *blLanguage = new QHBoxLayout();
    blLanguage->addWidget(lLanguage);
    blLanguage->addWidget(cbLanguage, 0, Qt::AlignLeft | Qt::AlignVCenter);
    blLanguage->addStretch(1);

    // Show desktop notifications
    QHBoxLayout *blNotifications = new QHBoxLayout();
    QLabel *lAligner = new QLabel(this);
    lAligner->setMinimumWidth(MIN_ADVANCED_TAB_LABEL_WIDTH);
    lAligner->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QCheckBox *cNotifications =
        new QCheckBox(tr("Show desktop notifications"), this);
    cNotifications->setObjectName("cNotifications");
    cNotifications->setChecked(true);
    blNotifications->addWidget(lAligner);
    blNotifications->addWidget(cNotifications);
    blNotifications->addStretch(1);
    
    // Autoexec
    QHBoxLayout *blAutostart = new QHBoxLayout();
    QLabel *lAligner2 = new QLabel(this);
    lAligner2->setMinimumWidth(MIN_ADVANCED_TAB_LABEL_WIDTH);
    lAligner2->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QCheckBox *cAutostart =
        new QCheckBox(tr("Start Drive on system startup"), this);
    cAutostart->setChecked(Settings::instance().autostart());
    cAutostart->setObjectName("cAutostart");
    cAutostart->setChecked(Settings::instance().autostart());
    blAutostart->addWidget(lAligner2);
    blAutostart->addWidget(cAutostart);
    blAutostart->addStretch(1);
        
    mainLayout->addLayout(blNotifications);
    mainLayout->addSpacing(8);
    mainLayout->addLayout(blSync);
    mainLayout->addSpacing(8);
    mainLayout->addLayout(blLocation);    
    mainLayout->addSpacing(8);
    mainLayout->addLayout(blAutostart);
    mainLayout->addSpacing(8);
    mainLayout->addLayout(blLanguage);
    mainLayout->addStretch(1);    

    QMetaObject::connectSlotsByName(this);
}

void AdvancedWidgetV2::on_pbMove_clicked(bool checked)
{
    QString path = QFileDialog::getExistingDirectory(this
        , QString(tr("Choose Folder to Move %1 Into")).arg(Strings::appName)        
        , Settings::instance().folder()
        , QFileDialog::ShowDirsOnly);

    if (path != QString())
    {
        leLocation->setText(QDir::toNativeSeparators(path));
    }
}
