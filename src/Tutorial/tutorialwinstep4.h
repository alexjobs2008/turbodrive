#ifndef TUTORIALWINSTEP4_H
#define TUTORIALWINSTEP4_H

#include <QDialog>
#include "tutorialstepinterface.h"

namespace Ui {
class TutorialWinStep4;
}

namespace Drive
{

class TutorialWinStep4 : public QDialog
{
    Q_OBJECT

public:
    explicit TutorialWinStep4(QWidget *parent = 0);
    ~TutorialWinStep4();
    TutorialStepInterface *stepItf() { return step; }

private slots:
    void pushedSettings();

private:
    Ui::TutorialWinStep4 *ui;
    TutorialStepInterface *step;
};

}

#endif // TUTORIALWINSTEP4_H
