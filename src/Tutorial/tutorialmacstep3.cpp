#include "tutorialmacstep3.h"
#include "ui_tutorialmacstep3.h"

namespace Drive
{

TutorialMacStep3::TutorialMacStep3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TutorialMacStep3),
    step(new TutorialStepInterface(this))
{
    ui->setupUi(this);
    step->init(ui->cancelButton, ui->backButton, ui->nextButton);

    ui->cancelButton->setFocusPolicy(Qt::NoFocus);
    ui->nextButton->setFocusPolicy(Qt::NoFocus);
    ui->backButton->setFocusPolicy(Qt::NoFocus);
}

TutorialMacStep3::~TutorialMacStep3()
{
    delete ui;
}

}
