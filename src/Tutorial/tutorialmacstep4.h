#ifndef TUTORIALMACSTEP4_H
#define TUTORIALMACSTEP4_H

#include <QDialog>
#include "tutorialstepinterface.h"

namespace Ui {
class TutorialMacStep4;
}

namespace Drive
{

class TutorialMacStep4 : public QWidget
{
    Q_OBJECT

public:
    explicit TutorialMacStep4(QWidget *parent = 0);
    ~TutorialMacStep4();
    TutorialStepInterface *stepItf() { return step; }

private:
    Ui::TutorialMacStep4 *ui;
    TutorialStepInterface *step;
};

#endif // TUTORIALMACSTEP4_H

}
