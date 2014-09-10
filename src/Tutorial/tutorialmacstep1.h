#ifndef TUTORIALMACSTEP1_H
#define TUTORIALMACSTEP1_H

#include <QDialog>
#include "tutorialstepinterface.h"

namespace Ui {
class TutorialMacStep1;
}

namespace Drive
{

class TutorialMacStep1 : public QDialog
{
    Q_OBJECT

public:
    explicit TutorialMacStep1(QWidget *parent = 0);
    ~TutorialMacStep1();
    TutorialStepInterface *stepItf() { return step; }

private:
    Ui::TutorialMacStep1 *ui;
    TutorialStepInterface *step;
};

}

#endif // TUTORIALMACSTEP1_H
