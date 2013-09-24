#include "AdvancedWidget.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>

AdvancedWidget::AdvancedWidget(QWidget *parent)
    : QFrame(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QGroupBox *gbLocation =
        new QGroupBox(tr("Assistent Drive folder location"), this);

    QLineEdit *leLocation = new QLineEdit(gbLocation);
    QPushButton *pbLocation = new QPushButton(tr("Move..."), gbLocation);
    QHBoxLayout *blLocation = new QHBoxLayout(gbLocation);
    
    blLocation->addWidget(leLocation);
    blLocation->addWidget(pbLocation);

    QGroupBox *gbSync = new QGroupBox(tr("Synchronization"), this);        
    
    QLabel *lSync =
        new QLabel(tr("Select which folders and files to synchronize: "), gbSync);
    
    QPushButton *pbSync = new QPushButton(QString("%1%2").arg(tr("Selective Sync")).arg(QChar(2026)), gbSync);
    
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
}