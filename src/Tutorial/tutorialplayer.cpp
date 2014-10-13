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

TutorialPlayer::TutorialPlayer(QObject *parent) :
    QObject(parent),
    currentStep(-1),
    dialog(new QDialog()),
    layout(new QVBoxLayout(dialog)),
    currentWidget(0)
{
    layout->setSizeConstraint(QLayout::SetNoConstraint);
    dialog->setFocusPolicy(Qt::NoFocus);
    dialog->setStyleSheet("background-color: rgb(255, 255, 255);");
    dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dialog->setContentsMargins(0, 0, 0, 0);
    layout->setContentsMargins(0, 0, 0, 0);

    QIcon icon;
    icon.addPixmap(QPixmap(":/appicon/16.png"));
    icon.addPixmap(QPixmap(":/appicon/24.png"));
    icon.addPixmap(QPixmap(":/appicon/32.png"));
    icon.addPixmap(QPixmap(":/appicon/48.png"));
    icon.addPixmap(QPixmap(":/appicon/256.png"));
    dialog->setWindowIcon(icon);

    dialog->setWindowTitle("МТС Диск");

    // Dialog window closed event
    connect(dialog, &QDialog::finished, this, &TutorialPlayer::dialogFinished);
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

// Show step widget
void TutorialPlayer::showStep()
{
    // Remove widget that is shown
    if (currentWidget != 0)
    {
        // dialog->layout()->removeWidget(currentWidget);
        currentWidget->hide();
    }

    // Show current widget
    currentWidget = steps[currentStep]->widget();
    currentWidget->show();
    // dialog->layout()->addWidget(currentWidget);
}

void TutorialPlayer::start()
{
    currentStep = 0;
    showStep();
    QSize size = currentWidget->size();
    QPoint pos = currentWidget->pos();
    (void)pos;
#ifdef Q_OS_WIN
    size += QSize(24, 24);
#endif
    dialog->resize(size);
    dialog->setMinimumSize(size);
    dialog->setMaximumSize(size);
    dialog->adjustSize();
    dialog->open();
}

void TutorialPlayer::next()
{
    // Next step after last - close dialog
    if (currentStep == steps.size() - 1)
    {
        finish();
    }

    // Show next step
    else if (currentStep >= 0 && currentStep < steps.size() - 1)
    {
        currentStep++;
        showStep();
    }

}

void TutorialPlayer::back()
{
    if (currentStep > 0 && currentStep < steps.size())
    {
        currentStep--;
        showStep();
    }
}

void TutorialPlayer::finish()
{
    if (currentStep >= 0 && currentStep < steps.size())
    {
        if (currentWidget != 0)
        {
            currentWidget->hide();
            currentWidget = 0;
        }

        disconnectSignals();

        dialog->close();
        // dialog->done(0);
    }

    emit finished();
}

void TutorialPlayer::dialogFinished(int)
{
    finish();
}

void TutorialPlayer::connectSignals()
{
    for (int i = 0; i < steps.size(); i++)
    {
        TutorialStepInterface *step = steps[i];
        QWidget *widget = step->widget();

        // Connect button events
        connect(step, &TutorialStepInterface::cancel, this, &TutorialPlayer::finish);
        connect(step, &TutorialStepInterface::back, this, &TutorialPlayer::back);
        connect(step, &TutorialStepInterface::next, this, &TutorialPlayer::next);

        layout->addWidget(widget);
        widget->hide();
    }
}

void TutorialPlayer::disconnectSignals()
{
    for (int i = 0; i < steps.size(); i++)
    {
        TutorialStepInterface *step = steps[i];
        QWidget *widget = step->widget();

        // Connect button events
        /* disconnect(step, &TutorialStepInterface::cancel, this, &TutorialPlayer::finish);
        disconnect(step, &TutorialStepInterface::back, this, &TutorialPlayer::back);
        disconnect(step, &TutorialStepInterface::next, this, &TutorialPlayer::next); */

        layout->removeWidget(widget);
    }
}


}
