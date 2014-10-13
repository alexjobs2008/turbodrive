#ifndef TUTORIALWINSTEP2_H
#define TUTORIALWINSTEP2_H

#include <QDialog>
#include "tutorialstepinterface.h"

namespace Ui {
class TutorialWinStep2;
}

namespace Drive
{

class TutorialWinStep2 : public QWidget
{
    Q_OBJECT

public:
    explicit TutorialWinStep2(QWidget *parent = 0);
    ~TutorialWinStep2();
    TutorialStepInterface *stepItf() { return step; }

private slots:
    void pushedGooglePlay();
    void pushedAppleAppStore();

private:
    Ui::TutorialWinStep2 *ui;
    TutorialStepInterface *step;
};

}

#endif // TUTORIALWINSTEP2_H
