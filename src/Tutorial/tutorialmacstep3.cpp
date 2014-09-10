#include "tutorialmacstep3.h"
#include "ui_tutorialmacstep3.h"

namespace Drive
{

TutorialMacStep3::TutorialMacStep3(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TutorialMacStep3),
    step(new TutorialStepInterface(this))
{
    ui->setupUi(this);
    step->init(ui->cancelButton, ui->backButton, ui->nextButton);
}

TutorialMacStep3::~TutorialMacStep3()
{
    delete ui;
}

}
