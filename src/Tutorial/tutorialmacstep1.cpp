#include "tutorialmacstep1.h"
#include "ui_tutorialmacstep1.h"

namespace Drive
{

TutorialMacStep1::TutorialMacStep1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TutorialMacStep1),
    step(new TutorialStepInterface(this))
{
    ui->setupUi(this);
    step->init(ui->cancelButton, 0, ui->nextButton);
    ui->cancelButton->setFocusPolicy(Qt::NoFocus);
    ui->nextButton->setFocusPolicy(Qt::NoFocus);
}

TutorialMacStep1::~TutorialMacStep1()
{
    delete ui;
}

}
