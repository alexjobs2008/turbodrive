#ifndef TUTORIALWINSTEP1_H
#define TUTORIALWINSTEP1_H

#include <QDialog>
#include "tutorialstepinterface.h"

namespace Ui {
class TutorialWinStep1;
}

namespace Drive
{

class TutorialWinStep1 : public QDialog
{
    Q_OBJECT

public:
    explicit TutorialWinStep1(QWidget *parent = 0);
    ~TutorialWinStep1();
    TutorialStepInterface *stepItf();

private:
    Ui::TutorialWinStep1 *ui;
    TutorialStepInterface *step;
};

}

#endif // TUTORIALWINSTEP1_H
