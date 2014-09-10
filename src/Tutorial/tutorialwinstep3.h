#ifndef TUTORIALWINSTEP3_H
#define TUTORIALWINSTEP3_H

#include <QDialog>
#include "tutorialstepinterface.h"

namespace Ui {
class TutorialWinStep3;
}

namespace Drive
{

class TutorialWinStep3 : public QDialog
{
    Q_OBJECT

public:
    explicit TutorialWinStep3(QWidget *parent = 0);
    ~TutorialWinStep3();
    TutorialStepInterface *stepItf() { return step; }

private:
    Ui::TutorialWinStep3 *ui;
    TutorialStepInterface *step;
};

}

#endif // TUTORIALWINSTEP3_H
