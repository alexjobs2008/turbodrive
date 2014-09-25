#include "tutorialplayer.h"
#include "tutorialmacstep1.h"
#include "tutorialmacstep2.h"
#include "tutorialmacstep3.h"
#include "tutorialmacstep4.h"
#include "tutorialwinstep1.h"
#include "tutorialwinstep2.h"
#include "tutorialwinstep3.h"
#include "tutorialwinstep4.h"

namespace Drive
{

const int TutorialDialogClosedFromCode = -111;

TutorialPlayer::TutorialPlayer(QObject *parent) :
    QObject(parent),
    currentStep(-1)
{
}

TutorialPlayer& TutorialPlayer::instance()
{
    static TutorialPlayer& self =
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN)
            GetWinTutorial();
#else
            GetMacTutorial();
#endif
    return self;
}

TutorialPlayer& TutorialPlayer::GetMacTutorial()
{
    static TutorialPlayer player;

    player.steps.append((new TutorialMacStep1())->stepItf());
    player.steps.append((new TutorialMacStep2())->stepItf());
    player.steps.append((new TutorialMacStep3())->stepItf());
    player.steps.append((new TutorialMacStep4())->stepItf());

    player.connectSignals();

    return player;
}

TutorialPlayer& TutorialPlayer::GetWinTutorial()
{
    static TutorialPlayer player;

    player.steps.append((new TutorialWinStep1())->stepItf());
    player.steps.append((new TutorialWinStep2())->stepItf());
    player.steps.append((new TutorialWinStep3())->stepItf());
    player.steps.append((new TutorialWinStep4())->stepItf());

    player.connectSignals();

    return player;
}

void TutorialPlayer::start()
{
    currentStep = 0;
    steps[currentStep]->dialog()->open();
}

void TutorialPlayer::next()
{
    if (currentStep == steps.size() - 1)
    {
        steps[currentStep]->dialog()->done(TutorialDialogClosedFromCode);
        emit finished();
    }
    else if (currentStep >= 0 && currentStep < steps.size() - 1)
    {
        steps[currentStep]->dialog()->done(TutorialDialogClosedFromCode);
        currentStep++;
        steps[currentStep]->dialog()->open();
    }

}

void TutorialPlayer::back()
{
    if (currentStep > 0 && currentStep < steps.size())
    {
        steps[currentStep]->dialog()->done(TutorialDialogClosedFromCode);
        currentStep--;
        steps[currentStep]->dialog()->open();
    }

}

void TutorialPlayer::finish()
{
    if (currentStep >= 0 && currentStep < steps.size())
    {
        steps[currentStep]->dialog()->done(TutorialDialogClosedFromCode);
    }

    emit finished();
}

void TutorialPlayer::dialogFinished(int result)
{
    if (result != TutorialDialogClosedFromCode)
    {
        emit finished();
    }
}

void TutorialPlayer::connectSignals()
{
    for (int i = 0; i < steps.size(); i++)
    {
        TutorialStepInterface *step = steps[i];
        QDialog *dialog = step->dialog();
        dialog->setFocusPolicy(Qt::NoFocus);

        connect(step, &TutorialStepInterface::cancel, this, &TutorialPlayer::finish);
        connect(step, &TutorialStepInterface::back, this, &TutorialPlayer::back);
        connect(step, &TutorialStepInterface::next, this, &TutorialPlayer::next);
        connect(dialog, &QDialog::finished, this, &TutorialPlayer::dialogFinished);
    }
}

}
