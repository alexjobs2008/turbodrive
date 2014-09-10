#include "tutorialwinstep3.h"
#include "ui_tutorialwinstep3.h"

namespace Drive
{

TutorialWinStep3::TutorialWinStep3(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TutorialWinStep3),
    step(new TutorialStepInterface(this))
{
    ui->setupUi(this);
    step->init(0, 0, ui->nextButton);
}

TutorialWinStep3::~TutorialWinStep3()
{
    delete ui;
}

}
