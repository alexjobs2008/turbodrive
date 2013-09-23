#include "AccountWidget.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>

AccountWidget::AccountWidget(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    QLabel *label = new QLabel(tr("Account"), this);

    mainLayout->addWidget(label, 1);
}