#include "tutorialstepinterface.h"

namespace Drive
{

TutorialStepInterface::TutorialStepInterface(QWidget *parent) :
    QObject(parent)
{
}


void TutorialStepInterface::init(
        QPushButton *cancelButton, QPushButton *backButton, QPushButton *nextButton)
{
    if (cancelButton != 0)
        connect(cancelButton, &QPushButton::clicked, this, &TutorialStepInterface::cancel);
    if (backButton != 0)
        connect(backButton, &QPushButton::clicked, this, &TutorialStepInterface::back);
    if (nextButton != 0)
        connect(nextButton, &QPushButton::clicked, this, &TutorialStepInterface::next);
}

}
