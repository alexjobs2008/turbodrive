#ifndef TUTORIALMACSTEP3_H
#define TUTORIALMACSTEP3_H

#include <QDialog>
#include "tutorialstepinterface.h"

namespace Ui {
class TutorialMacStep3;
}

namespace Drive
{

class TutorialMacStep3 : public QDialog
{
    Q_OBJECT

public:
    explicit TutorialMacStep3(QWidget *parent = 0);
    ~TutorialMacStep3();
    TutorialStepInterface *stepItf() { return step; }

private:
    Ui::TutorialMacStep3 *ui;
    TutorialStepInterface *step;
};

}

#endif // TUTORIALMACSTEP3_H
