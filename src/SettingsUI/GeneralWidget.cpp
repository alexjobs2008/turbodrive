#include "GeneralWidget.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>

GeneralWidget::GeneralWidget(QWidget *parent)
    : QFrame(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QCheckBox *cbNotifications =
        new QCheckBox(tr("Show desktop notifications"), this);
    cbNotifications->setChecked(true);

    QCheckBox *cbAutostart =
        new QCheckBox(tr("Start Assistent Drive on system startup"), this);

    cbAutostart->setChecked(true);

    //QLabel *label = new QLabel(tr("General"), this);

    mainLayout->addWidget(cbNotifications);
    mainLayout->addWidget(cbAutostart);
    mainLayout->addStretch(1);
}