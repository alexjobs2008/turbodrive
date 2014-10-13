#ifndef TUTORIALPLAYER_H
#define TUTORIALPLAYER_H

#include <QList>
#include <QObject>
#include <QPointer>
#include <QDialog>
#include <QVBoxLayout>
#include "tutorialstepinterface.h"

namespace Drive
{

class TutorialPlayer : public QObject
{
    Q_OBJECT

public:
    explicit TutorialPlayer(QObject *parent = 0);
    static TutorialPlayer& instance();

signals:
    void finished();

public slots:
    void start();

private slots:
    void next();
    void back();
    void finish();
    void dialogFinished(int);

private:
    void connectSignals();
    void disconnectSignals();
    void showStep();
    static TutorialPlayer& GetMacTutorial();
    static TutorialPlayer& GetWinTutorial();

private:
    QList<TutorialStepInterface*> steps;
    int currentStep;
    QDialog *dialog;
    QVBoxLayout *layout;
    QWidget *currentWidget;
};

}

#endif // TUTORIALPLAYER_H
