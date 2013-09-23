#include "AdvancedWidget.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>

AdvancedWidget::AdvancedWidget(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    QLabel *label = new QLabel(tr("Advanced"), this);

    mainLayout->addWidget(label, 1);
}