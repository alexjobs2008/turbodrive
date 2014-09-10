#ifndef TUTORIALMACSTEP2_H
#define TUTORIALMACSTEP2_H

#include <QDialog>
#include "tutorialstepinterface.h"

namespace Ui {
class TutorialMacStep2;
}

namespace Drive
{

class TutorialMacStep2 : public QDialog
{
    Q_OBJECT

public:
    explicit TutorialMacStep2(QWidget *parent = 0);
    ~TutorialMacStep2();
    TutorialStepInterface *stepItf() { return step; }

private slots:
    void pushedGooglePlay();
    void pushedAppleAppStore();

private:
    Ui::TutorialMacStep2 *ui;
    TutorialStepInterface *step;
};

}

#endif // TUTORIALMACSTEP2_H
