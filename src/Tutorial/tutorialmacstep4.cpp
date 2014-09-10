#include "tutorialmacstep4.h"
#include "ui_tutorialmacstep4.h"

namespace Drive
{

TutorialMacStep4::TutorialMacStep4(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TutorialMacStep4),
    step(new TutorialStepInterface(this))
{
    ui->setupUi(this);
    step->init(ui->cancelButton, ui->backButton, ui->nextButton);
}

TutorialMacStep4::~TutorialMacStep4()
{
    delete ui;
}

}
