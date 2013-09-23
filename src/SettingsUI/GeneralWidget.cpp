#include "GeneralWidget.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>

GeneralWidget::GeneralWidget(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    QLabel *label = new QLabel(tr("General"), this);

    mainLayout->addWidget(label, 1);
}