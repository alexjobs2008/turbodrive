#include "tutorialwinstep1.h"
#include "ui_tutorialwinstep1.h"

namespace Drive
{

TutorialWinStep1::TutorialWinStep1(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TutorialWinStep1)
{
    ui->setupUi(this);
}

TutorialWinStep1::~TutorialWinStep1()
{
    delete ui;
}

TutorialStepInterface *TutorialWinStep1::stepItf()
{
    step = new TutorialStepInterface(this);
    step->init(0, 0, ui->nextButton);
    return step;
}

}
