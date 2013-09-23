#include "ConnectionWidget.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>

ConnectionWidget::ConnectionWidget(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    QLabel *label = new QLabel(tr("Connection"), this);

    mainLayout->addWidget(label, 1);
}